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
#include "FWCore/Utilities/interface/transform.h"


template<typename T>
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

  const edm::EDGetTokenT<edm::View<T>> srcToken_;
  const std::vector<std::string> floatLabels_;
  const std::vector<edm::EDGetTokenT<edm::ValueMap<float>>> floatValTokens_;
};


template<typename T>
PATObjectValueMapEmbedder<T>::PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig) :
  srcToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  floatLabels_(iConfig.getUntrackedParameter<std::vector<std::string>>("floatLabels", std::vector<std::string>())),
  floatValTokens_(edm::vector_transform(iConfig.getUntrackedParameter<std::vector<edm::InputTag> >(
    "floatVals", std::vector<edm::InputTag>()), 
      [this](edm::InputTag const& tag){return consumes<edm::ValueMap<float> >(tag);})
  )
{
  produces<std::vector<T> >();
}


template<typename T>
void PATObjectValueMapEmbedder<T>::produce(edm::Event& iEvent,
                                           const edm::EventSetup& iSetup)
{
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);
  edm::Handle<edm::View<T> > in;
  iEvent.getByToken(srcToken_, in);
  
  if (floatValTokens_.size() != floatLabels_.size())
    {
      throw cms::Exception("InvalidParams")
        << "Must have equal number of values and labels";
    }
  
  for (size_t i = 0; i < floatValTokens_.size(); i++)
    {
      edm::Handle<edm::ValueMap<float> > floatVals;
      iEvent.getByToken(floatValTokens_[i], floatVals);

      for(size_t j = 0; j < in->size(); ++j)
        {
          edm::Ptr<T> t = in->ptrAt(j);

          out->push_back(*t);
          embedValue(out->back(), (*floatVals)[t], floatLabels_[i]);
        }
    }
  iEvent.put(std::move(out));
}

template<typename T>
void PATObjectValueMapEmbedder<T>::embedValue(T& object,
                                const float& value, const std::string& label) const
{
  object.addUserFloat(label, value);
}

template<typename T>
void PATObjectValueMapEmbedder<T>::embedValue(T& object,
                                const int& value, const std::string& label) const
{
  object.addUserInt(label, value);
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
