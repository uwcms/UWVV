#!/bin/bash
cmsRun Ntuplizer/test/ntuplize_cfg.py inputFiles=/store/mc/RunIISpring16MiniAODv2/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/60000/086609CA-D81B-E611-BF6C-002590D9D896.root outputFile=testMC.root channels=wz isMC=1 eCalib=1 muCalib=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1 $1
