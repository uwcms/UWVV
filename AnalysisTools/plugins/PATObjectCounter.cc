///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectCounter                                                       //
//                                                                           //
//    Takes a collection of PAT objects and adss and int of the     //
//    number of objects passing the (otional) specified cut to the event     //
//                                                                           //
//    Kenneth Long, U. Wisconsin                                             //
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
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"


template<typename T>
class PATObjectCounter : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectCounter(const edm::ParameterSet& iConfig);
  virtual ~PATObjectCounter() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<T> > srcToken_;
  StringCutObjectSelector<T> cut_;
  const std::string label_;
};


template<typename T>
PATObjectCounter<T>::PATObjectCounter(const edm::ParameterSet& iConfig) :
  srcToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  cut_(iConfig.getUntrackedParameter<std::string>("cut", "")),
  label_(iConfig.getParameter<std::string>("label"))
{
  produces<int>(label_);
}


template<typename T>
void PATObjectCounter<T>::produce(edm::Event& iEvent,
                                         const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  iEvent.getByToken(srcToken_, in);
  
  std::unique_ptr<int> num(new int(0));
  for(size_t i = 0; i < in->size(); ++i)
    {
      if (cut_(in->at(i)))
        *num += 1;
    }
  iEvent.put(std::move(num), label_);
}

typedef PATObjectCounter<pat::Electron> PATElectronCounter;
typedef PATObjectCounter<pat::Muon> PATMuonCounter;
typedef PATObjectCounter<pat::Tau> PATTauCounter;
typedef PATObjectCounter<pat::Jet> PATJetCounter;
typedef PATObjectCounter<pat::CompositeCandidate> PATCompositeCandidateCounter;

DEFINE_FWK_MODULE(PATElectronCounter);
DEFINE_FWK_MODULE(PATMuonCounter);
DEFINE_FWK_MODULE(PATTauCounter);
DEFINE_FWK_MODULE(PATJetCounter);
DEFINE_FWK_MODULE(PATCompositeCandidateCounter);
