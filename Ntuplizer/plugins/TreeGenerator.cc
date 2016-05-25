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
#include "UWVV/Ntuplizer/interface/EventInfo.h"
#include "UWVV/Ntuplizer/interface/TriggerBranches.h"


using namespace uwvv;

template<class... Ts> 
class TreeGenerator : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
 public:
  explicit TreeGenerator(const edm::ParameterSet&);
  virtual ~TreeGenerator() {;}

 private:
  virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iConfig) override;

  TTree* const makeTree() const;

  const edm::EDGetTokenT<edm::View<pat::CompositeCandidate> > candToken;

  TTree* const tree;
  EventInfo evtInfo;

  std::unique_ptr<BranchManager<Ts...> > branches;
  std::unique_ptr<TriggerBranches> triggerBranches;
};


template<class... FSParticles>
TreeGenerator<FSParticles...>::TreeGenerator(const edm::ParameterSet& config) :
  candToken(consumes<edm::View<pat::CompositeCandidate> >(config.getParameter<edm::InputTag>("src"))),
  tree(makeTree()),
  evtInfo(consumesCollector(), config.getParameter<edm::ParameterSet>("eventParams"))
{
  usesResource("TFileService");

  const edm::ParameterSet& branchParams = config.getParameter<edm::ParameterSet>("branches");
  branches = 
    std::unique_ptr<BranchManager<FSParticles...> >(new BranchManager<FSParticles...>("", tree, branchParams));

  const edm::ParameterSet& triggers = config.getParameter<edm::ParameterSet>("triggers");  
  triggerBranches = std::unique_ptr<TriggerBranches>(new TriggerBranches(consumesCollector(),
                                                                         triggers, tree));
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

  evtInfo.setEvent(event);
  triggerBranches->setEvent(event);

  for(size_t i = 0; i < cands->size(); ++i)
    {
      branches->fill(cands->ptrAt(i), evtInfo);
      triggerBranches->fill();

      tree->Fill();
    }
}


typedef TreeGenerator<CompositeDaughter<CompositeDaughter<pat::Electron, pat::Electron>, 
                                        CompositeDaughter<pat::Electron, pat::Electron> 
                                        > 
                      > TreeGeneratorEEEE;
typedef TreeGenerator<CompositeDaughter<CompositeDaughter<pat::Electron, pat::Electron>, 
                                        CompositeDaughter<pat::Muon, pat::Muon> 
                                        > 
                      > TreeGeneratorEEMuMu;
typedef TreeGenerator<CompositeDaughter<CompositeDaughter<pat::Muon, pat::Muon>, 
                                        CompositeDaughter<pat::Muon, pat::Muon> 
                                        > 
                      > TreeGeneratorMuMuMuMu;

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(TreeGeneratorEEEE);
DEFINE_FWK_MODULE(TreeGeneratorEEMuMu);
DEFINE_FWK_MODULE(TreeGeneratorMuMuMuMu);


