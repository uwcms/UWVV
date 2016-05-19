import FWCore.ParameterSet.Config as cms


eventBranches = cms.PSet(
    floats = cms.PSet(
        pvndof = cms.string('pvndof'),
        pvZ = cms.string('pvZ'),
        pvRho = cms.string('pvRho'),
        nTruePU = cms.string('nTruePU'),
        type1_pfMetEt = cms.string('type1_pfMetEt'),
        type1_pfMetPhi = cms.string('type1_pfMetPhi'),
        genWeight = cms.string('genWeight'),
        ),
    bools = cms.PSet(
        SS = cms.string('SS'),
        pvIsValid = cms.string('pvIsValid'),
        pvIdFake = cms.string('pvIsFake'),
        ),
    uints = cms.PSet(
        lumi = cms.string('lumi'),
        run = cms.string('run'),
        nvtx = cms.string('nvtx'),
        ),
    ulls = cms.PSet(
        evt = cms.string('evt'),
        ),
    )
