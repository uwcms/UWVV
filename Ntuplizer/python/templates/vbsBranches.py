import FWCore.ParameterSet.Config as cms


vbsBranches = cms.PSet(
    floats = cms.PSet(
        rapidity = cms.string('rapidity'),
        deltaPhiToJJ = cms.string('deltaPhiToJJ'),
        zeppenfeld = cms.string('zeppenfeld'),
        mjj = cms.string('mjj'),
        deltaEtajj = cms.string('deltaEtajj'),
        ),
    vFloats = cms.PSet(
        jetRapidity = cms.vstring('jetRapidity'),
        ),
    )


vbsGenBranches = cms.PSet(
    floats = cms.PSet(
        mjj = cms.string('mjjGen'),
        deltaEtajj = cms.string('deltaEtajjGen'),
        ),
    )
