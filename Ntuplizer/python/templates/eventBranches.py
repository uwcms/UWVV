import FWCore.ParameterSet.Config as cms


eventBranches = cms.PSet(
    floats = cms.PSet(
        pvndof = cms.string('pvndof'),
        pvZ = cms.string('pvZ'),
        pvRho = cms.string('pvRho'),
        nTruePU = cms.string('nTruePU'),
        type1_pfMETEt = cms.string('type1_pfMETEt'),
        type1_pfMETPhi = cms.string('type1_pfMETPhi'),
        genWeight = cms.string('genWeight'),
        jet1Pt = cms.string('jet1Pt'),
        jet2Pt = cms.string('jet2Pt'),
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
        nJets = cms.string('nJets'),
        nVIDTightElec = cms.string('userInt("nCBVIDTightElec")'),
        nVIDMedumElec = cms.string('userInt("nCBVIDMediumElec")'),
        nVIDLooseElec = cms.string('userInt("nCBVIDLooseElec")'),
        nVIDTightMuon = cms.string('userInt("nTightMuon")'),
        nVIDMedumMuonICHEP = cms.string('userInt("nMediumMuonICHEP")'),
        ),
    ulls = cms.PSet(
        evt = cms.string('evt'),
        ),
    )
