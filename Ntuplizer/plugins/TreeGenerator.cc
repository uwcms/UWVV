/////////////////////////////////////////////////////////////////////////////    
//                                                                         //    
//    TreeGenerator                                                        //    
//                                                                         //    
//    A builder of ntuples from composite candidates.                      //    
//                                                                         //    
//    Nate Woods, U. Wisconsin                                             //    
//                                                                         //    
/////////////////////////////////////////////////////////////////////////////    


//STL
#include <memory>

// CMSSW
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// ROOT
#include "TTree.h"

// UWVV
#include "UWVV/Ntuplizer/interface/BranchManager.h"
//#include "UWVV/Ntuplizer/interface/BranchManagerFwd.h"



template<class... Ts> 
class TreeGenerator : public edm::one::EDAnalyzer<edm::one::SharedResources>//, 
                      //edm::one::WatchLuminosityBlocks>
{
 public:
  explicit TreeGenerator(const edm::ParameterSet&);
  virtual ~TreeGenerator() {;}

 private:
  virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iConfig) override;

  TTree* const makeTree() const;

  const edm::EDGetTokenT<edm::View<pat::CompositeCandidate> > candToken;

  TTree* const tree;

  std::unique_ptr<BranchManager<Ts...> > branches;
};


template<class... FSParticles>
TreeGenerator<FSParticles...>::TreeGenerator(const edm::ParameterSet& config) :
  candToken(consumes<edm::View<pat::CompositeCandidate> >(config.getParameter<edm::InputTag>("src"))),
  tree(makeTree())
{
  usesResource("TFileService");

  branches = 
    std::unique_ptr<BranchManager<FSParticles...> >(new BranchManager<FSParticles...>("", tree));
}


template<class... FSParticles>
TTree* const TreeGenerator<FSParticles...>::makeTree() const
{
  edm::Service<TFileService> FS;

  return FS->make<TTree>("ntuple", "ntuple");
}


template<class... FSParticles> void
TreeGenerator<FSParticles...>::analyze(const edm::Event &event,
                                       const edm::EventSetup &setup)
{
  edm::Handle<edm::View<pat::CompositeCandidate> > cands;
  event.getByToken(candToken, cands);

  for(size_t i = 0; i < cands->size(); ++i)
    {
      branches->fill(cands->ptrAt(i));

      tree->Fill();
    }
}


typedef TreeGenerator<CompositeDaughter<CompositeDaughter<pat::Electron, pat::Electron>, 
                                        CompositeDaughter<pat::Muon, pat::Muon> 
                                        > 
                      > TreeGeneratorEEMuMu;

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(TreeGeneratorEEMuMu);


