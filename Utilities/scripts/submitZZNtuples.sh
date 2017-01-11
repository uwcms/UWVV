#!/bin/bash

### Little script to submit all necessary ntuplization jobs for the 2016 ZZ analysis
###     Specify an identifier for the jobs (e.g. 18JAN2016_0) and, optionally,
###     which types of ntuples to submit. Options are --data, --mc, --zz, --zl, and --z,
###     or a combination, for running data, monte carlo, 4l final states, 3l final states,
###     and 2l final states; e.g., --data --zz --zl would run only 4l and 3l final states
###     for data, no 2l final states and no monte carlo. If none of those are specified,
###     all are run


if [ "$1" == "" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
    echo "$0 usage: ./$0 [-h|--help] id [--data] [--mc] [--zz] [--zl] [--z]"
    echo "    id: string identifying this set of ntuples, e.g. a date."
    echo "    --[data/mc]: run only on data/mc."
    echo "    --[zz/zl/z]: run only 4l/3l/2l ntuples. More than one may be specified."
    echo "    --syst: (with --zz and --mc) run signal MC several more times with various systematic shifts."
    return 1
fi

ID="$1"
shift

while [ "$1" ]
do
    case "$1" in
        --data)
            DATA=1
            ;;
        --mc)
            MC=1
            ;;
        --zz)
            ZZ=1
            ;;
        --zl)
            ZL=1
            ;;
        --z)
            Z=1
            ;;
        --syst)
            SYST=1
            ;;
        *)
            echo "Unknown option: $1"
            return 1
            ;;
    esac

    shift
done

if [ "$DATA" == '' ] && [ "$MC" == '' ]
then
    DATA=1
    MC=1
fi

if [ "$ZZ" == '' ] && [ "$ZL" == '' ] && [ "$Z" == '' ]
then
    ZZ=1
    ZL=1
    Z=1
fi

if [ ! -d "$CMSSW_BASE"/src/UWVV ]
then
    echo "You can only submit ntuples from a CMSSW environment with UWVV set up!"
    return 1
fi

pushd "$CMSSW_BASE"/src/UWVV

# Check for a valid voms proxy (not a perfect check; just makes sure there's
# regular output from voms-proxy-info but no error output.
VOMS_OUT=$( voms-proxy-info 2> /dev/null )
VOMS_ERR=$( voms-proxy-info 2>&1 > /dev/null )
if [ ! "$VOMS_OUT" ] || [ "$VOMS_ERR" ]
then
    echo 'Something is wrong with your VOMS proxy -- please renew it or check it'
    return 1
fi

# global tags
GT_MC=80X_mcRun2_asymptotic_2016_miniAODv2_v1
GT_DATA=80X_dataRun2_Prompt_ICHEP16JEC_v0


if [ "$DATA" ]
then

    for era in B C D E
    do

        if [ "$ZZ" ]
        then
            echo Submitting ZZ Data Ntuples as UWVVZZ_DATA_2016"$era"_"$ID"

            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples MuonEG SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvZZ_data_2016"$era"_"$ID".sh UWVVZZ_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=0 eCalib=1 muCalib=1 globalTag="$GT_DATA"

            nohup bash /data/nawoods/uwvvZZ_data_2016"$era"_"$ID".sh &
        fi

        if [ "$ZL" ]
        then
            echo Submitting Z+L Data Ntuples as UWVVZPLUSL_DATA_2016"$era"_"$ID"

            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples MuonEG SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvZPlusl_data_2016"$era"_"$ID".sh UWVVZPLUSL_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=0 eCalib=1 muCalib=1 globalTag="$GT_DATA"

            nohup bash /data/nawoods/uwvvZPlusl_data_2016"$era"_"$ID".sh &
        fi

        if [ "$Z" ]
        then
            echo Submitting Single Z Data Ntuples as UWVVSINGLEZ_DATA_2016"$era"_"$ID"

            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvSingleZ_data_2016"$era"_"$ID".sh UWVVSINGLEZ_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='z' isMC=0 eCalib=1 muCalib=1 globalTag="$GT_DATA"

            nohup bash /data/nawoods/uwvvSingleZ_data_2016"$era"_"$ID".sh &
        fi
    done
fi

