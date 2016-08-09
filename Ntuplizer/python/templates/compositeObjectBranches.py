import FWCore.ParameterSet.Config as cms

compositeObjectBranches = cms.PSet(
    floats = cms.PSet(
        PtNoFSR = cms.string('ptNoFSR'),
        EtaNoFSR = cms.string('etaNoFSR'),
        PhiNoFSR = cms.string('phiNoFSR'),
        EnergyNoFSR = cms.string('energyNoFSR'),
        ),
    )
