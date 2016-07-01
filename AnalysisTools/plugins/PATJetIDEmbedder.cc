//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    PATJetIDEmbedder.cc                                                   //
//                                                                          //
//    Embed basic PF Jet IDs as userFloats                                  //
//                                                                          //
//    Author: Nate Woods, U. Wisconsin                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


#include<memory>
#include<string>
#include<vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


typedef pat::Jet Jet;
typedef std::vector<Jet> VJet;
typedef edm::View<Jet> JetView;

class PATJetIDEmbedder : public edm::stream::EDProducer<>
{
 public:
  explicit PATJetIDEmbedder(const edm::ParameterSet& pset);
  virtual ~PATJetIDEmbedder() {;}

 private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  bool passLoose(const Jet& jet) const;
  bool passTight(const Jet& jet) const;
  bool passTightLepVeto(const Jet& jet) const;
  bool passPUID(const Jet& jet) const;

  edm::EDGetTokenT<JetView> srcToken;
};


PATJetIDEmbedder::PATJetIDEmbedder(const edm::ParameterSet& pset) :
  srcToken(consumes<JetView>(pset.getParameter<edm::InputTag>("src")))
{
  produces<VJet>();
}


void PATJetIDEmbedder::produce(edm::Event& iEvent, 
                               const edm::EventSetup& iSetup)
{
  edm::Handle<JetView> in;
  iEvent.getByToken(srcToken, in);

  std::unique_ptr<VJet> out(new VJet());

  for(size_t i = 0; i < in->size(); ++i)
    {
      out->push_back(in->at(i)); // copies, transfers ownership

      Jet& jet = out->back();

      jet.addUserFloat("idLoose", float(passLoose(jet)));
      jet.addUserFloat("idTight", float(passTight(jet)));
      jet.addUserFloat("idPU", float(passPUID(jet)));
    }

  iEvent.put(std::move(out));
}


bool PATJetIDEmbedder::passLoose(const Jet& jet) const
{
  float absEta = std::abs(jet.eta());

  if(absEta > 3.)
    {
      return jet.neutralEmEnergyFraction() < 0.90 && jet.neutralMultiplicity() > 10;
    }

  bool result = (jet.neutralHadronEnergyFraction() < 0.99 && 
                 jet.neutralEmEnergyFraction() < 0.99 &&
                 (jet.chargedMultiplicity() + jet.neutralMultiplicity()) > 1);

  if(absEta < 2.4)
    result &= (jet.chargedHadronEnergyFraction() > 0. &&
               jet.chargedMultiplicity() > 0 &&
               jet.chargedEmEnergyFraction() < 0.99);

  return result;
}


bool PATJetIDEmbedder::passTight(const Jet& jet) const
{
  bool loose = passLoose(jet);
  if(!loose) return false;

  float absEta = std::abs(jet.eta());

  if(absEta > 3.)
      return loose;

  return (jet.neutralHadronEnergyFraction() < 0.90 &&
          jet.neutralEmEnergyFraction() < 0.90);
}


bool PATJetIDEmbedder::passTightLepVeto(const Jet& jet) const
{
  float absEta = std::abs(jet.eta());

  if(absEta > 3.) // no meaning past endcaps
      return false;

  bool tight = passTight(jet);
  if(!tight) return false;

  bool result = tight && jet.muonEnergyFraction() < 0.8;

  if(absEta < 2.4)
    result &= jet.chargedEmEnergyFraction() < 0.90;

  return result;
}


bool PATJetIDEmbedder::passPUID(const Jet& jet) const
{
  if(!jet.hasUserFloat("pileupJetId:fullDiscriminant"))
    return false;

  float mva = jet.userFloat("pileupJetId:fullDiscriminant");

  float absEta = std::abs(jet.eta());

  if(jet.pt() > 20.)
    {
      if(absEta > 3. && mva <= -0.45) return false;
      if(absEta > 2.75 && mva <= 0.55) return false;
      if(absEta > 2.5 && mva <= -0.6) return false;
      if(mva <= -0.63) return false;
    }
  else
    {
      if(absEta > 3. && mva <= -0.95) return false;
      if(absEta > 2.75 && mva <= -0.94) return false;
      if(absEta > 2.5 && mva <= -0.96) return false;
      if(mva <= -0.95) return false;
    }

  return true;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATJetIDEmbedder);
