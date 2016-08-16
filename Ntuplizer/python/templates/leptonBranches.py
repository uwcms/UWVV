import FWCore.ParameterSet.Config as cms


leptonBranches = cms.PSet(
    floats = cms.PSet(
        ZZIso = cms.string('? hasUserFloat("ZZIsoVal") ? userFloat("ZZIsoVal") : 999.'),

        SIP3D = cms.string('SIP3D'),
        PVDXY = cms.string('PVDXY'),
        PVDZ = cms.string('PVDZ'),
        ),
    ints = cms.PSet(
        PdgId = cms.string('pdgId'),
        ),
    bools = cms.PSet(
        ZZLooseID = cms.string('? hasUserFloat("ZZIDPass") ? userFloat("ZZIDPass") : 0.'),
        ZZTightID = cms.string('? hasUserFloat("ZZIDPassTight") ? userFloat("ZZIDPassTight") : 0.'),
        ZZLooseIDNoVtx = cms.string('? hasUserFloat("ZZIDPassNoVtx") ? userFloat("ZZIDPassNoVtx") : 0.'),
        ZZTightIDNoVtx = cms.string('? hasUserFloat("ZZIDPassTightNoVtx") ? userFloat("ZZIDPassTightNoVtx") : 0.'),
        ZZIsoPass = cms.string('? hasUserFloat("ZZIsoPass") ? userFloat("ZZIsoPass") : 0.'),
        ),
    )

leptonGenBranches = cms.PSet(
    floats = cms.PSet(
        GenPt = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.pt'),
        GenEta = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.eta'),
        GenPhi = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.phi'),
        ),
    ints = cms.PSet(
        GenPdgId = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.pdgId'),
        GenCharge = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.charge'),
        ),
    )
