import FWCore.ParameterSet.Config as cms


vbsBranches = cms.PSet(
    floats = cms.PSet(
        rapidity = cms.string('rapidity'),
        mjj = cms.string('mjj'),
        deltaPhiTojj = cms.string('deltaPhiTojj'),
        zeppenfeld = cms.string('zeppenfeld'),
        zeppenfeldj3 = cms.string('zeppenfeldj3'),
        deltaEtajj = cms.string('deltaEtajj'),
        ),
    vFloats = cms.PSet(
        jetRapidity = cms.vstring('jetRapidity'),
        ),
    )


vbsGenBranches = cms.PSet(
    floats = cms.PSet(
        rapidity = cms.string('rapidity'),
        mjj = cms.string('mjjGen'),
        deltaPhiTojj = cms.string('deltaPhiTojjGen'),
        zeppenfeld = cms.string('zeppenfeldGen'),
        zeppenfeldj3 = cms.string('zeppenfeldj3Gen'),
        deltaEtajj = cms.string('deltaEtajjGen'),
        ),
    vFloats = cms.PSet(
        jetRapidity = cms.vstring('genJetRapidity'),
        ),
    )


vbsSystematicBranches = cms.PSet(
    floats = cms.PSet(
        mjj_jesUp   = cms.string('mjj::jesUp'),
        mjj_jesDown = cms.string('mjj::jesDown'),
        mjj_jerUp   = cms.string('mjj::jerUp'),
        mjj_jerDown = cms.string('mjj::jerDown'),
        deltaPhiTojj_jesUp   = cms.string('deltaPhiTojj::jesUp'),
        deltaPhiTojj_jesDown = cms.string('deltaPhiTojj::jesDown'),
        deltaPhiTojj_jerUp   = cms.string('deltaPhiTojj::jerUp'),
        deltaPhiTojj_jerDown = cms.string('deltaPhiTojj::jerDown'),
        zeppenfeld_jesUp   = cms.string('zeppenfeld::jesUp'),
        zeppenfeld_jesDown = cms.string('zeppenfeld::jesDown'),
        zeppenfeld_jerUp   = cms.string('zeppenfeld::jerUp'),
        zeppenfeld_jerDown = cms.string('zeppenfeld::jerDown'),
        zeppenfeldj3_jesUp   = cms.string('zeppenfeldj3::jesUp'),
        zeppenfeldj3_jesDown = cms.string('zeppenfeldj3::jesDown'),
        zeppenfeldj3_jerUp   = cms.string('zeppenfeldj3::jerUp'),
        zeppenfeldj3_jerDown = cms.string('zeppenfeldj3::jerDown'),
        deltaEtajj_jesUp   = cms.string('deltaEtajj::jesUp'),
        deltaEtajj_jesDown = cms.string('deltaEtajj::jesDown'),
        deltaEtajj_jerUp   = cms.string('deltaEtajj::jerUp'),
        deltaEtajj_jerDown = cms.string('deltaEtajj::jerDown'),
        ),
    vFloats = cms.PSet(
        jetRapidity_jesUp = cms.vstring('jetRapidity::jesUp'),
        jetRapidity_jesDown = cms.vstring('jetRapidity::jesDown'),
        jetRapidity_jerUp = cms.vstring('jetRapidity::jerUp'),
        jetRapidity_jerDown = cms.vstring('jetRapidity::jerDown'),
        ),
    )


centralVBSBranches = cms.PSet(
    floats = cms.PSet(
        mjj_eta2p4 = cms.string('mjj::eta2p4'),
        deltaPhiTojj_eta2p4 = cms.string('deltaPhiTojj::eta2p4'),
        zeppenfeld_eta2p4 = cms.string('zeppenfeld::eta2p4'),
        zeppenfeldj3_eta2p4 = cms.string('zeppenfeldj3::eta2p4'),
        deltaEtajj_eta2p4 = cms.string('deltaEtajj::eta2p4'),
        ),
    vFloats = cms.PSet(
        jetRapidity_eta2p4 = cms.vstring('jetRapidity::eta2p4'),
        ),
    )
