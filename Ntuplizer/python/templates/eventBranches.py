import FWCore.ParameterSet.Config as cms


eventBranches = cms.PSet(
    floats = cms.PSet(
        pvndof = cms.string('pvndof'),
        pvZ = cms.string('pvZ'),
        pvRho = cms.string('pvRho'),
        type1_pfMETEt = cms.string('type1_pfMETEt'),
        type1_pfMETPhi = cms.string('type1_pfMETPhi'),
        ),
    vFloats = cms.PSet(
        lheWeights = cms.vstring('lheWeights'),
        jetPt = cms.vstring('jetPt'),
        jetEta = cms.vstring('jetEta'),
        jetPhi = cms.vstring('jetPhi'),
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
        ),
    ulls = cms.PSet(
        evt = cms.string('evt'),
        ),
    )

# gen information branches for regular ntuple
eventGenBranches = cms.PSet(
    floats = cms.PSet(
        genWeight = cms.string('genWeight'),
        nTruePU = cms.string('nTruePU'),
        ),
    )

# event branches for gen ntuple
genNtupleEventBranches = cms.PSet(
    floats = cms.PSet(
        genWeight = cms.string('genWeight'),
        ),
    vFloats = cms.PSet(
        jetPt = cms.vstring('genJetPt'),
        jetEta = cms.vstring('genJetEta'),
        jetPhi = cms.vstring('genJetPhi'),
        ),
    uints = cms.PSet(
        lumi = cms.string('lumi'),
        run = cms.string('run'),
        nJets = cms.string('nGenJets'),
        ),
    ulls = cms.PSet(
        evt = cms.string('evt'),
        ),
    )

jesSystematicBranches = cms.PSet(
    floats = cms.PSet(
        mjj_jesUp = cms.string('mjj::jesUp'),
        mjj_jesDown = cms.string('mjj::jesDown'),
        mjj_jerUp = cms.string('mjj::jerUp'),
        mjj_jerDown = cms.string('mjj::jerDown'),
        ),
    vFloats = cms.PSet(
        jetPt_jesUp = cms.vstring('jetPt::jesUp'),
        jetPt_jesDown = cms.vstring('jetPt::jesDown'),
        jetPt_jerUp = cms.vstring('jetPt::jerUp'),
        jetPt_jerDown = cms.vstring('jetPt::jerDown'),
        ),
    uints = cms.PSet(
        nJets_jesUp = cms.string('nJets::jesUp'),
        nJets_jesDown = cms.string('nJets::jesDown'),
        nJets_jerUp = cms.string('nJets::jerUp'),
        nJets_jerDown = cms.string('nJets::jerDown'),
        ),
    )

centralJetBranches = cms.PSet(
    floats = cms.PSet(
        mjj_eta2p4 = cms.string('mjj::eta2p4'),
        ),
    vFloats = cms.PSet(
        jetPt_eta2p4 = cms.vstring('jetPt::eta2p4'),
        ),
    uints = cms.PSet(
        nJets_eta2p4 = cms.string('nJets::eta2p4'),
        ),
    )

