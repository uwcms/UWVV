#!/usr/bin/bash


pushd $CMSSW_BASE/src

if [ ! -d ./EgammaAnalysis ]; then
    echo "Setting up electron energy scale corrections"
    git cms-merge-topic rafaellopesdesa:EgammaAnalysis80_EGMSmearer_Moriond17_23Jan

    pushd EgammaAnalysis/ElectronTools/data
    git clone https://github.com/ECALELFS/ScalesSmearings.git
    popd
fi

if [ ! -d ./ZZMatrixElement ]; then
    echo "\nSetting up ZZ matrix element stuff"
    git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

    pushd ZZMatrixElement
    git checkout -b from-v200p5 v2.0.0_patch5
    source setup.sh -j 12
    popd
fi

if [ ! -d ./RecoMET ]; then
    echo "\nChecking out MET recipe for Moriond 17"
    git cms-merge-topic -u cms-met:METRecipe_8020
fi

# copy libraries dowloaded by MELA to lib so they get packaged and used by CONDOR
cp ZZMatrixElement/MELA/data/"$SCRAM_ARCH"/*.so "$CMSSW_BASE"/lib/"$SCRAM_ARCH"

if [ ! -d ./KinZfitter ]; then
    echo "\nSetting up Z kinematic fit stuff"
    git clone https://github.com/VBF-HZZ/KinZfitter.git
fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V4 KaMuCa
fi

popd
