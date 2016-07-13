///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    PATObjectScaleFactorEmbedder                                           //
//                                                                           //
//    Takes a TH2F and a collection of PAT objects and embeds the            //
//    content of the bin at (obj.pt(), obj.eta()) as a userFloat             //
//                                                                           //
//    Nate Woods, U. Wisconsin                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>
#include <string>
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
#include "DataFormats/Common/interface/View.h"

// ROOT includes
#include "TH2F.h"
#include "TFile.h"


template<typename T>
class PATObjectScaleFactorEmbedder : public edm::stream::EDProducer<>
{

public:
  explicit PATObjectScaleFactorEmbedder(const edm::ParameterSet& iConfig);
  virtual ~PATObjectScaleFactorEmbedder() {};

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  const edm::EDGetTokenT<edm::View<T> > srcToken;
  const std::unique_ptr<TFile> file;
  const std::unique_ptr<TH2F> h;
  const std::string label;
};


template<typename T>
PATObjectScaleFactorEmbedder<T>::PATObjectScaleFactorEmbedder(const edm::ParameterSet& iConfig) :
  srcToken(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
  file(new TFile(iConfig.getParameter<std::string>("fileName").c_str())),
  h((TH2F*)(file->Get(iConfig.getParameter<std::string>("histName").c_str())->Clone())),
  label(iConfig.getParameter<std::string>("label"))
{
  if(file->IsZombie())
    throw cms::Exception("InvalidFile") 
      << "Scale factor file "<< iConfig.getParameter<std::string>("fileName")
      << " does not exist!" << std::endl;

  produces<std::vector<T> >();
}


template<typename T>
void PATObjectScaleFactorEmbedder<T>::produce(edm::Event& iEvent,
                                              const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<T> > in;
  std::unique_ptr<std::vector<T> > out(new std::vector<T>);

  iEvent.getByToken(srcToken, in);

  for(size_t i = 0; i < in->size(); ++i)
    {
      const T& t = in->at(i);

      // don't just give 0 for under/overflow
      int bin = h->FindBin(t.eta(), t.pt());
      if(h->IsBinOverflow(bin))
        {
          int binx, biny, binz;
          h->GetBinXYZ(bin, binx, biny, binz);
          if(binx > h->GetNbinsX())
            binx -= 1;
          if(biny > h->GetNbinsY())
            biny -= 1;
          
          bin = h->GetBin(binx, biny, binz);
        }
      if(h->IsBinUnderflow(bin))
        {
          int binx, biny, binz;
          h->GetBinXYZ(bin, binx, biny, binz);
          if(!binx)
            binx += 1;
          if(!biny)
            biny += 1;
          
          bin = h->GetBin(binx, biny, binz);
        }

      float value = h->GetBinContent(bin);

      out->push_back(t);
      out->back().addUserFloat(label, value);
    }

  iEvent.put(std::move(out));
}


typedef PATObjectScaleFactorEmbedder<pat::Electron> PATElectronScaleFactorEmbedder;
typedef PATObjectScaleFactorEmbedder<pat::Muon> PATMuonScaleFactorEmbedder;
typedef PATObjectScaleFactorEmbedder<pat::Tau> PATTauScaleFactorEmbedder;
typedef PATObjectScaleFactorEmbedder<pat::Jet> PATJetScaleFactorEmbedder;
typedef PATObjectScaleFactorEmbedder<pat::CompositeCandidate> PATCompositeCandidateScaleFactorEmbedder;

DEFINE_FWK_MODULE(PATElectronScaleFactorEmbedder);
DEFINE_FWK_MODULE(PATMuonScaleFactorEmbedder);
DEFINE_FWK_MODULE(PATTauScaleFactorEmbedder);
DEFINE_FWK_MODULE(PATJetScaleFactorEmbedder);
DEFINE_FWK_MODULE(PATCompositeCandidateScaleFactorEmbedder);
