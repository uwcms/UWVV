//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//   ZZCategoryEmbedder.cc                                                  //
//                                                                          //
//   Finds the category of a 4l event based on the 2016 HZZ4l group         //
//       rules and embeds it as a userInt.                                  //
//                                                                          //
//   Author: Nate Woods, U. Wisconsin                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>
#include <string>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"


typedef reco::Candidate Cand;
typedef pat::CompositeCandidate CCand;
typedef pat::CompositeCandidateRef CCandRef;


class ZZCategoryEmbedder : public edm::stream::EDProducer<> 
{
 public:
  ZZCategoryEmbedder(const edm::ParameterSet& pset);
  virtual ~ZZCategoryEmbedder(){}
 private:
  // Methods
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  int getCategory(const CCand& cand, // cand do consider
                  const unsigned nep, const unsigned nem, // # e+, e-
                  const unsigned nmp, const unsigned nmm, // # mu+, mu-
                  const unsigned nJets, const unsigned nBJets) const;

  // Tag of final state collection
  edm::EDGetTokenT<edm::View<CCand> > srcToken;
  // And the whole lepton and jet collections
  edm::EDGetTokenT<edm::View<pat::Jet> > jetToken;
  edm::EDGetTokenT<edm::View<pat::Electron> > electronToken;
  edm::EDGetTokenT<edm::View<pat::Muon> > muonToken;

  // To select tight leptons
  const StringCutObjectSelector<Cand, true> leptonSelector;

  // B discriminator to use
  const std::string bDiscrimLabel;

  // B discriminator cut
  const float bDiscrimCut;
};


ZZCategoryEmbedder::ZZCategoryEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<CCand> >(iConfig.getParameter<edm::InputTag>("src"))),
  jetToken(consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jetSrc"))),
  electronToken(consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("electronSrc"))),
  muonToken(consumes<edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("muonSrc"))),
  leptonSelector(iConfig.exists("tightLepCut") ?
                 iConfig.getParameter<std::string>("tightLepCut") :
                 std::string("userFloat(\"ZZIDPassTight\") > 0.5 && userFloat(\"ZZIsoPass\") > 0.5")),
  bDiscrimLabel(iConfig.exists("bDiscriminator") ?
                 iConfig.getParameter<std::string>("bDiscriminator") :
                 std::string("pfCombinedInclusiveSecondaryVertexV2BJetTags")),
  bDiscrimCut(iConfig.exists("bDiscriminatorCut") ?
               iConfig.getParameter<double>("bDiscriminatorCut") :
               0.800)
{
  produces<std::vector<CCand> >();
}


void ZZCategoryEmbedder::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
  // count of each type of lepton (e+, e-, mu+, mu-)
  unsigned nep=0, nem=0, nmp=0, nmm=0;

  edm::Handle<edm::View<pat::Electron> > elecs;
  iEvent.getByToken(electronToken, elecs);

  for(size_t i = 0; i < elecs->size(); ++i)
    {
      if(leptonSelector(elecs->at(i)))
        {
          if(elecs->at(i).charge() > 0)
            ++nep;
          else
            ++nem;
        }
    }

  edm::Handle<edm::View<pat::Muon> > mus;
  iEvent.getByToken(muonToken, mus);

  for(size_t i = 0; i < mus->size(); ++i)
    {
      if(leptonSelector(mus->at(i)))
        {
          if(mus->at(i).charge() > 0)
            ++nmp;
          else
            ++nmm;
        }
    }

  edm::Handle<edm::View<pat::Jet> > jets;
  iEvent.getByToken(jetToken, jets);
  
  unsigned nJets = jets->size();
  unsigned nBJets = 0;
  for(size_t j = 0; j < nJets; ++j)
    {
      if(jets->at(j).bDiscriminator(bDiscrimLabel) > bDiscrimCut)
        ++nBJets;
    }

  
  std::unique_ptr<std::vector<CCand> > out(new std::vector<CCand>);

  edm::Handle<edm::View<CCand> > in;
  iEvent.getByToken(srcToken, in);

  for(size_t i = 0; i < in->size(); ++i)
    {
      out->push_back(in->at(i));
      
      out->back().addUserInt("ZZCategory", 
                             getCategory(out->back(), nep, nem, nmp, nmm, 
                                         nJets, nBJets));
    }

  iEvent.put(std::move(out));
}


int ZZCategoryEmbedder::getCategory(const CCand& cand, // cand do consider
                                    const unsigned nep, const unsigned nem, // # e+, e-
                                    const unsigned nmp, const unsigned nmm, // # mu+, mu-
                                    const unsigned nJets, const unsigned nBJets) const
{
  // Do we care at all?
  if(!((nep >= 2 && nem >= 2) || (nmp >= 2 && nmm >= 2) ||
       (nep && nem && nmp && nmm)))
    return -1;

  unsigned nLep = nep + nem + nmp + nmm;

  // VBF 2-jet
  if(cand.userFloat("D_VBF2j") > 0.38 && nLep == 4 && 
     ((nJets >= 2 && nJets < 4 && nBJets < 2) ||
      (nJets >= 4 && nBJets == 0)))
    return 2;

  // VH hadronic
  if((cand.userFloat("D_WHh") > 0.999 || cand.userFloat("D_ZHh") > 0.999) && 
     nLep == 4 && 
     ((nJets >= 2 && nJets < 4 && nBJets < 2) ||
      (nJets >= 4 && nBJets == 0)))
    return 4;
  
  // VH leptonic
  if((nJets == 0 && nLep >= 5) ||
     (nJets < 4 && nBJets == 0 &&
      (nLep == 5 || (nLep == 6 && nep == nem && nmp == nmm))))
    return 3;

  // ttH
  if((nJets >= 4 && nBJets >= 1) || nLep >= 5)
    return 5;

  // VBF 1-jet
  if(nLep == 4 && nJets == 1 && cand.userFloat("D_VBF1j") > 0.56)
    return 1;
  
  // untagged
  return 0;
}



#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ZZCategoryEmbedder);


