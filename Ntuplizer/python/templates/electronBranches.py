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

        IDIsoEffScaleFactor = cms.string('? isGap ? '
                                         '(? hasUserFloat("effScaleFactorGap") ?'
                                         ' userFloat("effScaleFactorGap") : 1.) :'
                                         '(? hasUserFloat("effScaleFactor") ? '
                                         'userFloat("effScaleFactor") : 1.)'),
        TrkRecoEffScaleFactor = cms.string('? hasUserFloat("trkRecoEffScaleFactor") ? '
                                           'userFloat("trkRecoEffScaleFactor") : 1.'),
        EffScaleFactor = cms.string('(? isGap ? '
                                    ' (? hasUserFloat("effScaleFactorGap") ?'
                                    '  userFloat("effScaleFactorGap") : 1.) :'
                                    ' (? hasUserFloat("effScaleFactor") ? '
                                    '  userFloat("effScaleFactor") : 1.) '
                                    ') * '
                                    '(? hasUserFloat("trkRecoEffScaleFactor") ? '
                                    ' userFloat("trkRecoEffScaleFactor") : '
                                    ' 1. '
                                    ')'),
        ),

    uints = cms.PSet(
        MissingHits = cms.string('MissingHits'),
        ),
    bools = cms.PSet(
        IsGap = cms.string('isGap'),

        IsCBVIDtight = cms.string('userInt("CBVIDtight")'),
        IsCBVIDmedium = cms.string('userInt("CBVIDmedium")'),
        IsCBVIDloose = cms.string('userInt("CBVIDloose")'),
        )
    )
