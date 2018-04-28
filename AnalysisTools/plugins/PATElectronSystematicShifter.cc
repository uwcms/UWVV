//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//   PATElectronSystematicShifter.cc                                        //
//                                                                          //
//   Takes an electron collection from MC and shifts the electrons' energy  //
//   by the scale and/or resolution uncertainties for systematics           //
//   calculations.                                                          //
//                                                                          //
//   Nate Woods, U. Wisconsin                                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "EgammaAnalysis/ElectronTools/interface/EnergyScaleCorrection_class.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

// #include "EgammaAnalysis/ElectronTools/interface/SimpleElectron.h"

class PATElectronSystematicShifter : public edm::stream::EDProducer<>
{
public:
  explicit PATElectronSystematicShifter(const edm::ParameterSet&);
  virtual ~PATElectronSystematicShifter() {}

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<pat::Electron> > electronCollectionToken;
  const edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEBToken_;
  const edm::EDGetTokenT<EcalRecHitCollection> recHitCollectionEEToken_;
  const std::string filename; // files to base the corrections on (same as with CalibratedPatElectronProducerRun2)

  // Size of shift, in units of sigma
  const float scaleShift;
  const float rhoResShift;
  const float phiResShift;
  const bool shiftCollection;

  const EnergyScaleCorrection_class correcter;
};



PATElectronSystematicShifter::PATElectronSystematicShifter(const edm::ParameterSet& iConfig):
  electronCollectionToken(consumes<edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("src"))),
  recHitCollectionEBToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollectionEB"))),
  recHitCollectionEEToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollectionEE"))),
  filename(iConfig.getParameter<std::string>("correctionFile")),
  scaleShift(iConfig.exists("scaleShift") ? iConfig.getParameter<double>("scaleShift") : 0.),
  rhoResShift(iConfig.exists("rhoResShift") ? iConfig.getParameter<double>("rhoResShift") : 0.),
  phiResShift(iConfig.exists("phiResShift") ? iConfig.getParameter<double>("phiResShift") : 0.),
  shiftCollection(iConfig.exists("shiftCollection") ? iConfig.getParameter<bool>("shiftCollection") : 0.),
  correcter(filename)
{
  edm::Service<edm::RandomNumberGenerator> rng;
  if(!rng.isAvailable())
    throw cms::Exception("Configuration")
      << "PATElectronSystematicShifter requires the "
      << "RandomNumberGeneratorService, which is not present in this "
      << "configuration!" << std::endl
      << "Please add it!" << std::endl;

  produces<std::vector<pat::Electron> >();
}


void PATElectronSystematicShifter::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::unique_ptr<std::vector<pat::Electron> > out = std::make_unique<std::vector<pat::Electron> >();

  edm::Handle<edm::View<pat::Electron> > in;
  iEvent.getByToken(electronCollectionToken, in);

  edm::Handle<EcalRecHitCollection> recHitCollectionEBHandle;
  edm::Handle<EcalRecHitCollection> recHitCollectionEEHandle;

  iEvent.getByToken(recHitCollectionEBToken_, recHitCollectionEBHandle);
  iEvent.getByToken(recHitCollectionEEToken_, recHitCollectionEEHandle);

  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine& engine = rng->getEngine(iEvent.streamID());

  for(size_t iEle = 0; iEle < in->size(); ++iEle)
    {
      out->push_back(in->at(iEle));
      pat::Electron& ele = out->back();

      bool isEB = ele.isEB();
      float r9 = ele.r9();
      float absEta = std::abs(ele.eta());
      float et = ele.et();

      DetId seedDetId = ele.superCluster()->seed()->seed();
      const EcalRecHitCollection* recHits = (ele.isEB()) ? recHitCollectionEBHandle.product() : recHitCollectionEEHandle.product();
      EcalRecHitCollection::const_iterator seedRecHit = recHits->find(seedDetId);
      unsigned int gainSeedSC = 12;
      if (seedRecHit != recHits->end()) { 
          if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain6)) gainSeedSC = 6;
          if(seedRecHit->checkFlag(EcalRecHit::kHasSwitchToGain1)) gainSeedSC = 1;
      }
      ele.addUserFloat("gainSeed", gainSeedSC);

      float scale = 1.;

      float scaleError = correcter.ScaleCorrectionUncertainty(iEvent.id().run(),
                                        isEB, r9, absEta, et, gainSeedSC);
      ele.addUserFloat("scaleCorrError", scaleError);
      if(scaleShift)
          // flip sign of shift because we're "undoing" the correction applied
          // to data
          scale -= scaleShift * scaleError;

      float smearSigmaUp = correcter.getSmearingSigma(iEvent.id().run(), isEB, r9, absEta,
                            et, gainSeedSC, 1, 1);
      float smearSigmaDown = correcter.getSmearingSigma(iEvent.id().run(), isEB, r9, absEta,
                            et, gainSeedSC, -1, -1);
      ele.addUserFloat("resSmearSigmaUp", smearSigmaUp);
      ele.addUserFloat("resSmearSigmaDown", smearSigmaDown);
      ele.addUserFloat("PtScale_scaleUpResUp", CLHEP::RandGauss::shoot(&engine, 1-scaleError, smearSigmaUp));
      ele.addUserFloat("PtScale_scaleUpResDown", CLHEP::RandGauss::shoot(&engine, 1-scaleError, smearSigmaDown));
      ele.addUserFloat("PtScale_scaleDownResUp", CLHEP::RandGauss::shoot(&engine, 1+scaleError, smearSigmaUp));
      ele.addUserFloat("PtScale_scaleDownResDown", CLHEP::RandGauss::shoot(&engine, 1+scaleError, smearSigmaDown));

      if ((rhoResShift != 0. || phiResShift != 0.) && shiftCollection)
        {
          float smearSigma =
            correcter.getSmearingSigma(iEvent.id().run(), isEB, r9, absEta,
                                       et, gainSeedSC, rhoResShift, phiResShift);

          scale = CLHEP::RandGauss::shoot(&engine, scale, smearSigma);
        }

      // Replace the electron collection 
      if (shiftCollection)
        ele.setP4(math::PtEtaPhiMLorentzVector(scale*ele.pt(), ele.eta(),
                                                ele.phi(), ele.mass()));
    }

  iEvent.put(std::move(out));
}

DEFINE_FWK_MODULE(PATElectronSystematicShifter);
