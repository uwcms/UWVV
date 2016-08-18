import FWCore.ParameterSet.Config as cms


eventBranches = cms.PSet(
    floats = cms.PSet(
        pvndof = cms.string('pvndof'),
        pvZ = cms.string('pvZ'),
        pvRho = cms.string('pvRho'),
        type1_pfMETEt = cms.string('type1_pfMETEt'),
        type1_pfMETPhi = cms.string('type1_pfMETPhi'),
        jet1Pt = cms.string('jet1Pt'),
        jet2Pt = cms.string('jet2Pt'),
        ),
    bools = cms.PSet(
        pvIsValid = cms.string('pvIsValid'),
        pvIdFake = cms.string('pvIsFake'),
        ),
    uints = cms.PSet(
        lumi = cms.string('lumi'),
        run = cms.string('run'),
        nvtx = cms.string('nvtx'),
        nJets = cms.string('nJets'),
        nCBVIDTightElec = cms.string('userInt("nCBVIDTightElec")'),
        nCBVIDMediumElec = cms.string('userInt("nCBVIDMediumElec")'),
        nCBVIDLooseElec = cms.string('userInt("nCBVIDLooseElec")'),
        nTightMuon = cms.string('userInt("nTightMuon")'),
        nMediumMuonICHEP = cms.string('userInt("nMediumMuonICHEP")'),
        ),
    ulls = cms.PSet(
        evt = cms.string('evt'),
        ),
    )

eventGenBranches = cms.PSet(
    floats = cms.PSet(
        genWeight = cms.string('genWeight'),
        nTruePU = cms.string('nTruePU'),
        ),
    )
