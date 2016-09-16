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
        nPassJPL = cms.string('? hasUserInt("nPassJPL") ? '
                              'userInt("nPassJPL") : 999'),
        nPassJPM = cms.string('? hasUserInt("nPassJPM") ? ' 
                              'userInt("nPassJPM") : -999'),
        nPassJPT = cms.string('? hasUserInt("nPassJPT") ? '
                              'userInt("nPassJPT") : -999'),
        nPassCSVv2L = cms.string('? hasUserInt("nPassCSVv2L") ? '
                              'userInt("nPassCSVv2L") : -999'),
        nPassCSVv2M = cms.string('? hasUserInt("nPassCSVv2M") ? '
                                 'userInt("nPassCSVv2M") : -999'),
        nPassCSVv2T = cms.string('? hasUserInt("nPassCSVv2T") ? '
                                 'userInt("nPassCSVv2T") : -999'),
        nPassCMVAv2L = cms.string('? hasUserInt("nPassCMVAv2L") ? '
                                  'userInt("nPassCMVAv2L") : -999'),
        nPassCMVAv2M = cms.string('? hasUserInt("nPassCMVAv2M") ? '
                                  'userInt("nPassCMVAv2M") : -999'),
        nPassCMVAv2T = cms.string('? hasUserInt("nPassCMVAv2T") ? '
                                  'userInt("nPassCMVAv2T") : -999'),
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

