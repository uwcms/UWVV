import FWCore.ParameterSet.Config as cms


objectBranches = cms.PSet(
    floats = cms.PSet(
        Pt = cms.string('pt'),
        Eta = cms.string('eta'),
        Phi = cms.string('phi'),
        Mass = cms.string('mass'),
        Energy = cms.string('energy'),
        ),
    ints = cms.PSet(
        Charge = cms.string('charge'),
        PdgId = cms.string('pdgId'),
        ),
    )

