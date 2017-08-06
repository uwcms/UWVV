///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>
#include <string>
#include <iostream>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"

#include "UWVV/Utilities/interface/helpers.h"

typedef pat::CompositeCandidate CCand;



class CleanedJetCollectionEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit CleanedJetCollectionEmbedder(const edm::ParameterSet& iConfig);
  virtual ~CleanedJetCollectionEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<CCand> > srcToken;
  const edm::EDGetTokenT<edm::View<pat::Jet> > jetSrcToken;

  typedef const edm::Ptr<reco::Candidate> (FType) (const reco::Candidate* const);
};


CleanedJetCollectionEmbedder::CleanedJetCollectionEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<CCand> >(iConfig.getParameter<edm::InputTag>("src"))),
  jetSrcToken(consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jetSrc")))
{
  produces<std::vector<CCand> >();
}


void CleanedJetCollectionEmbedder::produce(edm::Event& iEvent,
                                            const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<CCand> > in;
  edm::Handle<edm::View<pat::Jet> > uncleanedJets;
  std::unique_ptr<std::vector<CCand> > out(new std::vector<CCand>);

  iEvent.getByToken(srcToken, in);
  iEvent.getByToken(jetSrcToken, uncleanedJets);

  edm::PtrVector<pat::Jet> cleanedJets; 

  for(size_t i = 0; i < in->size(); ++i)
    {
      edm::Ptr<CCand> cand = in->ptrAt(i);

      out->push_back(*cand);
      
      for(size_t i = 0; i < uncleanedJets->size(); ++i)
        {
          if(!uwvv::helpers::overlapWithAnyDaughter(uncleanedJets->at(i), *cand, 0.4)) 
            {
              cleanedJets.push_back(uncleanedJets->ptrAt(i));
            }
        }
      out->back().addUserData<edm::PtrVector<pat::Jet>>("cleanedJets", cleanedJets);
    }
      
  iEvent.put(std::move(out));
}

DEFINE_FWK_MODULE(CleanedJetCollectionEmbedder);

