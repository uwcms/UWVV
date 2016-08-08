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
#include "UWVV/Utilities/interface/helpers.h"

namespace uwvv
{

  template<class Obj1, class Obj2> struct CompositeDaughter {};
  
  
  template<class T> class BranchManager
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
    const std::string& extractDaughterName(const size_t i, 
                                           const std::vector<std::string>& names) const;

    bool daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const;
    
    std::unique_ptr<BranchManager<T1> > daughterBranches1;
    std::unique_ptr<BranchManager<T2> > daughterBranches2;

    const std::string daughterName1;
    const std::string daughterName2;
    const std::string fsrLabel;
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
    BranchManager<pat::CompositeCandidate>(name, tree, config),
    daughterName1(extractDaughterName(0,
                                      config.getParameter<std::vector<std::string> >("daughterNames"))),
    daughterName2(extractDaughterName(1,
                                      config.getParameter<std::vector<std::string> >("daughterNames"))),
    fsrLabel(config.exists("fsrLabel") ? 
             config.getParameter<std::string>("fsrLabel") :
             "")
  {
    std::vector<edm::ParameterSet> daughterParams = 
      config.getParameter<std::vector<edm::ParameterSet> >("daughterParams");
    
    if(daughterParams.size() < 2)
      throw cms::Exception("InvalidParams")
        << "You must provide two sets of daughter parameters for a composite "
        << "candidate with two daughters." << std::endl;

    daughterBranches1 = 
      std::unique_ptr<BranchManager<T1> >(new BranchManager<T1>(daughterName1,
                                                                tree, 
                                                                daughterParams.at(0)));
    daughterBranches2 = 
      std::unique_ptr<BranchManager<T2> >(new BranchManager<T2>(daughterName2,
                                                                tree,
                                                                daughterParams.at(1)));
  }
    

  template<>
  template<class T1, class T2>
  const std::string&
  BranchManager<CompositeDaughter<T1,T2> >::extractDaughterName(const size_t i,
                                                                const std::vector<std::string>& names) const
    {
      if(names.size() < i+1)
        throw cms::Exception("InvalidNames")
          << "You must provide at least " << i+1
          << " names for a composite candidate with "
          << i+1 << " daughters." << std::endl;

      return names.at(i);      
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
  
    size_t iDau1 = 0;
    size_t iDau2 = 1;

    if(daughtersNeedReorder(obj))
      {
        iDau1 = 1;
        iDau2 = 0;
      }

    daughterBranches1->fill(obj->daughter(iDau1), evt);
    daughterBranches2->fill(obj->daughter(iDau2), evt);
  }

  
  // Most things don't need to be reordered
  template<>
  template<class T1, class T2> bool
    BranchManager<CompositeDaughter<T1,T2> >::daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const
    {
      return false;
    }

  // within a Z, leptons should be ordered in pt
  template<> bool
    BranchManager<CompositeDaughter<pat::Electron, pat::Electron> >::daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const
    {
      return cand->daughter(1)->pt() > cand->daughter(0)->pt();
    }
  template<> bool
    BranchManager<CompositeDaughter<pat::Muon, pat::Muon> >::daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const
    {
      return cand->daughter(1)->pt() > cand->daughter(0)->pt();
    }

  // 4e and 4mu candidates should be ordered by Z compatibility
  template<> bool
    BranchManager<CompositeDaughter<CompositeDaughter<pat::Electron, pat::Electron>, 
    CompositeDaughter<pat::Electron, pat::Electron> > >::daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const
    {
      return helpers::zsNeedReorder<pat::Electron, pat::Electron>(cand, fsrLabel);
    }
  template<> bool
    BranchManager<CompositeDaughter<CompositeDaughter<pat::Muon, pat::Muon>, 
    CompositeDaughter<pat::Muon, pat::Muon> > >::daughtersNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand) const
    {
      return helpers::zsNeedReorder<pat::Muon, pat::Muon>(cand, fsrLabel);
    }

} // namespace

#endif // header guard
