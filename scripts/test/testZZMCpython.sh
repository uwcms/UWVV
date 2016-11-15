#!/bin/bash
python Ntuplizer/test/ntuplize_cfg.py outputFile=testZZMC.root channels=zz isMC=1 eCalib=1 muCalib=1 globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1 genInfo=1 $1 
