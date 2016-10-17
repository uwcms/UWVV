//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    PATMuonKalmanCorrector                                                //
//                                                                          //
//    Takes PAT muons, corrects their pt with the Kalman method.            //
//                                                                          //
//    Nate Woods, U. Wisconsin                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////



#include <vector>
#include <string>
#include <memory>

#include "TLorentzVector.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "KaMuCa/Calibration/interface/KalmanMuonCalibrator.h"


class PATMuonKalmanCorrector : public edm::stream::EDProducer<>
{
public:
  explicit PATMuonKalmanCorrector(const edm::ParameterSet& params);
  virtual ~PATMuonKalmanCorrector(){;}

private:
  virtual void produce(edm::Event& event, const edm::EventSetup& setup);

  const edm::EDGetTokenT<edm::View<pat::Muon> > srcToken;
  const std::string corType;
  const bool isMC;
  std::unique_ptr<KalmanMuonCalibrator> calib;

  const bool isSync;

  const float maxPt;

  const int closureShift;
};


PATMuonKalmanCorrector::PATMuonKalmanCorrector(const edm::ParameterSet& params) :
  srcToken(consumes<edm::View<pat::Muon> >(params.getParameter<edm::InputTag>("src"))),
  corType(params.getParameter<std::string>("calibType")),
  isMC(corType.find("MC") != std::string::npos),
  calib(new KalmanMuonCalibrator(corType)),
  isSync(params.exists("isSync") && isMC && params.getParameter<bool>("isSync")),
  maxPt(params.exists("maxPt") ? params.getParameter<double>("maxPt") : 200.),
  closureShift(params.exists("closureShift") ?
               params.getParameter<int>("closureShift") : 0)
{
  produces<pat::MuonCollection>();

  if(closureShift)
    calib->varyClosure(closureShift);

  std::cout << "Shift: " << closureShift << std::endl;
}


void
PATMuonKalmanCorrector::produce(edm::Event& event, const edm::EventSetup& setup)
{
  edm::Handle<edm::View<pat::Muon> > in;
  event.getByToken(srcToken, in);

  std::unique_ptr<std::vector<pat::Muon> > out(new std::vector<pat::Muon>);

  for(size_t i = 0; i < in->size(); i++)
    {
      edm::Ptr<pat::Muon> muIn = in->ptrAt(i);

      float pt = muIn->pt();

      out->push_back(*muIn);

      if(muIn->muonBestTrackType() == 1 && pt < maxPt)
        {
          float eta = muIn->eta();
          float phi = muIn->phi();
          float ptErr = muIn->bestTrack()->ptError();

          if(isMC || (pt > 2. && fabs(eta) < 2.4))
            {
              float ptOld = pt;
              std::cout << "Raw: " << pt << " ";
              pt = calib->getCorrectedPt(pt, eta, phi, muIn->charge());
              calib->varyClosure(0);
              std::cout << "unshifted: " << calib->getCorrectedPt(ptOld, eta, phi, muIn->charge()) << " ";
              calib->varyClosure(1);
              std::cout << "+1: " << calib->getCorrectedPt(ptOld,eta,phi,muIn->charge()) << " ";
              calib->varyClosure(closureShift);
              std::cout << "shifted: " << pt << std::endl;
            }

          if(isMC)
            {
              if(isSync)
                pt = calib->smearForSync(pt, eta);
              else
                pt = calib->smear(pt, eta);
            }

          ptErr = pt * calib->getCorrectedError(pt, eta, ptErr/pt);

          out->back().setP4(reco::Particle::PolarLorentzVector(pt, eta, phi, muIn->mass()));
          out->back().addUserFloat("kalmanPtError", ptErr);
          out->back().addUserCand("uncorrected", muIn);
        }
    }

  std::cout << std::endl;

  event.put(std::move(out));
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATMuonKalmanCorrector);

