///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectValueMapEmbedder                                              //
//                                                                           //
//    Takes a value map and a collection of PAT objects and embeds the       //
//    values from the map as userfloats in the objects                       //
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
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"


template<typename T>
class PATObjectValueMapEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectValueMapEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<T> > srcToken;
  const edm::EDGetTokenT<edm::ValueMap<float> > valToken;
  const std::string label;
};


template<typename T>
PATObjectValueMapEmbedder<T>::PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  valToken(consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("valueSrc"))),
  label(iConfig.getParameter<std::string>("label"))
{
  produces<std::vector<T> >();
}


template<typename T>
void PATObjectValueMapEmbedder<T>::produce(edm::Event& iEvent,
                                           const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);
  edm::Handle<edm::ValueMap<float> > values;

  iEvent.getByToken(srcToken, in);
  iEvent.getByToken(valToken, values);

  for(size_t i = 0; i < in->size(); ++i)
    {
      edm::Ptr<T> t = in->ptrAt(i);

      out->push_back(*t);
      out->back().addUserFloat(label, (*values)[t]);
    }

  iEvent.put(std::move(out));
}


typedef PATObjectValueMapEmbedder<pat::Electron> PATElectronValueMapEmbedder;
typedef PATObjectValueMapEmbedder<pat::Muon> PATMuonValueMapEmbedder;
typedef PATObjectValueMapEmbedder<pat::Tau> PATTauValueMapEmbedder;
typedef PATObjectValueMapEmbedder<pat::Jet> PATJetValueMapEmbedder;
typedef PATObjectValueMapEmbedder<pat::CompositeCandidate> PATCompositeCandidateValueMapEmbedder;

DEFINE_FWK_MODULE(PATElectronValueMapEmbedder);
DEFINE_FWK_MODULE(PATMuonValueMapEmbedder);
DEFINE_FWK_MODULE(PATTauValueMapEmbedder);
DEFINE_FWK_MODULE(PATJetValueMapEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateValueMapEmbedder);
