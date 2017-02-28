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

        # apparently there's a bug with multiple ternary operators, so cheat
        EffScaleFactor = cms.string('? hasUserFloat("effScaleFactorGap") && '
                                    '  hasUserFloat("effScaleFactor") && '
                                    '  hasUserFloat("trkRecoEffScaleFactor") ? '
                                    'userFloat("trkRecoEffScaleFactor") * '
                                    '(isGap * userFloat("effScaleFactorGap") + '
                                    ' (1-isGap) * userFloat("effScaleFactor")) :'
                                    '1.'),
        EffScaleFactorError = cms.string('? hasUserFloat("effScaleFactorGapError") && '
                                         '  hasUserFloat("effScaleFactorError") && '
                                         '  hasUserFloat("trkRecoEffScaleFactorError") && '
                                         '  hasUserFloat("trkRecoEffScaleFactorExtraError") ? '
                                         'sqrt((userFloat("trkRecoEffScaleFactorError") + '
                                         '      userFloat("trkRecoEffScaleFactorExtraError"))^2 + '
                                         '(isGap * userFloat("effScaleFactorGapError") + '
                                         '(1-isGap) * userFloat("effScaleFactorError"))^2) :'
                                         '0.'),
        TrkRecoEffScaleFactor = cms.string('? hasUserFloat("trkRecoEffScaleFactor") ? '
                                           'userFloat("trkRecoEffScaleFactor") : 1.'),
        TrkRecoEffScaleFactorError = cms.string('(? hasUserFloat("trkRecoEffScaleFactorError") && '
                                                '   hasUserFloat("trkRecoEffScaleFactorExtraError") ? '
                                                ' userFloat("trkRecoEffScaleFactorError") + '
                                                ' userFloat("trkRecoEffScaleFactorExtraError") : '
                                                ' 0.)'),
        IDIsoEffScaleFactor = cms.string('? hasUserFloat("effScaleFactorGap") && '
                                         '  hasUserFloat("effScaleFactor") ? '
                                         'isGap * userFloat("effScaleFactorGap") + '
                                         '(1-isGap) * userFloat("effScaleFactor") :'
                                         '1.'),
        IDIsoEffScaleFactorError = cms.string('? hasUserFloat("effScaleFactorGapError") && '
                                              '  hasUserFloat("effScaleFactorError") ? '
                                              'isGap * userFloat("effScaleFactorGapError") + '
                                              '(1-isGap) * userFloat("effScaleFactorError") :'
                                              '0.'),

        MtToMET = cms.string('mtToMET'),
        ),

    uints = cms.PSet(
        MissingHits = cms.string('MissingHits'),
        ),
    bools = cms.PSet(
        IsGap = cms.string('isGap'),
        IsEB = cms.string('isEB'),
        IsCBVIDTightNoIP = cms.string('? hasUserFloat("IsCBVIDTight") ? '
                                  'userFloat("IsCBVIDTight") : 0'),
        IsCBVIDTight = cms.string('? hasUserFloat("IsCBVIDTightwIP") ? '
                                  'userFloat("IsCBVIDTightwIP") : 0'),
        IsCBVIDMediumNoIP = cms.string('? hasUserFloat("IsCBVIDMedium") ?'
                                   'userFloat("IsCBVIDMedium") : 0'),
        IsCBVIDMedium = cms.string('? hasUserFloat("IsCBVIDMediumwIP") ? '
                                  'userFloat("IsCBVIDMediumwIP") : 0'),
        IsCBVIDLooseNoIP = cms.string('? hasUserFloat("IsCBVIDLoose") ? '
                                  'userFloat("IsCBVIDLoose") : 0'),
        IsCBVIDLoose = cms.string('? hasUserFloat("IsCBVIDLoosewIP") ? '
                                  'userFloat("IsCBVIDLoosewIP") : 0'),
        IsCBVIDVetoNoIP = cms.string('? hasUserFloat("IsCBVIDVeto") ? '
                                  'userFloat("IsCBVIDVeto") : 0'),
        IsCBVIDVeto = cms.string('? hasUserFloat("IsCBVIDVetowIP") ? '
                                  'userFloat("IsCBVIDVetowIP") : 0'),
        IsCBVIDHLTSafeNoIP = cms.string('? hasUserFloat("IsCBVIDHLTSafe") ? '
                                  'userFloat("IsCBVIDHLTSafe") : 0'),
        IsCBVIDHLTSafe = cms.string('? hasUserFloat("IsCBVIDHLTSafewIP") ? '
                                  'userFloat("IsCBVIDHLTSafewIP") : 0'),
        IsWWLoose = cms.string('? hasUserInt("IsWWLoose") ? '
                                  'userInt("IsWWLoose") : 0'),
        )
    )
