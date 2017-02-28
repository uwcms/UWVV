// PATMuonIDEmbedder.cc
// Embeds muons ids as userInts for later
// via Devin Taylor, U. Wisconsin
// with modifications by K. Long, U. Wisconsin

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

class MuonIdEmbedder : public edm::stream::EDProducer<>
{
public:
  explicit MuonIdEmbedder(const edm::ParameterSet&);
  ~MuonIdEmbedder() {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  // Methods
  void beginJob() {}
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  void endJob() {}

  bool isMediumMuonICHEP(const reco::Muon& recoMu);
  bool isWZMediumMuon(const pat::Muon& patMu, const reco::Vertex& pv);
  bool isWZMediumMuonNoIso(const pat::Muon& patMu, const reco::Vertex& pv);
  bool isWZLooseMuon(const pat::Muon& patMu);
  bool isWZLooseMuonNoIso(const pat::Muon& patMu);
  bool isSoftMuonICHEP(const reco::Muon& recoMu, const reco::Vertex& pv);

  // Data
  edm::EDGetTokenT<edm::View<pat::Muon> > collectionToken_; // input collection
  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;  // vertices
  std::auto_ptr<std::vector<pat::Muon> > out;             // Collection we'll output at the end
};

// Constructors and destructors
MuonIdEmbedder::MuonIdEmbedder(const edm::ParameterSet& iConfig):
  collectionToken_(consumes<edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("src"))),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc")))
{
  produces<std::vector<pat::Muon> >();
}

void MuonIdEmbedder::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  out = std::auto_ptr<std::vector<pat::Muon> >(new std::vector<pat::Muon>);

  edm::Handle<edm::View<pat::Muon> > collection;
  iEvent.getByToken(collectionToken_, collection);

  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vertexToken_, vertices);

  const reco::Vertex& pv = *vertices->begin();

  for (size_t c = 0; c < collection->size(); ++c) {
    const auto obj = collection->at(c);
    pat::Muon newObj = obj;

    newObj.addUserInt("IsTightMuon", obj.isTightMuon(pv));
    newObj.addUserInt("IsMediumMuonICHEP", isMediumMuonICHEP(obj));
    newObj.addUserInt("IsWZMediumMuon", isWZMediumMuon(obj, pv));
    newObj.addUserInt("IsWZMediumMuonNoIso", isWZMediumMuonNoIso(obj, pv));
    newObj.addUserInt("IsWZLooseMuon", isWZLooseMuon(obj));
    newObj.addUserInt("IsWZLooseMuonNoIso", isWZLooseMuonNoIso(obj));
    newObj.addUserInt("IsSoftMuon", obj.isSoftMuon(pv));
    newObj.addUserInt("IsSoftMuonICHEP", isSoftMuonICHEP(obj,pv));
    newObj.addUserInt("IsHighPtMuon", obj.isHighPtMuon(pv));
    newObj.addUserFloat("segmentCompatibility", muon::segmentCompatibility(obj));
    newObj.addUserInt("IsGoodMuon", muon::isGoodMuon(obj, muon::TMOneStationTight));
    int highPurity = 0;
    if (obj.innerTrack().isNonnull()) {
        highPurity = obj.innerTrack()->quality(reco::TrackBase::highPurity);
    }
    newObj.addUserInt("highPurityTrack",highPurity);
    
    out->push_back(newObj);
  }

  iEvent.put(out);
}

// ICHEP short term IDs
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Short_Term_Instructions_for_ICHE
bool MuonIdEmbedder::isMediumMuonICHEP(const reco::Muon & recoMu) 
  {
    bool goodGlob = recoMu.isGlobalMuon() && 
                    recoMu.globalTrack()->normalizedChi2() < 3 && 
                    recoMu.combinedQuality().chi2LocalPosition < 12 && 
                    recoMu.combinedQuality().trkKink < 20; 
    bool isMedium = muon::isLooseMuon(recoMu) && 
                    recoMu.innerTrack()->validFraction() > 0.49 && 
                    muon::segmentCompatibility(recoMu) > (goodGlob ? 0.303 : 0.451); 
    return isMedium; 
  }
bool MuonIdEmbedder::isWZLooseMuon(const pat::Muon& patMu) 
  {
    reco::MuonPFIsolation pfIsoDB04 = patMu.pfIsolationR04();
    float muIso = (pfIsoDB04.sumChargedHadronPt
                        + std::max(0., pfIsoDB04.sumNeutralHadronEt
                            + pfIsoDB04.sumPhotonEt
                            - 0.5*pfIsoDB04.sumPUPt)
                  ) / patMu.pt();
    return isWZLooseMuonNoIso(patMu) && muIso < 0.25;
  }
bool MuonIdEmbedder::isWZLooseMuonNoIso(const pat::Muon& patMu) 
  {
    return isMediumMuonICHEP(patMu) && 
        patMu.trackIso()/patMu.pt() < 0.4;
  }
bool MuonIdEmbedder::isWZMediumMuonNoIso(const pat::Muon& patMu, const reco::Vertex& pv) 
  {
    double dxy = std::abs(patMu.innerTrack()->dxy(pv.position()));
    return isWZLooseMuonNoIso(patMu) && 
        ( patMu.pt() > 20. ? dxy < 0.02 : dxy < 0.01 ) &&
        std::abs(patMu.innerTrack()->dz(pv.position())) < 0.1;
  }
bool MuonIdEmbedder::isWZMediumMuon(const pat::Muon& patMu, const reco::Vertex& pv) 
  {
    reco::MuonPFIsolation pfIsoDB04 = patMu.pfIsolationR04();
    float muIso = (pfIsoDB04.sumChargedHadronPt
                        + std::max(0., pfIsoDB04.sumNeutralHadronEt
                            + pfIsoDB04.sumPhotonEt
                            - 0.5*pfIsoDB04.sumPUPt)
                  ) / patMu.pt();
    return isWZMediumMuonNoIso(patMu, pv) && muIso < 0.15;
  }
bool MuonIdEmbedder::isSoftMuonICHEP(const reco::Muon & recoMu, const reco::Vertex& pv) 
  {
    bool soft = muon::isGoodMuon(recoMu, muon::TMOneStationTight) &&
                recoMu.innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5 &&
                recoMu.innerTrack()->hitPattern().pixelLayersWithMeasurement() > 0 &&
                fabs(recoMu.innerTrack()->dxy(pv.position())) < 0.3 &&
                fabs(recoMu.innerTrack()->dz(pv.position())) < 20.;
    return soft;
  }
void MuonIdEmbedder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(MuonIdEmbedder);
