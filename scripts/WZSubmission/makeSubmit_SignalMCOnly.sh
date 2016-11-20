#!/bin/bash
DATE=`date +%Y-%m-%d`
if [ -z "$CMSSW_BASE" ]; then
    echo "Must submit within a CMS environment"
    exit 1
fi
uwvv=$CMSSW_BASE/src/UWVV
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloNoHLT_$DATE.sh \
    --campaign "RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2*" \
    --samples \
        "WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8" \
        "WZTo3LNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" \
   --filesPerJob 3 \
    UWVVNtuples_WZSignalMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1
# Tranche 3 withHLT samples
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloWithHLT_$DATE.sh \
    --campaign "RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1" \
    --samples \
        "WLLJJToLNu_M-60_EWK_13TeV-madgraph-pythia8" \
    --filesPerJob 3 \
    UWVVNtuples_WZSignalMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloNoHLT_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloNoHLT_$DATE.sh
chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloWithHLT_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarloWithHLT_$DATE.sh
