#ifndef UWVV_Ntuplizer_BranchManager_h
#define UWVV_Ntuplizer_BranchManager_h

// STL
#include <string>
#include <functional>
#include <vector>
#include <memory>

// ROOT
#include "TTree.h"
#include "TMath.h"

// CMSSW
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/Ptr.h"

// UWVV
#include "UWVV/Ntuplizer/interface/EventInfo.h"
#include "UWVV/Ntuplizer/interface/BranchHolder.h"
#include "UWVV/Ntuplizer/interface/FunctionLibrary.h"


namespace uwvv
{

  template<class Obj1, class Obj2> struct CompositeDaughter {};
  
  
  template<class... Ts> class BranchManager;
  
  template<class T> class BranchManager<T>
  {
   public:
    BranchManager() {;}
    BranchManager(const std::string& name, TTree* const tree,
                  const edm::ParameterSet& config);
    virtual ~BranchManager(){;}
    
    void fill(const reco::Candidate* const obj, EventInfo& evt);
    void fill(const edm::Ptr<T>& obj, EventInfo& evt);
  
    const std::string& getName() const {return name;}
  
   protected:
    edm::Ptr<T> extractMasterPtr(const reco::Candidate* const);
  
   private:
    template<typename B> void
      addBranchesFromPSet(std::vector<std::unique_ptr<BranchHolder<B, T> > >& addTo,
                          const edm::ParameterSet& toAdd, 
                          TTree* const tree);
  
    const std::string name;
  
    std::vector<std::unique_ptr<BranchHolder<float, T> > >              floatBranches;
    std::vector<std::unique_ptr<BranchHolder<bool, T> > >               boolBranches;
    std::vector<std::unique_ptr<BranchHolder<int, T> > >                intBranches;
    std::vector<std::unique_ptr<BranchHolder<unsigned, T> > >           uintBranches;
    std::vector<std::unique_ptr<BranchHolder<unsigned long long, T> > > ullBranches;
  };
  
  
  template<>
  template<class T1, class T2> 
  class BranchManager<CompositeDaughter<T1, T2> > : 
    public BranchManager<pat::CompositeCandidate>
  {
   public:
    BranchManager() {;}
    BranchManager(const std::string& name, TTree* const tree,
                  const edm::ParameterSet& config);
    virtual ~BranchManager() {;}
  
    void fill(const reco::Candidate* const obj, EventInfo& evt);
    void fill(const edm::Ptr<pat::CompositeCandidate> & obj, EventInfo& evt);
  
   private:
    std::unique_ptr<BranchManager<T1> > daughterBranches1;
    std::unique_ptr<BranchManager<T2> > daughterBranches2;
  };
  
  
  
  template<class T>
  BranchManager<T>::BranchManager(const std::string& name, TTree* const tree,
                                  const edm::ParameterSet& config) :
    name(name)
  {
    if(config.exists("floats"))
      addBranchesFromPSet(floatBranches,
                          config.getParameter<edm::ParameterSet>("floats"),
                          tree);
  
    if(config.exists("bools"))
      addBranchesFromPSet(boolBranches,
                          config.getParameter<edm::ParameterSet>("bools"),
                          tree);
  
    if(config.exists("ints"))
      addBranchesFromPSet(intBranches,
                          config.getParameter<edm::ParameterSet>("ints"),
                          tree);
  
    if(config.exists("uints"))
      addBranchesFromPSet(uintBranches,
                          config.getParameter<edm::ParameterSet>("uints"),
                          tree);
  
    if(config.exists("ulls"))
      addBranchesFromPSet(ullBranches,
                          config.getParameter<edm::ParameterSet>("ulls"),
                          tree);
  }
  
  
  template<class T>
  template<typename B> void
  BranchManager<T>::addBranchesFromPSet(std::vector<std::unique_ptr<BranchHolder<B, T> > >& addTo,
                                        const edm::ParameterSet& toAdd, 
                                        TTree* const tree)
  {
    FunctionLibrary<B,T> fLib = FunctionLibrary<B,T>();
  
    for(auto&& b : toAdd.getParameterNames())
      addTo.push_back(std::unique_ptr<BranchHolder<B, T> >(new BranchHolder<B, T>(getName()+b, 
                                                                                  tree, 
                                                                                  fLib.getFunction(toAdd.getParameter<std::string>(b)))));
  }
  
  
  template<class T> void
  BranchManager<T>::fill(const reco::Candidate* const abstractObject, 
                         EventInfo& evt)
  {
    edm::Ptr<T> obj = extractMasterPtr(abstractObject);
  
    fill(obj, evt);
  }
  
  
  template<class T> void
  BranchManager<T>::fill(const edm::Ptr<T>& obj, EventInfo& evt)
  {
    for(auto&& b : floatBranches)
      b->fill(obj, evt);
  
    for(auto&& b : boolBranches)
      b->fill(obj, evt);
  
    for(auto&& b : intBranches)
      b->fill(obj, evt);
  
    for(auto&& b : uintBranches)
      b->fill(obj, evt);
  
    for(auto&& b : ullBranches)
      b->fill(obj, evt);
  }
  
  
  template<class T>
  edm::Ptr<T> BranchManager<T>::extractMasterPtr(const reco::Candidate* const obj)
  {
    if(!obj->hasMasterClone())
      throw cms::Exception("InvalidObject")
        << "Invalid " << this->getName() << " object passed to Ntuplizer";
  
    return obj->masterClone().castTo<edm::Ptr<T> >();
  }
  
  
  template<>
  template<class T1, class T2>
  BranchManager<CompositeDaughter<T1, T2> >::BranchManager(const std::string& name, 
                                                           TTree* const tree,
                                                           const edm::ParameterSet& config) :
    BranchManager<pat::CompositeCandidate>(name, tree, config)
  {
    std::vector<edm::ParameterSet> daughterParams = 
      config.getParameter<std::vector<edm::ParameterSet> >("daughterParams");
    
    std::vector<std::string> daughterNames = 
      config.getParameter<std::vector<std::string> >("daughterNames");
  
    daughterBranches1 = 
      std::unique_ptr<BranchManager<T1> >(new BranchManager<T1>(daughterNames.at(0), 
                                                                tree, 
                                                                daughterParams.at(0)));
    daughterBranches2 = 
      std::unique_ptr<BranchManager<T2> >(new BranchManager<T2>(daughterNames.at(1), 
                                                                tree,
                                                                daughterParams.at(1)));
  }
  
  
  template<>
  template<class T1, class T2> void
  BranchManager<CompositeDaughter<T1, T2> >::fill(const reco::Candidate* const abstractObject, 
                                                  EventInfo& evt)
  {
    edm::Ptr<pat::CompositeCandidate> obj = extractMasterPtr(abstractObject);
  
    fill(obj, evt);
  }
  
  
  template<>
  template<class T1, class T2> void
  BranchManager<CompositeDaughter<T1, T2> >::fill(const edm::Ptr<pat::CompositeCandidate>& obj, 
                                                  EventInfo& evt)
  {
    if(obj.isNull() || obj->numberOfDaughters() != 2)
      throw cms::Exception("InvalidObject")
        << "Invalid " << this->getName() 
        <<" CompositeCandidate object passed to Ntuplizer";
  
    BranchManager<pat::CompositeCandidate>::fill(obj, evt);
  
    daughterBranches1->fill(obj->daughter(0), evt);
    daughterBranches2->fill(obj->daughter(1), evt);
  }

} // namespace

#endif // header guard
