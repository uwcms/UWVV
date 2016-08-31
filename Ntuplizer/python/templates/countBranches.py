import FWCore.ParameterSet.Config as cms


wzCountBranches = cms.PSet(
    uints = cms.PSet(
        nCBVIDTightElec = cms.string('? hasUserInt("nCBVIDtightElec") ?'
                                     'userInt("nCBVIDtightElec") : 999'),
        nCBVIDMediumElec = cms.string('? hasUserInt("nCBVIDmediumElec") ?'
                                      'userInt("nCBVIDmediumElec") : 999'),
        nCBVIDLooseElec = cms.string('? hasUserInt("nCBVIDlooseElec") ?'
                                     'userInt("nCBVIDlooseElec") : 999'),
        nTightMuon = cms.string('? hasUserInt("nTightMuon") ? '
                                'userInt("nTightMuon") : 999'),
        nMediumMuonICHEP = cms.string('? hasUserInt("nMediumMuonICHEP") ? '
                                      'userInt("nMediumMuonICHEP") : 999'),
        ),
    )

zzCountBranches = cms.PSet(
    uints = cms.PSet(
        nZZLooseElec = cms.string('? hasUserInt("nZZLooseElectrons") ?'
                                  'userInt("nZZLooseElectrons") : 999'),
        nZZTightElec = cms.string('? hasUserInt("nZZTightElectrons") ?'
                                  'userInt("nZZTightElectrons") : 999'),
        nZZTightIsoElec = cms.string('? hasUserInt("nZZTightIsoElectrons") ?'
                                     'userInt("nZZTightIsoElectrons") : 999'),
        nZZLooseMu = cms.string('? hasUserInt("nZZLooseMuons") ?'
                                'userInt("nZZLooseMuons") : 999'),
        nZZTightMu = cms.string('? hasUserInt("nZZTightMuons") ?'
                                'userInt("nZZTightMuons") : 999'),
        nZZTightIsoMu = cms.string('? hasUserInt("nZZTightIsoMuons") ?'
                                   'userInt("nZZTightIsoMuons") : 999'),
        ),
    )

