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

if [ "$DATA" ]
then

    for era in B C D E
    do

        if [ "$ZZ" ]
        then
            echo Submitting ZZ Data Ntuples as UWVVZZ_DATA_2016"$era"_"$ID"
        
            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples MuonEG SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvZZ_data_2016"$era"_"$ID".sh UWVVZZ_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=0 eCalib=1 muCalib=1 globalTag=80X_dataRun2_Prompt_ICHEP16JEC_v0
        
            nohup bash /data/nawoods/uwvvZZ_data_2016"$era"_"$ID".sh &
        fi
        
        if [ "$ZL" ]
        then
            echo Submitting Z+L Data Ntuples as UWVVZPLUSL_DATA_2016"$era"_"$ID"
        
            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples MuonEG SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvZPlusl_data_2016"$era"_"$ID".sh UWVVZPLUSL_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=0 eCalib=1 muCalib=1 globalTag=80X_dataRun2_Prompt_ICHEP16JEC_v0

            nohup bash /data/nawoods/uwvvZPlusl_data_2016"$era"_"$ID".sh &
        fi
        
        if [ "$Z" ]
        then
            echo Submitting Single Z Data Ntuples as UWVVSINGLEZ_DATA_2016"$era"_"$ID"
        
            python Utilities/scripts/submitJobs.py --applyLumiMask --dataEra Run2016"$era"-PromptReco-v2 --samples SingleElectron SingleMuon DoubleMuon DoubleEG --filesPerJob 3 -o /data/nawoods/uwvvSingleZ_data_2016"$era"_"$ID".sh UWVVSINGLEZ_DATA_2016"$era"_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='z' isMC=0 eCalib=1 muCalib=1 globalTag=80X_dataRun2_Prompt_ICHEP16JEC_v0
        
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
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'Graviton2PBToZZTo4L*' 'ZZTo4L*amcatnlo*'  'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' 'GluGluHToZZTo4L_M125*pythia8' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mc_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

        nohup bash /data/nawoods/uwvvZZ_mc_"$ID".sh &

        # no HLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8' 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'GluGluToContinToZZTo2e2tau*' 'GluGluToContinToZZTo2mu2tau*' 'ZZTo4L*powheg*' --filesPerJob 2 -o /data/nawoods/uwvvZZ_mcNoHLT_"$ID".sh UWVVZZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zz' isMC=1 eCalib=1 muCalib=1 genInfo=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

        nohup bash /data/nawoods/uwvvZZ_mcNoHLT_"$ID".sh &
    fi

    if [ "$ZL" ]
    then
        echo Submitting Z+L Data Ntuples as UWVVZPLUSL_MC_"$ID"

        # reHLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' --filesPerJob 2 -o /data/nawoods/uwvvZPlusL_mc_"$ID".sh UWVVZPLUSL_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=1 eCalib=1 muCalib=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

        nohup bash /data/nawoods/uwvvZPlusl_mc_"$ID".sh &

        # no HLT
        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1 --samples 'WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8' 'GluGluToContinToZZTo4*' 'GluGluToContinToZZTo2e2mu*' 'GluGluToContinToZZTo2e2tau*' 'GluGluToContinToZZTo2mu2tau*' 'ZZTo4L*powheg*' --filesPerJob 2 -o /data/nawoods/uwvvZPlusl_mcNoHLT_"$ID".sh UWVVZPLUSL_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='zl' isMC=1 eCalib=1 muCalib=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

        nohup bash /data/nawoods/uwvvZPlusl_mcNoHLT_"$ID".sh &
    fi

    if [ "$Z" ]
    then
        echo Submitting Single Z Data Ntuples as UWVVSINGLEZ_MC_"$ID"

        python Utilities/scripts/submitJobs.py --campaign RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1 --samples 'DYJetsToLL_M-50*amcatnlo*' 'TTJets*' --filesPerJob 2 -o /data/nawoods/uwvvSingleZ_mc_"$ID".sh UWVVSINGLEZ_MC_"$ID" Ntuplizer/test/ntuplize_cfg.py channels='z' isMC=1 eCalib=1 muCalib=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

        nohup bash /data/nawoods/uwvvSingleZ_mc_"$ID".sh &
    fi
fi

popd