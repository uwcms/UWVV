import FWCore.ParameterSet.Config as cms


objectBranches = cms.PSet(
    floats = cms.PSet(
        Pt = cms.string('pt'),
        Eta = cms.string('eta'),
        Phi = cms.string('phi'),
        PtFSR = cms.string('ptFSR'),
        EtaFSR = cms.string('etaFSR'),
        PhiFSR = cms.string('phiFSR'),
        Mass = cms.string('mass'),
        Energy = cms.string('energy'),
        MtToMET = cms.string('mtToMET'),
        ),
    ints = cms.PSet(
        Charge = cms.string('charge'),
        ),
    )
