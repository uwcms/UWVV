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
        ),
    uints = cms.PSet(
        MatchedStations = cms.string('MatchedStations'),
        BestTrackType = cms.string('BestTrackType'),
        ),
    bools = cms.PSet(
        IsPFMuon = cms.string('IsPFMuon'),
        IsGlobal = cms.string('IsGlobal'),
        IsTracker = cms.string('IsTracker'),

        HighPtID = cms.string('? hasUserFloat("highPtIDPass") ? '
                              'userFloat("highPtIDPass") : 0.'),
        TrackerHighPtID = cms.string('? hasUserFloat("trackerHighPtIDPass") ? '
                                     'userFloat("trackerHighPtIDPass") : 0.'),
        ZZHighPtID = cms.string('? hasUserFloat("highPtIDPass") && hasUserFloat("ZZIDPass") ? '
                                '(? userFloat("ZZIDPass") > 0. || (pt > 200. && userFloat("trackerHighPtIDPass") > 0.) ? '
                                ' 1. : 0.) : 0.'), 
        ),
    )
