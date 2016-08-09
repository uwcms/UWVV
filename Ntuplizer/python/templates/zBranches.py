import FWCore.ParameterSet.Config as cms


zBranches = cms.PSet(
    floats = cms.PSet(
        DR = cms.string('DR'),
        Mt = cms.string('mt'),
        Mass = cms.string('mass'),
        MassNoFSR = cms.string('massNoFSR'),
        ),
    bools = cms.PSet(
        SS = cms.string('SS'),
        ),
    )
