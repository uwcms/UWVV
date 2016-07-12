//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    PATJetSmearing.cc                                                     //
//                                                                          //
//    Apply energy smearing to PF jets. Outputs a collection of jets with   //
//    the smearing applied and with corrections that undo the smearing or   //
//    apply +/- 1sigma corrections for for systematics.                     //
//    To get the original 4-momentum back:                                  //
//        jet.p4() * jet.userFloat("jerCorrInverse")                        //
//    To get the 4-momentum with JER+1sigma:                                //
//        jet.p4() * jet.userFloat("jerCorrUp")                             //
//    To get the 4-momentum with JER-1sigma:                                //
//        jet.p4() * jet.userFloat("jerCorrDown")                           //
//                                                                          //
//    Obviously, this only makes sense for MC                               //
//                                                                          //
//    Author: Nate Woods, U. Wisconsin                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include<memory>
#include<string>
#include<vector>
#include<cmath> // std::sqrt, std::abs, std::sin
#include<algorithm> // std::max

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetResolutionObject.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "TRandom3.h"


typedef pat::Jet Jet;
typedef std::vector<Jet> VJet;
typedef edm::View<Jet> JetView;

class PATJetSmearing : public edm::stream::EDProducer<>
{
 public:
  explicit PATJetSmearing(const edm::ParameterSet& pset);
  virtual ~PATJetSmearing() {;}

 private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  edm::EDGetTokenT<JetView> srcToken;
  edm::EDGetTokenT<double> rhoToken;
};


PATJetSmearing::PATJetSmearing(const edm::ParameterSet& pset) :
  srcToken(consumes<JetView>(pset.getParameter<edm::InputTag>("src"))),
  rhoToken(consumes<double>(pset.getParameter<edm::InputTag>("rhoSrc")))
{
  produces<VJet>();
}


void PATJetSmearing::produce(edm::Event& iEvent, 
                             const edm::EventSetup& iSetup)
{
  edm::Handle<JetView> in;
  iEvent.getByToken(srcToken, in);
  edm::Handle<double> rho;
  iEvent.getByToken(rhoToken, rho);

  std::unique_ptr<VJet> out(new VJet());

  JME::JetResolutionScaleFactor resSF = 
    JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");
  JME::JetResolution resPt = JME::JetResolution::get(iSetup, "AK4PFchs_pt");

  for(size_t i = 0; i < in->size(); ++i)
    {
      const edm::Ptr<Jet> jetPtr = in->ptrAt(i);
      out->push_back(*jetPtr); // copies, transfers ownership

      Jet& jet = out->back(); // for convenience
      
      float pt = jet.pt();
      float eta = jet.eta();
      float phi = jet.phi();

      JME::JetParameters params;
      params.setJetPt(pt).setJetEta(eta).setRho(*rho);

      float relPtErr = resPt.getResolution(params);

      double jerCorr = 1.;
      double jerCorrUp = 1.;
      double jerCorrDn = 1.;

      JME::JetParameters paramsSF;
      paramsSF.setJetEta(eta).setRho(*rho);
      
      float sf = resSF.getScaleFactor(paramsSF);
      float sfUp = resSF.getScaleFactor(paramsSF, Variation::UP);
      float sfDn = resSF.getScaleFactor(paramsSF, Variation::DOWN);

      double ptJER, ptJERUp, ptJERDn;
      const reco::GenJet* gen = jet.genJet();
      if(gen && reco::deltaR(eta, phi, gen->eta(), gen->phi()) < 0.2 &&
         (std::abs(pt - gen->pt()) < 3. * relPtErr * pt))
        {
          float genPt = gen->pt();

          ptJER = std::max(float(0.), genPt + sf * (pt - genPt));
          ptJERUp = std::max(float(0.), genPt + sfUp * (pt - genPt));
          ptJERDn = std::max(float(0.), genPt + sfDn * (pt - genPt));
        }
      else
        {
          TRandom3 rand;
          rand.SetSeed(std::abs(static_cast<int>(std::sin(phi)*100000)));
          float smear = rand.Gaus(0.,1.);
          float sig = std::sqrt(sf * sf - 1.) * relPtErr * pt;
          float sigUp = std::sqrt(sfUp * sfUp - 1.) * relPtErr * pt;
          float sigDn = std::sqrt(sfDn * sfDn - 1.) * relPtErr * pt;
          ptJER = std::max(float(0.), smear * sig + pt);
          ptJERUp = std::max(float(0.), smear * sigUp + pt);
          ptJERDn = std::max(float(0.), smear * sigDn + pt);
        }

      jerCorr = ptJER / pt;
      jerCorrUp = ptJERUp / pt;
      jerCorrDn = ptJERDn / pt;

      math::XYZTLorentzVector p4JER = jerCorr * jet.p4();
      jet.setP4(p4JER);
      jet.addUserFloat("jerCorrInverse", 1./jerCorr);
      jet.addUserFloat("jerCorrUp", jerCorrUp/jerCorr);
      jet.addUserFloat("jerCorrDown", jerCorrDn/jerCorr);
    }

  iEvent.put(std::move(out));
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATJetSmearing);
