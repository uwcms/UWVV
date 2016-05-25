import FWCore.ParameterSet.Config as cms


zBranches = cms.PSet(
    floats = cms.PSet(
        DR = cms.string('DR'),
        Mt = cms.string('mt'),
        MassFSR = cms.string('massFSR'),
        ),
    bools = cms.PSet(
        SS = cms.string('SS'),
        ),
    )
