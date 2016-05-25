import FWCore.ParameterSet.Config as cms



triggerBranches = cms.PSet(
    trigNames = cms.vstring('doubleMu', 'doubleE', 
                            'singleESingleMu', 'singleMuSingleE',
                            'tripleE', 'doubleESingleMu', 'doubleMuSingleE',
                            'tripleMu', 'singleE'),
    doubleMuPaths = cms.vstring('HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v\\[0-9]+',
                                'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v\\[0-9]+'),
    doubleEPaths = cms.vstring('HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v\\[0-9]+'),
    singleESingleMuPaths = cms.vstring('HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v\\[0-9]+'),
    singleMuSingleEPaths = cms.vstring('HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v\\[0-9]+'),
    tripleEPaths = cms.vstring('HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v\\[0-9]+'),
    doubleESingleMuPaths = cms.vstring('HLT_Mu8_DiEle12_CaloIdL_TrackIdL_v\\[0-9]+'),
    doubleMuSingleEPaths = cms.vstring('HLT_DiMu9_Ele9_CaloIdL_TrackIdL_v\\[0-9]+'),
    tripleMuPaths = cms.vstring('HLT_TripleMu_12_10_5_v\\[0-9]+'),
    singleEPaths = cms.vstring('HLT_Ele27_WPLoose_Gsf_v\\[0-9]+'),

    trigResultsSrc = cms.InputTag("TriggerResults", "", "HLT"),
    trigPrescaleSrc = cms.InputTag("patTrigger"),
    )

