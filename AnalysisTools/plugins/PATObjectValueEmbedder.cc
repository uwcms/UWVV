///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectValueEmbedder                                                //
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


template<typename T, typename V>
class PATObjectValueEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectValueEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectValueEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  void embedValue(T& object, const int& value, const std::string& label) const;
  void embedValue(T& object, const bool& value, const std::string& label) const;
  void embedValue(T& object, const double& value, const std::string& label) const;
  void embedValue(T& object, const float& value, const std::string& label) const;

  const edm::EDGetTokenT<edm::View<T> > srcToken_;
  const edm::EDGetTokenT<V> valToken_;
  const std::string label_;
};


template<typename T, typename V>
PATObjectValueEmbedder<T,V>::PATObjectValueEmbedder(const edm::ParameterSet& iConfig) :
  srcToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  valToken_(consumes<V>(iConfig.getParameter<edm::InputTag>("valueSrc"))),
  label_(iConfig.getParameter<std::string>("label"))
{
  produces<std::vector<T> >();
}


template<typename T, typename V>
void PATObjectValueEmbedder<T,V>::produce(edm::Event& iEvent,
                                         const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);
  edm::Handle<V> value;

  iEvent.getByToken(srcToken_, in);
  iEvent.getByToken(valToken_, value);

  for(size_t i = 0; i < in->size(); ++i)
    {
      edm::Ptr<T> t = in->ptrAt(i);

      out->push_back(*t);
      embedValue(out->back(), *value, label_);
    }

  iEvent.put(std::move(out));
}

template<typename T, typename V>
void PATObjectValueEmbedder<T,V>::embedValue(T& object,
                                    const double& value, const std::string& label) const
{
    object.addUserFloat(label, value);
}

template<typename T, typename V>
void PATObjectValueEmbedder<T,V>::embedValue(T& object,
                                    const float& value, const std::string& label) const
{
    object.addUserFloat(label, value);
}

template<typename T, typename V>
void PATObjectValueEmbedder<T,V>::embedValue(T& object,
                                    const int& value, const std::string& label) const
{
    object.addUserInt(label, value);
}

template<typename T, typename V>
void PATObjectValueEmbedder<T,V>::embedValue(T& object,
                                    const bool& value, const std::string& label) const
{
    object.addUserInt(label, value);
}

typedef PATObjectValueEmbedder<pat::Electron, double> PATElectronDoubleEmbedder;
typedef PATObjectValueEmbedder<pat::Muon, double> PATMuonDoubleEmbedder;
typedef PATObjectValueEmbedder<pat::Tau, double> PATTauDoubleEmbedder;
typedef PATObjectValueEmbedder<pat::Jet, double> PATJetDoubleEmbedder;
typedef PATObjectValueEmbedder<pat::CompositeCandidate, double> PATCompositeCandidateDoubleEmbedder;
typedef PATObjectValueEmbedder<pat::Electron, int> PATElectronIntEmbedder;
typedef PATObjectValueEmbedder<pat::Muon, int> PATMuonIntEmbedder;
typedef PATObjectValueEmbedder<pat::Tau, int> PATTauIntEmbedder;
typedef PATObjectValueEmbedder<pat::Jet, int> PATJetIntEmbedder;
typedef PATObjectValueEmbedder<pat::CompositeCandidate, int> PATCompositeCandidateIntEmbedder;
typedef PATObjectValueEmbedder<pat::Electron, int> PATElectronIntEmbedder;
typedef PATObjectValueEmbedder<pat::Muon, int> PATMuonIntEmbedder;
typedef PATObjectValueEmbedder<pat::Tau, int> PATTauIntEmbedder;
typedef PATObjectValueEmbedder<pat::Jet, int> PATJetIntEmbedder;
typedef PATObjectValueEmbedder<pat::CompositeCandidate, int> PATCompositeCandidateIntEmbedder;

DEFINE_FWK_MODULE(PATElectronDoubleEmbedder);
DEFINE_FWK_MODULE(PATMuonDoubleEmbedder);
DEFINE_FWK_MODULE(PATTauDoubleEmbedder);
DEFINE_FWK_MODULE(PATJetDoubleEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateDoubleEmbedder);
DEFINE_FWK_MODULE(PATElectronIntEmbedder);
DEFINE_FWK_MODULE(PATMuonIntEmbedder);
DEFINE_FWK_MODULE(PATTauIntEmbedder);
DEFINE_FWK_MODULE(PATJetIntEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateIntEmbedder);
