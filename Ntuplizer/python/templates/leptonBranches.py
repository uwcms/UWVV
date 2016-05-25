import FWCore.ParameterSet.Config as cms


leptonBranches = cms.PSet(
    floats = cms.PSet(
        FSREt   = cms.string('? hasUserCand("fsr") ? userCand("fsr").et() : -999.'),
        FSREta  = cms.string('? hasUserCand("fsr") ? userCand("fsr").eta() : -999.'),
        FSRPhi  = cms.string('? hasUserCand("fsr") ? userCand("fsr").phi() : -999.'),
        FSRDREt = cms.string('? hasUserFloat("fsrDREt") ? userFloat("fsrDREt") : 999.'),
        ),
    ints = cms.PSet(
        PdgId = cms.string('pdgId'),
        ),
    )
