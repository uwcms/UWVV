#!/bin/bash
DATE=`date +%Y-%m-%d`
if [ -z "$CMSSW_BASE" ]; then
    echo "Must submit within a CMS environment"
    exit 1
fi
uwvv=$CMSSW_BASE/src/UWVV
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloNoHLT_$DATE.sh \
    --campaign "RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2*" \
    --samples \
        "WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8" \
        "WZTo3LNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" \
        "GluGluToContinToZZTo2e2mu_13TeV_MCFM701_pythia8" \
        "GluGluToContinToZZTo2e2nu_13TeV_MCFM701_pythia8" \
        "GluGluToContinToZZTo4e_13TeV_MCFM701_pythia8" \
        "GluGluToContinToZZTo4mu_13TeV_MCFM701_pythia8" \
        "ZZTo4L_13TeV_powheg_pythia8" \
        "ZZTo2L2Nu_13TeV_powheg_pythia8" \
        "ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8" \
        "TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" \
        "TTJets_DiLept_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" \
        "ZZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "WWW_4F_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "WWZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "WWTo2L2Nu_13TeV-powheg" \
        "WWG_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8" \
        "tZq_ll_4f_13TeV-amcatnlo-pythia8" \
        "ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1" \
        "ST_t-channel_antitop_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1" \
        "ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1" \
        "ST_tW_antitop_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1" \
        "ST_tW_top_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1" \
        "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" \
        "DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" \
    --filesPerJob 3 \
    UWVVNtuples_WZMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1
# Tranche 2 reHLT samples
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloReHLT_$DATE.sh \
    --campaign "RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-*" \
    --samples \
        "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" \
        "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" \
        "ZZTo4L_13TeV-amcatnloFXFX-pythia8" \
        "TT_TuneCUETP8M1_13TeV-powheg-pythia8" \
    --filesPerJob 3 \
    UWVVNtuples_WZMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1
# Tranche 3 withHLT samples
${uwvv}/Utilities/scripts/submitJobs.py \
    -o ${uwvv}/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloWithHLT_$DATE.sh \
    --campaign "RunIISpring16MiniAODv2-premix_withHLT_80X_mcRun2_asymptotic_v14-v1" \
    --samples \
        "WLLJJToLNu_M-60_EWK_13TeV-madgraph-pythia8" \
        "tZq_ll_4f_13TeV-amcatnlo-herwigpp" \
        "TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8" \
        "TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8" \
    --filesPerJob 3 \
    UWVVNtuples_WZMonteCarlo_$DATE \
    $uwvv/Ntuplizer/test/ntuplize_cfg.py \
        channels=wz \
        isMC=1 \
        eCalib=1 
        muCalib=1 \
        globalTag=80X_mcRun2_asymptotic_2016_miniAODv2_v1

chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloNoHLT_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloNoHLT_$DATE.sh
chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloReHLT_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloReHLT_$DATE.sh
chmod +x $CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloWithHLT_$DATE.sh
$CMSSW_BASE/src/UWVV/scripts/WZSubmission/GeneratedScripts/submitWZMonteCarloWithHLT_$DATE.sh
