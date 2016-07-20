# UWVV
Some tools for CMS analyses

## Setup
Current supported CMSSW release: `CMSSW_8_0_12` 

```bash
scram pro -n uwvv CMSSW CMSSW_8_0_[current version]
cd uwvv/src
cmsenv
git cms-init # do before anything else
git clone https://github.com/nwoods/UWVV.git # or git clone git@github.com:nwoods/UWVV.git if you prefer that
cd UWVV
source recipe/setup.sh # install necessary packages
pushd ..
scram b -j 8 # compile
popd
```

To set up a python virtual environment with Rootpy, a non-bugged, IPython, and some other nice things

```bash
source recipe/setupPython.sh
```

The first time you call `setupPython.sh`, it creates the virtualenv, installs the packages in it, and activates it. 
After that, it just activates it.
