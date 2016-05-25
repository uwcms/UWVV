#include "UWVV/Ntuplizer/interface/TriggerBranches.h"



using namespace uwvv;

// Single branch implementation

TriggerBranch::TriggerBranch(const std::string& name, 
                             const std::vector<std::string>& pathExps,
                             TTree* const tree) :
  name(name)
{
  if(!pathExps.size())
    throw cms::Exception("BadTriggerPath")
      << "Trigger branch must contain at least one path." << std::endl;
  
  for(auto& expr : pathExps)
    paths.push_back(expr);
  
  tree->Branch((name+"Pass").c_str(), &pass);
  tree->Branch((name+"Prescale").c_str(), &prescale);
}


void TriggerBranch::setup(const edm::TriggerNames& names)
{
  for(auto& path : paths)
    path.setup(names);
}


void TriggerBranch::fill(const edm::TriggerResults& results,
                         const pat::PackedTriggerPrescales& prescales)
{
  // don't need to check for validity (paths do that)

  prescale = paths.at(0).prescale(prescales);
  pass = false;
  for(auto& path : paths)
    {
      if(path.prescale(prescales) != prescale)
        throw cms::Exception("InvalidPrescale")
          << "ERROR: trying to take OR of paths with different prescales "
          << "(" << paths.at(0).name() << " and " << path.name() << ")"
          << std::endl;

      pass |= path.pass(results);
    }
}


// Branch collection implementation

TriggerBranches::TriggerBranches(edm::ConsumesCollector cc, 
                                 const edm::ParameterSet& config,
                                 TTree* const tree) :
  resultsToken(cc.consumes<edm::TriggerResults>(config.getParameter<edm::InputTag>("trigResultsSrc"))),
  prescalesToken(cc.consumes<pat::PackedTriggerPrescales>(config.getParameter<edm::InputTag>("trigPrescaleSrc"))),
  isValid(false)
{
  std::vector<std::string> names = 
    config.getParameter<std::vector<std::string> >("trigNames");
  
  for(auto& name : names)
    {
      std::vector<std::string> paths = 
        config.getParameter<std::vector<std::string> >(name + "Paths");
      
      branches[name] = std::unique_ptr<TriggerBranch>(new TriggerBranch(name, paths, tree));
    }
}


void TriggerBranches::setEvent(const edm::Event& event)
{
  event.getByToken(resultsToken, results);
  event.getByToken(prescalesToken, prescales);

  const edm::TriggerNames& names = event.triggerNames(*results);

  if((!isValid) || id != names.parameterSetID())
    {
      id = names.parameterSetID();

      for(auto& b : branches)
        b.second->setup(names);

      isValid = true;
    }
}


void TriggerBranches::fill()
{
  if(!isValid)
    throw cms::Exception("UninitializedTriggerBranches")
      << "ERROR: attempt to use uninitialized TriggerBranches object."
      << std::endl;

  for(auto& b : branches)
    b.second->fill(*results, *prescales);
}
