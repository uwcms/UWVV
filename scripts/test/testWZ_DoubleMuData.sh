#!/bin/bash
cmsRun Ntuplizer/test/ntuplize_cfg.py \
    inputFiles=/store/data/Run2016B/DoubleMuon/MINIAOD/23Sep2016-v3/00000/00269AA1-479B-E611-A359-0025905A6082.root \
    outputFile=testMC.root \
    channels=wz \
    isMC=0 \
    eCalib=1 \
    muCalib=1 \
    lheWeights=0 \
    globalTag=80X_dataRun2_2016SeptRepro_v6 \
    $1
