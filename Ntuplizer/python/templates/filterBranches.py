import FWCore.ParameterSet.Config as cms


# Bad Muon filter for ReMiniAOD following: https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes
# MET filters for Moriond 2017 following: https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
metAndBadMuonFilters = cms.PSet(
    trigNames = cms.vstring(
        # 'Flag_HBHENoiseFilter',
        # 'Flag_HBHENoiseIsoFilter',
        # 'Flag_EcalDeadCellTriggerPrimitiveFilter',
        # 'Flag_goodVertices',
        # 'Flag_eeBadScFilter',
        # 'Flag_globalTightHalo2016Filter',
        # 'Flag_badMuons',
        # 'Flag_duplicateMuons',
    ),
    Flag_HBHENoiseFilterPaths = cms.vstring('Flag_HBHENoiseFilter'),
    Flag_HBHENoiseIsoFilterPaths = cms.vstring('Flag_HBHENoiseIsoFilter'),
    Flag_EcalDeadCellTriggerPrimitiveFilterPaths = cms.vstring('Flag_EcalDeadCellTriggerPrimitiveFilter'),
    Flag_goodVerticesPaths = cms.vstring('Flag_goodVertices'),
    Flag_eeBadScFilterPaths = cms.vstring('Flag_eeBadScFilter'),
    Flag_globalTightHalo2016FilterPaths = cms.vstring('Flag_globalTightHalo2016Filter'),
    Flag_BadChargedCandidateFilterPaths = cms.vstring('Flag_BadChargedCandidateFilter'),
    Flag_BadPFMuonFilterPaths = cms.vstring('Flag_BadPFMuonFilter'),

    Flag_badMuonsPaths = cms.vstring('Flag_badMuons'),
    Flag_duplicateMuonsPaths = cms.vstring('Flag_duplicateMuons'),
    trigResultsSrc = cms.InputTag("TriggerResults", "", "PAT"),
    trigPrescaleSrc = cms.InputTag("patTrigger"),
    checkPrescale = cms.bool(False),
    )

# MET filters for Moriond 2017 following: https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
metFilters = cms.PSet(
    trigNames = cms.vstring(
        # 'Flag_HBHENoiseFilter',
        # 'Flag_HBHENoiseIsoFilter',
        # 'Flag_EcalDeadCellTriggerPrimitiveFilter',
        # 'Flag_goodVertices',
        # 'Flag_eeBadScFilter',
        # 'Flag_globalTightHalo2016Filter',
    ),
    Flag_HBHENoiseFilterPaths = cms.vstring('Flag_HBHENoiseFilter'),
    Flag_HBHENoiseIsoFilterPaths = cms.vstring('Flag_HBHENoiseIsoFilter'),
    Flag_EcalDeadCellTriggerPrimitiveFilterPaths = cms.vstring('Flag_EcalDeadCellTriggerPrimitiveFilter'),
    Flag_goodVerticesPaths = cms.vstring('Flag_goodVertices'),
    Flag_eeBadScFilterPaths = cms.vstring('Flag_eeBadScFilter'),
    Flag_globalTightHalo2016FilterPaths = cms.vstring('Flag_globalTightHalo2016Filter'),

    trigResultsSrc = cms.InputTag("TriggerResults", "", "PAT"),
    trigPrescaleSrc = cms.InputTag("patTrigger"),
    checkPrescale = cms.bool(False),
    )
