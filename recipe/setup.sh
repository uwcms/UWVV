#!/usr/bin/bash


pushd $CMSSW_BASE/src

# electron calibrations
git cms-merge-topic -u matteosan1:smearer_76X

# muon calibrations
git clone https://github.com/bachtis/Analysis.git -b KaMuCa_V2 KaMuCa 

popd