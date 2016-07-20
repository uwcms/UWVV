#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./RecoEgamma ]; then
    echo "Setting up electron ID code"
    git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma
fi

if [ ! -d ./EgammaAnalysis ]; then
    echo "Setting up electron energy scale corrections"
    git remote add -f -t ecal_smear_fix_80X emanueledimarco https://github.com/emanueledimarco/cmssw.git
    git cms-addpkg EgammaAnalysis/ElectronTools
    git checkout -b from-277de3c 277de3c

    pushd EgammaAnalysis/ElectronTools/data
    git clone -b ICHEP2016_approval_7p65fb https://github.com/emanueledimarco/ScalesSmearings.git
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
    git clone https://github.com/VBF-HZZ/KinZfitter.git
fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V4 KaMuCa
fi

popd
