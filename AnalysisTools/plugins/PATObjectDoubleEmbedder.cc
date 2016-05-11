///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectDoubleEmbedder                                                //
//                                                                           //
//    Takes a double and a collection of PAT objects and embeds the          //
//    double as a userfloat in the objects                                   //
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
#include "DataFormats/Common/interface/View.h"


template<typename T>
class PATObjectDoubleEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectDoubleEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectDoubleEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<T> > srcToken;
  const edm::EDGetTokenT<double> valToken;
  const std::string label;
};


template<typename T>
PATObjectDoubleEmbedder<T>::PATObjectDoubleEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  valToken(consumes<double>(iConfig.getParameter<edm::InputTag>("valueSrc"))),
  label(iConfig.getParameter<std::string>("label"))
{
  produces<std::vector<T> >();
}


template<typename T>
void PATObjectDoubleEmbedder<T>::produce(edm::Event& iEvent,
                                         const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);
  edm::Handle<double> value;

  iEvent.getByToken(srcToken, in);
  iEvent.getByToken(valToken, value);

  for(size_t i = 0; i < in->size(); ++i)
    {
      edm::Ptr<T> t = in->ptrAt(i);

      out->push_back(*t);
      out->back().addUserFloat(label, *value);
    }

  iEvent.put(std::move(out));
}


typedef PATObjectDoubleEmbedder<pat::Electron> PATElectronDoubleEmbedder;
typedef PATObjectDoubleEmbedder<pat::Muon> PATMuonDoubleEmbedder;
typedef PATObjectDoubleEmbedder<pat::Tau> PATTauDoubleEmbedder;
typedef PATObjectDoubleEmbedder<pat::Jet> PATJetDoubleEmbedder;
typedef PATObjectDoubleEmbedder<pat::CompositeCandidate> PATCompositeCandidateDoubleEmbedder;

DEFINE_FWK_MODULE(PATElectronDoubleEmbedder);
DEFINE_FWK_MODULE(PATMuonDoubleEmbedder);
DEFINE_FWK_MODULE(PATTauDoubleEmbedder);
DEFINE_FWK_MODULE(PATJetDoubleEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateDoubleEmbedder);
