///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectValueMapEmbedder                                              //
//                                                                           //
//    Takes a value map and a collection of PAT objects and embeds the       //
//    values from the map as userfloats/ints/bool in the objects             //
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


template<typename T, typename V>
class PATObjectValueMapEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectValueMapEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  //void embedvalue(const T& object, const V& value, const std::string& label) const;
  void embedValue(T& object, const int& value, const std::string& label) const;
  void embedValue(T& object, const float& value, const std::string& label) const;

  const edm::EDGetTokenT<edm::View<T> > srcToken;
  const edm::EDGetTokenT<edm::ValueMap<V> > valToken;
  const std::string label;
};


template<typename T, typename V>
PATObjectValueMapEmbedder<T,V>::PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  valToken(consumes<edm::ValueMap<V> >(iConfig.getParameter<edm::InputTag>("valueSrc"))),
  label(iConfig.getParameter<std::string>("label"))
{
  produces<std::vector<T> >();
}


template<typename T, typename V>
void PATObjectValueMapEmbedder<T,V>::produce(edm::Event& iEvent,
                                           const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);
  edm::Handle<edm::ValueMap<V> > values;

  iEvent.getByToken(srcToken, in);
  iEvent.getByToken(valToken, values);

  for(size_t i = 0; i < in->size(); ++i)
    {
      edm::Ptr<T> t = in->ptrAt(i);

      out->push_back(*t);
      //out->back().addUserFloat(label, (*values)[t]);
      embedValue(out->back(), (*values)[t], label);
    }

  iEvent.put(std::move(out));
}

template<typename T, typename V>
void PATObjectValueMapEmbedder<T,V>::embedValue(T& object,
                                const float& value, const std::string& label) const
{
  object.addUserFloat(label, value);
}

template<typename T, typename V>
void PATObjectValueMapEmbedder<T,V>::embedValue(T& object,
                                const int& value, const std::string& label) const
{
  object.addUserInt(label, value);
}


typedef PATObjectValueMapEmbedder<pat::Electron, float> PATElectronValueMapFloatEmbedder;
typedef PATObjectValueMapEmbedder<pat::Muon, float> PATMuonValueMapFloatEmbedder;
typedef PATObjectValueMapEmbedder<pat::Tau, float> PATTauValueMapFloatEmbedder;
typedef PATObjectValueMapEmbedder<pat::Jet, float> PATJetValueMapFloatEmbedder;
typedef PATObjectValueMapEmbedder<pat::CompositeCandidate, float> PATCompositeCandidateValueMapFloatEmbedder;
typedef PATObjectValueMapEmbedder<pat::Electron, int> PATElectronValueMapIntEmbedder;
typedef PATObjectValueMapEmbedder<pat::Muon, int> PATMuonValueMapIntEmbedder;
typedef PATObjectValueMapEmbedder<pat::Tau, int> PATTauValueMapIntEmbedder;
typedef PATObjectValueMapEmbedder<pat::Jet, int> PATJetValueMapIntEmbedder;
typedef PATObjectValueMapEmbedder<pat::CompositeCandidate, int> PATCompositeCandidateValueMapIntEmbedder;

DEFINE_FWK_MODULE(PATElectronValueMapFloatEmbedder);
DEFINE_FWK_MODULE(PATMuonValueMapFloatEmbedder);
DEFINE_FWK_MODULE(PATTauValueMapFloatEmbedder);
DEFINE_FWK_MODULE(PATJetValueMapFloatEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateValueMapFloatEmbedder);
DEFINE_FWK_MODULE(PATElectronValueMapIntEmbedder);
DEFINE_FWK_MODULE(PATMuonValueMapIntEmbedder);
DEFINE_FWK_MODULE(PATTauValueMapIntEmbedder);
DEFINE_FWK_MODULE(PATJetValueMapIntEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateValueMapIntEmbedder);
