import FWCore.ParameterSet.Config as cms


muonBranches = cms.PSet(
    floats = cms.PSet(
        RelPFIsoDB = cms.string('(pfIsolationR03.sumChargedParticlePt'
                                '+max(0.0,pfIsolationR03.sumPhotonEt'
                                '+pfIsolationR03.sumNeutralHadronEt'
                                '-0.5*pfIsolationR03.sumPUPt))'
                                '/pt()'),
        PFChargedIso = cms.string('pfIsolationR03.sumChargedParticlePt'),
        PFPhotonIso = cms.string('pfIsolationR03.sumPhotonEt'),
        PFNeutralIso = cms.string('pfIsolationR03.sumNeutralHadronEt'),
        PFPUIso = cms.string('pfIsolationR03.sumPUPt'),
        SIP3D = cms.string('SIP3D'),
        PVDXY = cms.string('PVDXY'),
        PVDZ = cms.string('PVDZ'),
        GenPt = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.pt'),
        GenEta = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.eta'),
        GenPhi = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.phi'),
        ),
    ints = cms.PSet(
        GenPdgId = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.pdgId'),
        GenCharge = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.charge'),
        ),
    uints = cms.PSet(
        MatchedStations = cms.string('MatchedStations'),
        BestTrackType = cms.string('BestTrackType'),
        ),
    bools = cms.PSet(
        IsPFMuon = cms.string('IsPFMuon'),
        IsGlobal = cms.string('IsGlobal'),
        IsTracker = cms.string('IsTracker'),
        ),
    )
