#!/bin/bash
cmsRun Ntuplizer/test/ntuplize_cfg.py inputFiles=/store/data/Run2016B/DoubleMuon/MINIAOD/PromptReco-v2/000/273/150/00000/680BED0F-D919-E611-85E6-02163E01424F.root outputFile=testDoubleMu.root channels=zl isMC=0 eCalib=1 muCalib=1 globalTag=80X_dataRun2_Prompt_ICHEP16JEC_v0 $1
