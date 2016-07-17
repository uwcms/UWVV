#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./RecoEgamma ]; then
    echo "Setting up electron ID code"
    git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma
fi

if [ ! -d ./EgammaAnalysis ]; then
    echo "Setting up electron energy scale corrections"
    git remote add -f -t smearings80X shervin86 https://github.com/shervin86/cmssw.git
    git cherry-pick f3b0b0140483c336212baa035cf9a820a016a799
    git cherry-pick a5aaeb7a598800ae98e88ea1a952ecd1d66aa059
    git cherry-pick c7ac16dd88969510d2d6d6ea2c4702e0108bf151
    git cherry-pick 054a90830c77423ee673204611522018ace69c5d
    git cms-addpkg EgammaAnalysis/ElectronTools
    pushd EgammaAnalysis/ElectronTools/data
    git clone -b ICHEP2016_approval_4fb https://github.com/ECALELFS/ScalesSmearings.git
    popd
fi

if [ ! -d ./ZZMatrixElement ]; then
    echo "Setting up ZZ matrix element stuff"
    git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

    pushd ZZMatrixElement
    git checkout -b from-v200p5 v2.0.0_patch5
    source setup.sh -j 12
    popd
fi

if [ ! -d ./KinZfitter ]; then
    echo "Setting up Z kinematic fit stuff"
    git clone -b segfaultFix https://github.com/nwoods/KinZfitter.git
fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V4 KaMuCa
fi

popd
