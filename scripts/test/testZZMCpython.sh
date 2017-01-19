#!/bin/bash
python Ntuplizer/test/ntuplize_cfg.py \
    inputFiles=/store/mc/RunIISummer16MiniAODv2/ZZTo4L_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/221CC46F-2FC6-E611-8FFC-0CC47A1E0488.root \
    outputFile=testMC.root \
    channels=wz \
    isMC=1 \
    eCalib=1 \
    muCalib=1 \
    lheWeights=3 \
    globalTag=80X_mcRun2_asymptotic_2016_TrancheIV_v7 \
    $1
