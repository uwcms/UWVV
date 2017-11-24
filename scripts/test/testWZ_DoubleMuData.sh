#!/bin/bash
cmsRun UWVV/Ntuplizer/test/ntuplize_cfg.py \
    inputFiles=/store/data/Run2016C/DoubleMuon/MINIAOD/03Feb2017-v1/110000/02292378-59ED-E611-BA0B-0CC47A4D768E.root \
    datasetName=/DoubleMuon/Run2016C-03Feb2017-v1/MINIAOD \
    outputFile=test_DoubleMu.root \
    channels=wz \
    isMC=0 \
    eCalib=1 \
    muCalib=1 \
    lheWeights=0 \
    globalTag=80X_dataRun2_2016SeptRepro_v6 \
    $1
