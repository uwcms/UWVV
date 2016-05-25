///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATCompositeUserCandPromoter                                           //
//                                                                           //
//    Takes a collection of PAT CompositeCandidates, checks their daughters  //
//    to see if the daughters have userCands with a given label, and if so   //
//    adds them to the CompositeCandidates as userCands.                     //
//                                                                           //
//    Nate Woods, U. Wisconsin                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>
#include <iostream>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"


typedef reco::Candidate Cand;
typedef edm::Ptr<Cand> CandPtr;
typedef pat::CompositeCandidate CCand;
typedef edm::Ptr<CCand> CCandPtr;

namespace
{

  template<class T>
  std::vector<CandPtr> getCands(const edm::Ptr<T>& cand,
                                      const std::string& label)
  {
    std::vector<CandPtr> out;

    if(cand->hasUserCand(label))
      out.push_back(cand->userCand(label));

    return out;
  }

  // This is technically bad form. Whatever.
  template<>
  std::vector<CandPtr> getCands(const CCandPtr& cand,
                                      const std::string& label)
  {
    std::vector<CandPtr> out;

    if(cand->hasUserInt("n"+label+"Cands"))
      {
        size_t nCands = (size_t)cand->hasUserInt("n"+label+"Cands");
        for(size_t i = 0; i < nCands; ++i)
          {
            CandPtr c = cand->userCand(label+std::to_string(i));
            if(c.isNonnull())
              out.push_back(c);
          }
      }

    return out;
  }

}


template<class T1, class T2>
class PATCompositeUserCandPromoter : public edm::stream::EDProducer<>
{

public:
  explicit PATCompositeUserCandPromoter(const edm::ParameterSet& iConfig);
  virtual ~PATCompositeUserCandPromoter() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<CCand> > srcToken;
  const std::string label;
};


template<class T1, class T2>
PATCompositeUserCandPromoter<T1,T2>::PATCompositeUserCandPromoter(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<CCand> >(iConfig.getParameter<edm::InputTag>("src"))),
  label(iConfig.getParameter<std::string>("label"))
{
  produces<std::vector<CCand> >();
}


template<class T1, class T2>
void PATCompositeUserCandPromoter<T1,T2>::produce(edm::Event& iEvent,
                                                  const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<CCand> > in;
  std::unique_ptr<std::vector<CCand> > out(new std::vector<CCand>);

  iEvent.getByToken(srcToken, in);

  for(size_t i = 0; i < in->size(); ++i)
    {
      CCandPtr c = in->ptrAt(i);

      edm::Ptr<T1> dau1 = c->daughter(0)->masterClone().castTo<edm::Ptr<T1> >();
      std::vector<CandPtr> cands1 = ::getCands(dau1, label);
      edm::Ptr<T2> dau2 = c->daughter(1)->masterClone().castTo<edm::Ptr<T2> >();
      std::vector<CandPtr> cands2 = ::getCands(dau2, label);

      out->push_back(*c);

      size_t nCand = 0;

      for(size_t i = 0; i < cands1.size(); ++i)
        {
          out->back().addUserCand(label + std::to_string(nCand), cands1.at(i));
          nCand++;
        }
      for(size_t i = 0; i < cands2.size(); ++i)
        {
          out->back().addUserCand(label + std::to_string(nCand), cands2.at(i));
          nCand++;
        }

      out->back().addUserInt("n"+label+"Cands", nCand);
    }

  iEvent.put(std::move(out));
}


typedef PATCompositeUserCandPromoter<pat::Electron, pat::Electron> PATElectronCompositeUserCandPromoter;
typedef PATCompositeUserCandPromoter<pat::Muon, pat::Muon> PATMuonCompositeUserCandPromoter;
typedef PATCompositeUserCandPromoter<CCand, CCand> PATCompositeCompositeUserCandPromoter;

DEFINE_FWK_MODULE(PATElectronCompositeUserCandPromoter);
DEFINE_FWK_MODULE(PATMuonCompositeUserCandPromoter);
DEFINE_FWK_MODULE(PATCompositeCompositeUserCandPromoter);