if [ "$MC" ]
then

    if [ "$ZZ" ]
    then
        echo Submitting ZZ MC Ntuples as UWVVZZ_MC_"$ID"

        # reHLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS lheWeights=2

        nohup bash /data/nawoods/uwvvZZ_mc_"$ID".sh &

        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoPDF_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS

        nohup bash /data/nawoods/uwvvZZ_mcNoPDF_"$ID".sh &

        # no HLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'ZZTo4L*powheg*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS lheWeights=2

        nohup bash /data/nawoods/uwvvZZ_mcNoHLT_"$ID".sh &

        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8' 'WWZ*' 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'GluGluToContinToZZTo2e2tau*' 'GluGluToContinToZZTo2mu2tau*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLTNoLHE_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS lheWeights=0

        nohup bash /data/nawoods/uwvvZZ_mcNoHLTNoLHE_"$ID".sh &

        # alternative production
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS lheWeights=2

        nohup bash /data/nawoods/uwvvZZ_mcSpecial_"$ID".sh &

        # more alternative production
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag="$GT_MC" genLeptonType=dressedHPFS

        nohup bash /data/nawoods/uwvvZZ_mcSpecial2_"$ID".sh &

        # with systematic shifts
        if [ "$SYST" ]
        then
            echo Submitting ZZ MC Ntuples with Systematic Shifts

            # EES+
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_eScaleUp_"$ID".sh UWVVZZ_MC_ESCALEUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_eScaleUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_eScaleUp_"$ID".sh UWVVZZ_MC_ESCALEUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_eScaleUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_eScaleUp_"$ID".sh UWVVZZ_MC_ESCALEUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_eScaleUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_eScaleUp_"$ID".sh UWVVZZ_MC_ESCALEUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_eScaleUp_"$ID".sh &

            # EES-
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_eScaleDn_"$ID".sh UWVVZZ_MC_ESCALEDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_eScaleDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_eScaleDn_"$ID".sh UWVVZZ_MC_ESCALEDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_eScaleDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_eScaleDn_"$ID".sh UWVVZZ_MC_ESCALEDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_eScaleDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_eScaleDn_"$ID".sh UWVVZZ_MC_ESCALEDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 eScaleShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_eScaleDn_"$ID".sh &

            # EER+ (rho)
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_eRhoResUp_"$ID".sh UWVVZZ_MC_ERHORESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_eRhoResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_eRhoResUp_"$ID".sh UWVVZZ_MC_ERHORESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_eRhoResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_eRhoResUp_"$ID".sh UWVVZZ_MC_ERHORESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_eRhoResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_eRhoResUp_"$ID".sh UWVVZZ_MC_ERHORESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_eRhoResUp_"$ID".sh &

            # EER- (rho)
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_eRhoResDn_"$ID".sh UWVVZZ_MC_ERHORESDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_eRhoResDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_eRhoResDn_"$ID".sh UWVVZZ_MC_ERHORESDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_eRhoResDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_eRhoResDn_"$ID".sh UWVVZZ_MC_ERHORESDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_eRhoResDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_eRhoResDn_"$ID".sh UWVVZZ_MC_ERHORESDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 eRhoResShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_eRhoResDn_"$ID".sh &

            # EER+ (phi)
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_ePhiResUp_"$ID".sh UWVVZZ_MC_EPHIRESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 ePhiResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_ePhiResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_ePhiResUp_"$ID".sh UWVVZZ_MC_EPHIRESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 ePhiResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_ePhiResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_ePhiResUp_"$ID".sh UWVVZZ_MC_EPHIRESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eeee,eemm' isMC=1 genInfo=1 eCalib=1 muCalib=1 ePhiResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_ePhiResUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_ePhiResUp_"$ID".sh UWVVZZ_MC_EPHIRESUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 ePhiResShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_ePhiResUp_"$ID".sh &

            # MES/MER+
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_mClosureUp_"$ID".sh UWVVZZ_MC_MCLOSUREUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_mClosureUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_mClosureUp_"$ID".sh UWVVZZ_MC_MCLOSUREUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_mClosureUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_mClosureUp_"$ID".sh UWVVZZ_MC_MCLOSUREUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_mClosureUp_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_mClosureUp_"$ID".sh UWVVZZ_MC_MCLOSUREUP_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_mClosureUp_"$ID".sh &

            # MES/MER-
            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZTo4L*amcatnlo*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_mClosureDn_"$ID".sh UWVVZZ_MC_MCLOSUREDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mc_mClosureDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'ZZTo4L*powheg*' 'WWZ*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_mClosureDn_"$ID".sh UWVVZZ_MC_MCLOSUREDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcNoHLT_mClosureDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'ZZJJTo4L_EWK_13TeV-madgraph-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial_mClosureDn_"$ID".sh UWVVZZ_MC_MCLOSUREDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='eemm,mmmm' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial_mClosureDn_"$ID".sh &

            python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1 --samples 'TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcSpecial2_mClosureDn_"$ID".sh UWVVZZ_MC_MCLOSUREDN_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 genInfo=1 eCalib=1 muCalib=1 mClosureShift=-1 globalTag="$GT_MC" lheWeights=0

            nohup bash /data/nawoods/uwvvZZ_mcSpecial2_mClosureDn_"$ID".sh &
        fi
    fi

    if [ "$ZL" ]
    then
        echo Submitting Z+L Data Ntuples as UWVVZPLUSL_MC_"$ID"

        # reHLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' --filesPerJob 2 -o /data/nawoods/uwvvZPlusL_mc_"$ID".sh UWVVZPLUSL_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=1 eCalib=1 muCalib=1 globalTag="$GT_MC" lheWeights=0

        nohup bash /data/nawoods/uwvvZPlusl_mc_"$ID".sh &

        # no HLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8' 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'GluGluToContinToZZTo2e2tau*' 'GluGluToContinToZZTo2mu2tau*' 'ZZTo4L*powheg*' --filesPerJob 2 -o /data/nawoods/uwvvZPlusl_mcNoHLT_"$ID".sh UWVVZPLUSL_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=1 eCalib=1 muCalib=1 globalTag="$GT_MC" lheWeights=0

        nohup bash /data/nawoods/uwvvZPlusl_mcNoHLT_"$ID".sh &
    fi

    if [ "$Z" ]
    then
        echo Submitting Single Z Data Ntuples as UWVVSINGLEZ_MC_"$ID"

        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' --filesPerJob 2 -o /data/nawoods/uwvvSingleZ_mc_"$ID".sh UWVVSINGLEZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='z' isMC=1 eCalib=1 muCalib=1 globalTag="$GT_MC" lheWeights=0

        nohup bash /data/nawoods/uwvvSingleZ_mc_"$ID".sh &
    fi
fi

popd