
// STL
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// CMSSW
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


template<class T>
class PATObjectPrinter : public edm::one::EDAnalyzer<>
{
public:
  explicit PATObjectPrinter(const edm::ParameterSet& config);
  ~PATObjectPrinter() {;}

private:
  virtual void analyze(edm::Event const& evt,
                       edm::EventSetup const& setup);

  void printCandInfo(const T& cand) const;

  const edm::EDGetTokenT<edm::View<T> > candToken;

  std::string getObjectName() const {return "Candidate";}

  void anythingElse(const T& cand) const {}
};


template<class T>
PATObjectPrinter<T>::PATObjectPrinter(const edm::ParameterSet& config) :
  candToken(consumes<edm::View<T> >(config.getParameter<edm::InputTag>("src")))
{
}


template<class T>
void PATObjectPrinter<T>::analyze(edm::Event const& event, edm::EventSetup const& setup)
{
  edm::Handle<edm::View<T> > candsIn;
  event.getByToken(candToken, candsIn);

  if(candsIn->size())
    std::cout << event.id().run() << ":" << event.id().luminosityBlock() <<":"
              << event.id().event() << ":"
              << "(" << candsIn->size() << " " << getObjectName() << "s)" << std::endl;
  else
    std::cout << "    ... but the collection is empty." << std::endl;

  for(size_t i = 0; i < candsIn->size(); ++i)
    printCandInfo(candsIn->at(i));

  std::cout << std::endl;
}


template<class T>
void PATObjectPrinter<T>::printCandInfo(const T& cand) const
{
  std::cout << "id: " << cand.pdgId() << " m: " << cand.mass()
            << " pt: " << cand.pt()
            << " eta: " << cand.eta() << " phi: " << cand.phi();
  anythingElse(cand);

  if(cand.hasUserFloat("idLoose"))
    std::cout << " Loose ID: " << cand.userFloat("idLoose");

  if(cand.hasUserInt("pileupJetIdUpdated:fullId"))
    std::cout << " PUID: " << cand.userInt("pileupJetIdUpdated:fullId");

  std::cout << std::endl;
}


template<>
std::string PATObjectPrinter<pat::Jet>::getObjectName() const
{
  return "jet";
}


template<>
std::string PATObjectPrinter<pat::Electron>::getObjectName() const
{
  return "electron";
}


template<>
std::string PATObjectPrinter<pat::Muon>::getObjectName() const
{
  return "muon";
}


template<>
void PATObjectPrinter<pat::Electron>::anythingElse(const pat::Electron& cand) const
{
  std::cout << " SIP3D: " << fabs(cand.dB(pat::Electron::PV3D)) / cand.edB(pat::Electron::PV3D);
  std::cout << " SCEta: " << cand.superCluster()->eta();
}


typedef PATObjectPrinter<pat::Jet> JetPrinter;
typedef PATObjectPrinter<pat::Electron> ElectronPrinter;
typedef PATObjectPrinter<pat::Muon> MuonPrinter;


DEFINE_FWK_MODULE(JetPrinter);
DEFINE_FWK_MODULE(ElectronPrinter);
DEFINE_FWK_MODULE(MuonPrinter);
