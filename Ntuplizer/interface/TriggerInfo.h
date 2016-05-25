#ifndef UWVV_Ntuplizer_EventInfo_h
#define UWVV_Ntuplizer_EventInfo_h

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"


namespace uwvv
{

  class TriggerInfoHolder
  {
   public:
    TriggerInfoHolder(const std::string& nameExp) : 
    nameExp_(nameExp),
    name_(nameExp),
    bit_(999),
    isValid_(false)
    {;}
    ~TriggerInfoHolder() {;}
  
    // Set bit, name
    void setup(const edm::TriggerNames& names)
    {
      std::regex re(nameExp_);
      bit_ = names.size();
      for(size_t i = 0; i < names.size(); ++i)
        {
          if(std::regex_match(names.triggerName(i), re))
            {
              // There can be only one
              if(bit_ != names.size())
                throw cms::Exception("BadTriggerExpression")
                  << "Two path matched trigger path expression \""
                  << nameExp_ << "\" (\"" << names.triggerName(bit_)
                  << "\" and \"" << names.triggerName(i) << "\")" 
                  << std::endl;
            }
          bit_ = i;
          name_ = names.triggerName(i);
        }
      isValid_ = true;
    }

    bool pass(const edm::triggerResults& results)
    {
      if(!isValid_)
        explode();

      return results.accept(bit_);
    }

    unsigned prescale(const pat::PackedTriggerPrescales& prescales)
    {
      if(!isValid_)
        explode();

      return prescales.getPrescaleForIndex(bit_);
    }
  
   private:
    // for attempted uninitialized use
    void explode() const
    {
      throw cms::Exception("UninitializedTriggerInfo")
        << "ERROR: attempt to use trigger info before it's set up." 
        << std::endl;
    }

    // reg exp for trigger name
    const std::string nameExp_;
    // name and bit of the actual trigger (may change)
    std::string name_;
    size_t bit_;

    // have we set up the name and bit?
    bool isValid_;
  };
   
  
  class EventInfo
  {
   public:
    EventInfo(edm::ConsumesCollector cc, const edm::ParameterSet& config);
    ~EventInfo() {;}
  
    void setEvent(const edm::Event& event);
  
    const edm::EventID id() const {return currentEvent_->id();}
  
    const edm::Ptr<reco::Vertex> pv() 
    {
      return (nVertices() ? vertices()->ptrAt(0) : edm::Ptr<reco::Vertex>(NULL, 0));
    }
    size_t nVertices() {return vertices()->size();}
    const edm::Handle<edm::View<reco::Vertex> >& vertices() {return vertices_.get();}
    const edm::Handle<edm::View<pat::Electron> >& electrons() {return electrons_.get();}
    const edm::Handle<edm::View<pat::Muon> >& muons() {return muons_.get();}
    const edm::Handle<edm::View<pat::Tau> >& taus() {return taus_.get();}
    const edm::Handle<edm::View<pat::Photon> >& photons() {return photons_.get();}
    const edm::Handle<edm::View<pat::Jet> >& jets() {return jets_.get();}
    const edm::Handle<edm::View<pat::PackedCandidate> >& pfCands() {return pfCands_.get();}
    const pat::MET& met() {return mets_.get()->front();}
    const edm::Handle<pat::METCollection>& mets() {return mets_.get();}
    const edm::Handle<std::vector<PileupSummaryInfo> >& puInfo() {return puInfo_.get();}
    const edm::Handle<GenEventInfoProduct>& genEventInfo() {return genEventInfo_.get();}
  
   private:
    const edm::Event* currentEvent_;
    EventInfoHolder<edm::View<reco::Vertex> > vertices_;
    EventInfoHolder<edm::View<pat::Electron> > electrons_;
    EventInfoHolder<edm::View<pat::Muon> > muons_;
    EventInfoHolder<edm::View<pat::Tau> > taus_;
    EventInfoHolder<edm::View<pat::Photon> > photons_;
    EventInfoHolder<edm::View<pat::Jet> > jets_;
    EventInfoHolder<edm::View<pat::PackedCandidate> > pfCands_;
    EventInfoHolder<pat::METCollection> mets_;
    EventInfoHolder<std::vector<PileupSummaryInfo> > puInfo_;
    EventInfoHolder<GenEventInfoProduct> genEventInfo_;
  };

} // namespace

#endif // header guard
