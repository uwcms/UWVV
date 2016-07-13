import FWCore.ParameterSet.Config as cms


leptonBranches = cms.PSet(
    floats = cms.PSet(
        FSREt   = cms.string('? hasUserCand("fsr") ? userCand("fsr").et() : -999.'),
        FSREta  = cms.string('? hasUserCand("fsr") ? userCand("fsr").eta() : -999.'),
        FSRPhi  = cms.string('? hasUserCand("fsr") ? userCand("fsr").phi() : -999.'),
        FSRDREt = cms.string('? hasUserFloat("fsrDREt") ? userFloat("fsrDREt") : 999.'),

        ZZIso = cms.string('? hasUserFloat("ZZIsoVal") ? userFloat("ZZIsoVal") : 999.'),

        SIP3D = cms.string('SIP3D'),
        PVDXY = cms.string('PVDXY'),
        PVDZ = cms.string('PVDZ'),

        GenPt = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.pt'),
        GenEta = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.eta'),
        GenPhi = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.phi'),

        EffScaleFactor = cms.string('? hasUserFloat("effScaleFactor") ? '
                                    'userFloat("effScaleFactor") : 1.'),
        EffScaleFactorError = cms.string('? hasUserFloat("effScaleFactorError") ? '
                                         'userFloat("effScaleFactorError") : 0.'),
        ),
    ints = cms.PSet(
        PdgId = cms.string('pdgId'),
        GenPdgId = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.pdgId'),
        GenCharge = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.charge'),
        ),
    bools = cms.PSet(
        ZZLooseID = cms.string('? hasUserFloat("ZZIDPass") ? userFloat("ZZIDPass") : 0.'),
        ZZTightID = cms.string('? hasUserFloat("ZZIDPassTight") ? userFloat("ZZIDPassTight") : 0.'),
        ZZLooseIDNoVtx = cms.string('? hasUserFloat("ZZIDPassNoVtx") ? userFloat("ZZIDPassNoVtx") : 0.'),
        ZZTightIDNoVtx = cms.string('? hasUserFloat("ZZIDPassTightNoVtx") ? userFloat("ZZIDPassTightNoVtx") : 0.'),
        ZZIsoPass = cms.string('? hasUserFloat("ZZIsoPass") ? userFloat("ZZIsoPass") : 0.'),
        ),
    )
