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
  const std::vector<std::string> boolLabels_;
  const std::vector<std::string> intLabels_;
  const std::vector<edm::EDGetTokenT<edm::ValueMap<float>>> floatValTokens_;
  const std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> boolValTokens_;
  const std::vector<edm::EDGetTokenT<edm::ValueMap<int>>> intValTokens_;
};


template<typename T>
PATObjectValueMapEmbedder<T>::PATObjectValueMapEmbedder(const edm::ParameterSet& iConfig) :
  srcToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  floatLabels_(iConfig.getUntrackedParameter<std::vector<std::string>>("floatLabels", std::vector<std::string>())),
  boolLabels_(iConfig.getUntrackedParameter<std::vector<std::string>>("boolLabels", std::vector<std::string>())),
  intLabels_(iConfig.getUntrackedParameter<std::vector<std::string>>("intLabels", std::vector<std::string>())),
  floatValTokens_(edm::vector_transform(iConfig.getUntrackedParameter<std::vector<edm::InputTag> >(
    "floatVals", std::vector<edm::InputTag>()), 
      [this](edm::InputTag const& tag){return consumes<edm::ValueMap<float> >(tag);})),
  boolValTokens_(edm::vector_transform(iConfig.getUntrackedParameter<std::vector<edm::InputTag> >(
    "boolVals", std::vector<edm::InputTag>()), 
      [this](edm::InputTag const& tag){return consumes<edm::ValueMap<bool> >(tag);})),
  intValTokens_(edm::vector_transform(iConfig.getUntrackedParameter<std::vector<edm::InputTag> >(
    "intVals", std::vector<edm::InputTag>()), 
      [this](edm::InputTag const& tag){return consumes<edm::ValueMap<int> >(tag);}))
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

  //TODO: template, cleanup 
  if (floatValTokens_.size() != floatLabels_.size())
    {
      throw cms::Exception("InvalidParams")
        << "Must have equal number of values and labels";
    }
  if (boolValTokens_.size() != boolLabels_.size())
    {
      throw cms::Exception("InvalidParams")
        << "Must have equal number of values and labels";
    }
  if (intValTokens_.size() != intLabels_.size())
    {
      throw cms::Exception("InvalidParams")
        << "Must have equal number of values and labels";
    }
  std::vector<size_t> lengths = {floatLabels_.size(), boolLabels_.size(), intLabels_.size()};

  for (size_t i = 0; i < in->size(); i++) 
    {
      out->push_back(in->at(i));
    }
  for (size_t i = 0; i < (*std::max_element(lengths.begin(), lengths.end())); i++)
    {
      edm::Handle<edm::ValueMap<float> > floatVals;
      edm::Handle<edm::ValueMap<bool> > boolVals;
      edm::Handle<edm::ValueMap<int> > intVals;
      if(i < floatLabels_.size())
        {
          iEvent.getByToken(floatValTokens_[i], floatVals);
        }
      if(i < boolLabels_.size())
        {
          iEvent.getByToken(boolValTokens_[i], boolVals);
        }
      if(i < intLabels_.size())
        {
          iEvent.getByToken(intValTokens_[i], intVals);
        }
      
      for(size_t j = 0; j < in->size(); ++j)
        {
          T& obj = out->at(j);
          edm::Ptr<T> t = in->ptrAt(j);
          if(i < floatLabels_.size())
            {
              embedValue(obj, (*floatVals)[t], floatLabels_[i]);
            }
          if(i < boolLabels_.size())
            {
              embedValue(obj, (*boolVals)[t], boolLabels_[i]);
            }
          if(i < intLabels_.size())
            {
              embedValue(obj, (*intVals)[t], intLabels_[i]);
            }
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
