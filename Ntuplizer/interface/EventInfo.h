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
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"



namespace uwvv
{

  template<class T> class EventInfoHolder
  {
   public:
    EventInfoHolder(edm::ConsumesCollector& cc, const edm::InputTag& tag) :
      token_(cc.consumes<T>(tag)),
      isValid_(false)
        {;}
    ~EventInfoHolder() {;}

    void setEvent(const edm::Event& event)
    {
      currentEvent_ = &event;
      isValid_ = false;
    }

    const edm::Handle<T>& get()
    {
      if(isValid_)
        return handle_;

      currentEvent_->getByToken(token_, handle_);
      isValid_ = true;
      return handle_;
    }

   private:
    const edm::EDGetTokenT<T> token_;
    edm::Handle<T> handle_;
    const edm::Event* currentEvent_;
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
    const edm::Handle<edm::View<reco::GenJet> >& genJets() {return genJets_.get();}
    const edm::Handle<edm::View<reco::GenParticle> >& genParticles() {return genParticles_.get();}


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
    EventInfoHolder<edm::View<reco::GenJet> > genJets_;
    EventInfoHolder<edm::View<reco::GenParticle> > genParticles_;
  };

} // namespace

#endif // header guard
