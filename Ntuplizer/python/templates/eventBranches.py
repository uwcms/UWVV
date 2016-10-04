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
        mjj = cms.string('mjj'),
        deltaEtajj = cms.string('deltaEtajj'),
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
        jet1Pt = cms.string('genJet1Pt'),
        jet1Eta = cms.string('genJet1Eta'),
        jet2Pt = cms.string('genJet2Pt'),
        jet2Eta = cms.string('genJet2Eta'),
        mjj = cms.string('mjjGen'),
        deltaEtajj = cms.string('deltaEtajjGen'),
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
        jet1Pt_jesUp = cms.string('jet1Pt::jesUp'),
        jet2Pt_jesUp = cms.string('jet2Pt::jesUp'),
        mjj_jesUp = cms.string('mjj::jesUp'),
        deltaEtajj_jesUp = cms.string('deltaEtajj::jesUp'),
        jet1Pt_jesDown = cms.string('jet1Pt::jesDown'),
        jet2Pt_jesDown = cms.string('jet2Pt::jesDown'),
        mjj_jesDown = cms.string('mjj::jesDown'),
        deltaEtajj_jesDown = cms.string('deltaEtajj::jesDown'),
        jet1Pt_jerUp = cms.string('jet1Pt::jerUp'),
        jet2Pt_jerUp = cms.string('jet2Pt::jerUp'),
        mjj_jerUp = cms.string('mjj::jerUp'),
        deltaEtajj_jerUp = cms.string('deltaEtajj::jerUp'),
        jet1Pt_jerDown = cms.string('jet1Pt::jerDown'),
        jet2Pt_jerDown = cms.string('jet2Pt::jerDown'),
        mjj_jerDown = cms.string('mjj::jerDown'),
        deltaEtajj_jerDown = cms.string('deltaEtajj::jerDown'),
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
        jet1Pt_eta2p4 = cms.string('jet1Pt::eta2p4'),
        jet2Pt_eta2p4 = cms.string('jet2Pt::eta2p4'),
        mjj_eta2p4 = cms.string('mjj::eta2p4'),
        deltaEtajj_eta2p4 = cms.string('deltaEtajj::eta2p4'),
        ),
    uints = cms.PSet(
        nJets_eta2p4 = cms.string('nJets::eta2p4'),
        ),
    )

