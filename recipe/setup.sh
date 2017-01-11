#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./RecoEgamma ]; then
    echo "Setting up electron ID code"
    git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma
fi

if [ ! -d ./EgammaAnalysis ]; then
    echo "Setting up electron energy scale corrections"
    git cms-merge-topic shervin86:Moriond2017_JEC_energyScales

    pushd EgammaAnalysis/ElectronTools/data
    git clone https://github.com/ECALELFS/ScalesSmearings.git
    popd
fi

if [ ! -d ./ZZMatrixElement ]; then
    echo "Setting up ZZ matrix element stuff"
    git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

    pushd ZZMatrixElement
    git checkout -b from-v200p5 v2.0.0_patch5
    source setup.sh -j 12
    popd

    # copy libraries dowloaded by MELA to lib so they get packaged and used by CONDOR
    cp ZZMatrixElement/MELA/data/"$SCRAM_ARCH"/*.so "$CMSSW_BASE"/lib/"$SCRAM_ARCH"
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
