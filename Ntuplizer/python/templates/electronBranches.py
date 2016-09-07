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

        EffScaleFactor = cms.string('? isGap ? '
                                    '(? hasUserFloat("effScaleFactorGap") ?'
                                    ' userFloat("effScaleFactorGap") : 1.) :'
                                    '(? hasUserFloat("effScaleFactor") ? '
                                    'userFloat("effScaleFactor") : 1.)'),
        EffScaleFactorError = cms.string('? isGap ? '
                                         '(? hasUserFloat("effScaleFactorErrorGap") ?'
                                         ' userFloat("effScaleFactorErrorGap") : 0.) :'
                                         '(? hasUserFloat("effScaleFactorError") ? '
                                         'userFloat("effScaleFactorError") : 0.)'),
        TrkRecoEffScaleFactor = cms.string('? hasUserFloat("trkRecoEffScaleFactor") ? '
                                           'userFloat("trkRecoEffScaleFactor") : 1.'),
        MtToMET = cms.string('mtToMET'),
        ),

    uints = cms.PSet(
        MissingHits = cms.string('MissingHits'),
        ),
    bools = cms.PSet(
        IsGap = cms.string('isGap'),
        IsCBVIDtight = cms.string('? hasUserFloat("isCBVIDtight") ? '
                                  'userFloat("isCBVIDtight") : 0'),
        IsCBVIDmedium = cms.string('? hasUserFloat("isCBVIDmedium") ?'
                                   'userFloat("isCBVIDmedium") : 0'),
        IsCBVIDloose = cms.string('? hasUserFloat("isCBVIDloose") ? '
                                  'userFloat("isCBVIDloose") : 0'),
        )
    )
