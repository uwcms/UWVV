import FWCore.ParameterSet.Config as cms


fsBranches = cms.PSet(
    floats = cms.PSet(
        Mt = cms.string('mt'),
        MassFSR = cms.string('massFSR'),
        ),
    )

kinFitBranches = cms.PSet(
    floats = cms.PSet(
        MassRefit = cms.string('? hasUserFloat("massRefit") ? '
                               'userFloat("massRefit") : mass'),
        MassRefitError = cms.string('? hasUserFloat("massRefitError") ? '
                                    'userFloat("massRefitError") : mass'),
        ),
    )
