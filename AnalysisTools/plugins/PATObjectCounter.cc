///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectCountEmbedder                                                 //
//                                                                           //
//    Takes a collection of PAT objects and embeds number of objects         //
//    passing the (otional) specified cut                                    //
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
class PATObjectCountEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectCountEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectCountEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<T> > srcToken_;
  StringCutObjectSelector<T> cut_;
  const std::string label_;
};


template<typename T>
PATObjectCountEmbedder<T>::PATObjectCountEmbedder(const edm::ParameterSet& iConfig) :
  srcToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  cut_(iConfig.getUntrackedParameter<std::string>("cut", "")),
  label_(iConfig.getParameter<std::string>("label"))
{
  produces<unsigned int>();
}


template<typename T>
void PATObjectCountEmbedder<T>::produce(edm::Event& iEvent,
                                         const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  iEvent.getByToken(srcToken_, in);
  
  std::unique_ptr<unsigned int> num(new unsigned int(0));
  for(size_t i = 0; i < in->size(); ++i)
    {
      if (cut_(in->at(i)))
        *num += 1;
    }
  iEvent.put(std::move(num), label_);
}

typedef PATObjectCountEmbedder<pat::Electron> PATElectronCountEmbedder;
typedef PATObjectCountEmbedder<pat::Muon> PATMuonCountEmbedder;
typedef PATObjectCountEmbedder<pat::Tau> PATTauCountEmbedder;
typedef PATObjectCountEmbedder<pat::Jet> PATJetCountEmbedder;
typedef PATObjectCountEmbedder<pat::CompositeCandidate> PATCompositeCandidateCountEmbedder;

DEFINE_FWK_MODULE(PATElectronCountEmbedder);
DEFINE_FWK_MODULE(PATMuonCountEmbedder);
DEFINE_FWK_MODULE(PATTauCountEmbedder);
DEFINE_FWK_MODULE(PATJetCountEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateCountEmbedder);
