# UWVV
Some tools for CMS analyses

UWVV is designed for analyses that use final state particles (typically leptons) to reconstruct intermediate and initial states. For example, in the H->ZZ->4l analysis, electron and muon pairs are built into Z candidates, and the Z candidates are built into Higgs candidates. It contains tools for building a full analysis flow out of CMS EDM modules, and for making flat ntuples where each row represents one initial state candidate.

It uses the [CMSSW framework](https://github.com/cms-sw/cmssw) and expects [miniAOD](https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016) input. Much of the inspiration (and a little bit of the code) comes from [FSA](https://github.com/uwcms/FinalStateAnalysis/). A few tools, like the batch submission scripts, are specific to the computing infrastructure at the University of Wisconsin - Madison.

## Setup
Current supported CMSSW release: `CMSSW_8_0_26_patch2+`

```bash
scram pro -n uwvv CMSSW CMSSW_8_0_[current version]
cd uwvv/src
cmsenv
git cms-init # do before anything else
git clone --recursive https://github.com/nwoods/UWVV.git # or git@github.com:nwoods/UWVV.git if you prefer that
cd UWVV
source recipe/setup.sh # install necessary packages
pushd ..
scram b -j 8 # compile
popd
```

To set up a python virtual environment with Rootpy, a non-bugged IPython, and some other nice things

```bash
source recipe/setupPython.sh
```

The first time you call `setupPython.sh`, it creates the virtualenv, installs the packages in it, and activates it.
After that, it just activates it.


## Use
To make a basic ntuple of four-lepton final state candidates, do

```bash
cmsRun Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 inputFiles=file:aNiceMonteCarloFile.root
```

For more on how to build your own analysis, see the `AnalysisTools` directory. For more on making ntuples, see the `Ntuplizer` directory.