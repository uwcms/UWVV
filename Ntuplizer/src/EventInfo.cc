#include "UWVV/Ntuplizer/interface/EventInfo.h"


using namespace uwvv;

EventInfo::EventInfo(edm::ConsumesCollector cc,
                     const edm::ParameterSet& config) :
  vertices_(cc, config.getParameter<edm::InputTag>("vtxSrc")),
  electrons_(cc, config.getParameter<edm::InputTag>("eSrc")),
  muons_(cc, config.getParameter<edm::InputTag>("mSrc")),
  taus_(cc, config.getParameter<edm::InputTag>("tSrc")),
  photons_(cc, config.getParameter<edm::InputTag>("gSrc")),
  jets_(cc, config.getParameter<edm::InputTag>("jSrc")),
  pfCands_(cc, config.getParameter<edm::InputTag>("pfCandSrc")),
  mets_(cc, config.getParameter<edm::InputTag>("metSrc")),
  puInfo_(cc, config.getParameter<edm::InputTag>("puSrc")),
  genEventInfo_(cc, config.getParameter<edm::InputTag>("genEventInfoSrc"))
{
}


void EventInfo::setEvent(const edm::Event& event)
{
  vertices_.setEvent(event);
  electrons_.setEvent(event);
  muons_.setEvent(event);
  taus_.setEvent(event);
  photons_.setEvent(event);
  jets_.setEvent(event);
  pfCands_.setEvent(event);
  mets_.setEvent(event);
  puInfo_.setEvent(event);
  genEventInfo_.setEvent(event);

  currentEvent_ = &event;
}
