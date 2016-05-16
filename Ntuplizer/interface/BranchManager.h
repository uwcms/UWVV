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
//#include "UWVV/Ntuplizer/interface/EventInfo.h"
#include "UWVV/Ntuplizer/interface/BranchHolder.h"


template<class Obj1, class Obj2> struct CompositeDaughter {};


template<class... Ts> class BranchManager;

template<class T> class BranchManager<T>
{
 public:
  BranchManager() {;}
  BranchManager(const std::string& name, TTree* const tree);
  virtual ~BranchManager(){;}
  
  void fill(const reco::Candidate* const obj);
  void fill(const edm::Ptr<T>& obj);

  const std::string& getName() const {return name;}

 protected:
  edm::Ptr<T> extractMasterPtr(const reco::Candidate* const);

 private:
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
  BranchManager(const std::string& name, TTree* const tree);
  virtual ~BranchManager() {;}

  void fill(const reco::Candidate* const obj);
  void fill(const edm::Ptr<pat::CompositeCandidate> & obj);

 private:
  std::string getDaughterName(size_t whichDaughter) const;
  
  std::unique_ptr<BranchManager<T1> > daughterBranches1;
  std::unique_ptr<BranchManager<T2> > daughterBranches2;
};



template<class T>
BranchManager<T>::BranchManager(const std::string& name, TTree* const tree) :
  name(name)
{
  floatBranches.push_back(std::unique_ptr<BranchHolder<float, T> >(new BranchHolder<float, T>(name+"Mass", tree, "mass")));
  boolBranches.push_back(std::unique_ptr<BranchHolder<bool, T> >(new BranchHolder<bool, T>(name+"IsMuon", tree, "isMuon")));
  intBranches.push_back(std::unique_ptr<BranchHolder<int, T> >(new BranchHolder<int, T>(name+"Charge", tree, "charge")));
  uintBranches.push_back(std::unique_ptr<BranchHolder<unsigned, T> >(new BranchHolder<unsigned, T>(name+"NDaughters", tree, "numberOfDaughters")));

  floatBranches.push_back(std::unique_ptr<BranchHolder<float, T > >(new BranchHolder<float, T>(name+"Pt", tree, [](const edm::Ptr<T>& obj){return obj->pt();})));
}


template<class T> void
BranchManager<T>::fill(const reco::Candidate* const abstractObject)
{
  edm::Ptr<T> obj = extractMasterPtr(abstractObject);

  fill(obj);
}


template<class T> void
BranchManager<T>::fill(const edm::Ptr<T>& obj)
{
  for(auto&& b : floatBranches)
      b->fill(obj);

  for(auto&& b : boolBranches)
      b->fill(obj);

  for(auto&& b : intBranches)
      b->fill(obj);

  for(auto&& b : uintBranches)
      b->fill(obj);

  for(auto&& b : ullBranches)
      b->fill(obj);
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
                                                         TTree* const tree) :
  BranchManager<pat::CompositeCandidate>(name, tree)
{
  daughterBranches1 = std::unique_ptr<BranchManager<T1> >(new BranchManager<T1>(this->getDaughterName(1), tree));
  daughterBranches2 = std::unique_ptr<BranchManager<T2> >(new BranchManager<T2>(this->getDaughterName(2), tree));
}


template<>
template<class T1, class T2> void
BranchManager<CompositeDaughter<T1, T2> >::fill(const reco::Candidate* const abstractObject)
{
  edm::Ptr<pat::CompositeCandidate> obj = extractMasterPtr(abstractObject);

  fill(obj);
}


template<>
template<class T1, class T2> void
BranchManager<CompositeDaughter<T1, T2> >::fill(const edm::Ptr<pat::CompositeCandidate>& obj)
{
  if(obj.isNull() || obj->numberOfDaughters() != 2)
    throw cms::Exception("InvalidObject")
      << "Invalid " << this->getName() 
      <<" CompositeCandidate object passed to Ntuplizer";

  BranchManager<pat::CompositeCandidate>::fill(obj);

  daughterBranches1->fill(obj->daughter(0));
  daughterBranches2->fill(obj->daughter(1));
}


template<> std::string
BranchManager<CompositeDaughter<pat::Muon, pat::Muon> >::getDaughterName(size_t which) const
{
  if(which == 1)
    return "m1";
  if(which == 2)
    return "m2";

  return "Uh oh";
}

template<> std::string
BranchManager<CompositeDaughter<pat::Electron, pat::Electron> >::getDaughterName(size_t which) const
{
  if(which == 1)
    return "e1";
  if(which == 2)
    return "e2";

  return "Uh oh";
}

template<> std::string
BranchManager<CompositeDaughter<CompositeDaughter<pat::Electron,pat::Electron>, 
                                CompositeDaughter<pat::Muon,pat::Muon> 
                                > 
              >::getDaughterName(size_t which) const
{
  if(which == 1)
    return "e1_e2_";
  if(which ==2)
    return "m1_m2_";

  return "Uh oh";
}


#endif // header guard
