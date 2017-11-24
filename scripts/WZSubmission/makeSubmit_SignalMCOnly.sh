#!/bin/bash
DATE=`date +%Y-%m-%d`
if [ -z "$CMSSW_BASE" ]; then
    echo "Must submit within a CMS environment"
    exit 1
fi
uwvv=$CMSSW_BASE/src/UWVV
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarlo_$DATE.sh \
    --campaign "RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1" \
    --samples \
        "WZTo3LNu_3Jets_MLL-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" \
   --filesPerJob 1 \
    UWVVNtuples_WZSignalMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        lheWeights=3 \
        globalTag=80X_mcRun2_asymptotic_2016_TrancheIV_v8 \
chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarlo_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZSignalMonteCarlo_$DATE.sh
