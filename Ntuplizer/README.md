# UWVV ntuples

Each row in a UWVV ntuple represents one initial state candidate (not one event). The columns are information about the candidate, its daughters (and their daughters, recursively), and the event they live in. The format is essentially the same as in [FSA ntuples](https://github.com/uwcms/InitialStateAnalysis/tree/miniAOD_dev_80X/NtupleTools/test). 


## Tree generation

Trees are channel-specific. By default, a tree is in the output file as `[channel]/ntuple`, e.g. `eemm/ntuple` for a 2 electron, 2 muon final state. Trees are made by an EDAnalyzer called something like `TreeGeneratorEEMuMu` (or similar, depending on the channel). 

This snippet makes a 2e2mu tree
```python
import FWCore.ParameterSet.Config as cms

from UWVV.Ntuplizer.eventParams import makeEventParams

###
### Set up flow (see AnalysisTools), ntuple parameters (see below), process etc.
###

process.eemmTree = cms.EDAnalyzer(
    'TreeGeneratorEEMuMu',
    src = flow.finalObjTag('eemm'),
    branches = eemmBranchPSet, # see below
    eventParams = makeEventParams(flow.finalTags()), # for setting up whole-event information
    triggers = triggerPSet, # see below
    )
```


### Defining a single branch

Each branch is associated with an object (which may be a composite candidate or a final state object candidate) and defined by a cms.string. Quantities which can be calculated directly from the candidate object use CMS StringObjectFunctions constructed from the cms.string. For example, one can make a branch called `[object]GenPt` containing the pt of the gen particle associated to a PAT lepton (or -999 if there isn't one) like this
```python
GenPt = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.pt')
```
CMS StringObjectFunctions always return doubles, which are automatically converted to the correct type for the branch.

For quantities that require a more involved calculation or other information about the event, functions are defined in `Ntuplizer/interface/FunctionLibrary.h`. These functions are stored in maps specific to the object type and branch type. These functions take as arguments an `edm::Ptr` to the object and a reference to a `uwvv::EventInfo` object, which has access to a number of useful collections and quantities in the event. If the string defining the branch is a key in the approriate map, the library function is used. Otherwise, it is interpreted as a CMS StringObjectFunction.


### Specifying all branches

The definitions of the branches are passed to the TreeGenerator as a cms.PSet called `branches`. It should contain the following
* One PSet for each type of branch related to the event or the initial state. Each of these PSets should contain cms.strings defining the branch content (see above). Currently supported branches are 
  * `floats`
  * `ints`
  * `uints` (unsigned ints)
  * `bools`
  * `ulls` (unsigned long long ints)
* `daughterNames`, a cms.vstring with the names of the first daughters, if any
* `daughterParams`, a cms.VPSet containing exactly one cms.PSet for each daughter, containing the exact same items as in this list, to define the branches related to the daughters

A very simple tree, containing just the pt of a Z and its daughter electrons, would be defined like this
```python
branches = cms.PSet(
    floats = cms.PSet(
        Pt = cms.string('pt'),
        ),
    daughterNames = cms.vstring('e1', 'e2'),
    daughterParams = cms.VPSet(
        cms.PSet(
            floats = cms.PSet(
                Pt = cms.string('pt'),
                ),
            ),
        cms.PSet(
            floats = cms.PSet(
                Pt = cms.string('pt'),
                ),
            ),
        ),
    )
```

Some useful functions for copying and combining cms.PSets can be found in `Utilities/python/helpers.py`.

### Trigger branches

Trigger branches are defined separately, by the `triggers` parameter, which should be a cms.PSet containing
* `trigNames` (cms.vstring), a list of names for all the sets of triggers desired. Trigger results will appear in the ntuple as `[name]Pass`
* One cms.vstring for each entry in `trigNames`, called `[name]Paths`, giving the names of one or more paths to check
  * The decision stored in the ntuple is a logical OR of all paths in the cms.vstring
  * Regular expressions are allowed, which is most useful for the version number for each path. The string `'HLT_Mu50_v\\[0-9]+'` will match any version of the 50 GeV single muon path.
* `trigResultSrc` (cms.InputTag), the input tag for the trigger decisions
* `trigPrescaleSrc` (cms.InputTag), the input tag for the trigger prescales
* `checkPrescale` (cms.bool, optional) indicates whether the prescales should be stored in the ntuple (in branches called `[name]Prescale`). 
  * If `checkPrescale` is True (the default), all paths for a given branch must have the same prescale or the TreeGenerator will throw an exception

The following will put dimuon and dielectron trigger decisions into the ntuple with no prescales
```python
triggerBranches = cms.PSet(
    trigNames = cms.vstring('doubleMu', 'doubleE',),
    doubleMuPaths = cms.vstring('HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v\\[0-9]+',
                                'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v\\[0-9]+'),
    doubleEPaths = cms.vstring('HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v\\[0-9]+',
                               'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v\\[0-9]+',
                               'HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v\\[0-9]+'),

    trigResultsSrc = cms.InputTag("TriggerResults", "", "HLT"),
    trigPrescaleSrc = cms.InputTag("patTrigger"),

    checkPrescale = cms.bool(False),
    )
```

### Branch naming convention

Branches with information about the initial state or the event are simply named after the quantity they hold, e.g. `Mass` for the reconstructed invariant mass of the initial state or `nvtx` for the number of reconstructed vertices in the event. 

Branches with information about intermediate state particles or final state daughters are called `[object][Quantity]`, e.g. `e1Pt` for the pt of the first electron. Naming intermediate states `[daughter1]_[daughter2]_`, so that their branches are named things like `e1_e2_Mass` (for the mass of an intermediate Z->ee candidate), is recommended but not required. 


## Metadata

Useful meta info about the ntuples in the file, like which runs and lumi sections were processed and the sum of the generator weights of the sample, can be produced alongside the analysis ntuples. 

```python
process.metaInfo = cms.EDAnalyzer(
    'MetaTreeGenerator',
    eventParams = makeEventParams(flow.finalTags()),
    )
```

The tree will be stored in the file as `metaInfo/metaInfo`. Each row is one luminosity block.
