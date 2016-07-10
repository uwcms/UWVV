#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./RecoEgamma ]; then
    echo "Setting up electron ID code"
    git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma
fi

if [ ! -d ZZMatrixElement ]; then
    echo "Setting up ZZ matrix element stuff"
    git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

    # Get a version we know works during the awkward MELA v1->v2 transition
    pushd ZZMatrixElement
    git checkout c6d45de674ee145a8a4d74ba99f3e446dd1d9987
    popd
fi

if [ ! -d ./KinZfitter ]; then
    echo "Setting up Z kinematic fit stuff"
    git clone -b segfaultFix git@github.com:nwoods/KinZfitter.git
fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V3 KaMuCa
fi

popd
