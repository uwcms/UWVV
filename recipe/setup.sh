#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./RecoEgamma ]; then
    echo "Setting up electron ID code"
    git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma
fi

if [ ! -d ZZMatrixElement ]; then
    echo "Setting up ZZ matrix element stuff"
    git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

    pushd ZZMatrixElement
    git checkout -b from-v200p3 v2.0.0_patch3
    source setup.sh -j 12
    popd
fi

if [ ! -d ./KinZfitter ]; then
    echo "Setting up Z kinematic fit stuff"
    git clone -b segfaultFix https://github.com/nwoods/KinZfitter.git
fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V3 KaMuCa
fi

popd
