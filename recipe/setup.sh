#!/usr/bin/bash


if [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
    echo "$0 usage: ./$0 [-h|--help] [--hzzExtras] [-j NTHREADS]"
    echo "    --hzzExtras: Get and compile HZZ matrix element and kinematic fit stuff, and generate the UWVV plugins that use them."
    echo "               This is not the default because most people do not need them and one of the packages' authors frequently make changes that break everything without intervention on our side."
    echo "               NB if you use this option and later use scram b clean, you should rerun this script with this option or your CONDOR jobs may fail."
    echo "    -j NTHREADS: [with --hzzExtras] Compile ZZMatrixElement package with NTHREADS threads (default 12)."
    exit 1
fi

while [ "$1" ]
do
    case "$1" in
        --hzzExtras)
            HZZ=1
            ;;
        -j)
            shift
            UWVVNTHREADS="$1"
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac

    shift
done

if [ ! "$UWVVNTHREADS" ]; then
    UWVVNTHREADS=12
fi

pushd $CMSSW_BASE/src

if [ ! -d ./EgammaAnalysis ]; then
    echo "Setting up electron energy scale corrections"
    git cms-merge-topic rafaellopesdesa:EgammaAnalysis80_EGMSmearer_Moriond17_23Jan

    pushd EgammaAnalysis/ElectronTools/data
    git clone https://github.com/ECALELFS/ScalesSmearings.git
    popd
fi

if [ "$HZZ" ]; then

    if [ ! -d ./ZZMatrixElement ]; then
        echo "\nSetting up ZZ matrix element stuff"
        git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

        pushd ZZMatrixElement
        git checkout -b from-v200p5 v2.0.0_patch5
        source setup.sh -j "$UWVVNTHREADS"
        popd
    fi

    # copy libraries dowloaded by MELA to lib so they get packaged and used by CONDOR
    cp ZZMatrixElement/MELA/data/"$SCRAM_ARCH"/*.so "$CMSSW_BASE"/lib/"$SCRAM_ARCH"

    if [ ! -d ./KinZfitter ]; then
        echo "\nSetting up Z kinematic fit stuff"
        git clone https://github.com/VBF-HZZ/KinZfitter.git
    fi

    # generate UWVV's MELA plugin
    cp UWVV/AnalysisTools/plugins/ZZDiscriminantEmbedderCode.txt UWVV/AnalysisTools/plugins/ZZDiscriminantEmbedder.cc
    cp UWVV/AnalysisTools/plugins/ZKinematicFitEmbedderCode.txt UWVV/AnalysisTools/plugins/ZKinematicFitEmbedder.cc

fi

if [ ! -d ./KaMuCa ]; then
    echo "Setting up muon calibration"
    git clone https://github.com/bachtis/analysis.git -b KaMuCa_V4 KaMuCa
fi

popd
