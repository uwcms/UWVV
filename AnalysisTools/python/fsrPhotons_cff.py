###################################################################################################################
###    miniAOD_fsrPhotons_cff.py                                                                                ###
###    Copied from https://github.com/VBF-HZZ/UFHZZAnalysisRun2/blob/csa14/FSRPhotons/python/fsrPhotons_cff.py  ###
###          I take no credit for it                                                                            ###
###################################################################################################################

import FWCore.ParameterSet.Config as cms

## PF Photons
fsrPhotons = cms.EDProducer("MiniAODFSRPhotonProducer",
    srcCands = cms.InputTag("packedPFCandidates"),
    muons = cms.InputTag("slimmedMuons"), 
    ptThresh = cms.double(1.0), ## will tighten to 2 at analysis level
    extractMuonFSR = cms.bool(False),
)

# fsrPhotonPFIsoChHad03pt02 + fsrPhotonPFIsoChHadPU03pt02
fsrPhotonPFIsoChHadPUNoPU03pt02 = cms.EDProducer(
    "PhotonPFIsoCalculator",
    photonLabel = cms.InputTag("fsrPhotons"),
    pfLabel     = cms.InputTag("packedPFCandidates"),
    pfSelection = cms.string("charge!=0 && abs(pdgId)==211 && pt>0.2"),
    deltaR = cms.double(0.3),
    deltaRself = cms.double(0.0001),
    vetoConeEndcaps = cms.double(0.0),
    directional = cms.bool(False),
    debug       = cms.untracked.bool(False)
)

# fsrPhotonPFIsoNHad03 + fsrPhotonPFIsoPhoton03
fsrPhotonPFIsoNHadPhoton03 = cms.EDProducer("PhotonPFIsoCalculator",
    photonLabel = cms.InputTag("fsrPhotons"),
    pfLabel     = cms.InputTag("packedPFCandidates"),
    pfSelection = cms.string("charge==0 && (abs(pdgId)==22||abs(pdgId)==130) && pt>0.5"), 
    deltaR = cms.double(0.3),
    deltaRself  = cms.double(0.01),
    vetoConeEndcaps = cms.double(0.0),
    directional = cms.bool(False),
    debug       = cms.untracked.bool(False) 
)

import PhysicsTools.PatAlgos.producersLayer1.pfParticleProducer_cfi 
boostedFsrPhotons = PhysicsTools.PatAlgos.producersLayer1.pfParticleProducer_cfi.patPFParticles.clone(
    pfCandidateSource = 'fsrPhotons'
)
boostedFsrPhotons.userData.userFloats.src = cms.VInputTag(
    cms.InputTag("fsrPhotonPFIsoChHadPUNoPU03pt02"),
    cms.InputTag("fsrPhotonPFIsoNHadPhoton03")
)

fsrPhotonSequence = cms.Sequence(
    fsrPhotons 
    + fsrPhotonPFIsoChHadPUNoPU03pt02
    + fsrPhotonPFIsoNHadPhoton03
    + boostedFsrPhotons
)

