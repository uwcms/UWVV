import FWCore.ParameterSet.Config as cms


electronBranches = cms.PSet(
    floats = cms.PSet(
        MVANonTrigID = cms.string('userFloat("MVAIDNonTrig")'),
        RelPFIsoRho = cms.string('(pfIsolationVariables.sumChargedHadronPt'
                                 '+max(0.0,pfIsolationVariables.sumNeutralHadronEt'
                                 '+pfIsolationVariables.sumPhotonEt'
                                 '-userFloat("rho_fastjet")*userFloat("EffectiveArea")))'
                                 '/pt'),
        Rho = cms.string('userFloat("rho_fastjet")'),
        EffectiveArea = cms.string('userFloat("EffectiveArea")'),
        PFChargedIso = cms.string('pfIsolationVariables.sumChargedHadronPt'),
        PFPhotonIso = cms.string('pfIsolationVariables.sumPhotonEt'),
        PFNeutralIso = cms.string('pfIsolationVariables.sumNeutralHadronEt'),
        PFPUIso = cms.string('pfIsolationVariables.sumPUPt'),
        SCEta = cms.string('superCluster.eta'),
        SCPhi = cms.string('superCluster.phi'),
        SCEnergy = cms.string('superCluster.energy'),
        SCRawEnergy = cms.string('superCluster.rawEnergy'),
        GenPt = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.pt'),
        GenEta = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.eta'),
        GenPhi = cms.string('? genParticleRef.isNull ? -999. : genParticleRef.phi'),
        ),
    ints = cms.PSet(
        GenPdgId = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.pdgId'),
        GenCharge = cms.string('? genParticleRef.isNull ? -999 : genParticleRef.charge'),
        ),
    uints = cms.PSet(
        MissingHits = cms.string('MissingHits'),
        ),
    )
