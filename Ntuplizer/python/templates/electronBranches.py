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
        ),
    uints = cms.PSet(
        MissingHits = cms.string('MissingHits'),
        ),
    )
