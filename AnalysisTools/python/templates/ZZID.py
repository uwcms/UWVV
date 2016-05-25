from UWVV.AnalysisTools.templates.ElectronBaseFlow import ElectronBaseFlow
from UWVV.AnalysisTools.templates.MuonBaseFlow import MuonBaseFlow

import FWCore.ParameterSet.Config as cms


class ZZID(ElectronBaseFlow, MuonBaseFlow):
    def __init__(self, *args, **kwargs):
        super(ZZID, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZID, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding':
            eIDEmbedder = cms.EDProducer(
                "PATElectronZZIDEmbedder",
                src = step.getObjTag('e'),
                idLabel = cms.string(self.getZZIDLabel()),
                vtxSrc = step.getObjTag('v'),
                bdtLabel = cms.string('MVAIDNonTrig'),
                idCutLowPtLowEta = cms.double(-.265),
                idCutLowPtMedEta = cms.double(-.556),
                idCutLowPtHighEta = cms.double(-.551),
                idCutHighPtLowEta = cms.double(-.072),
                idCutHighPtMedEta = cms.double(-.286),
                idCutHighPtHighEta = cms.double(-.267),
                missingHitsCut = cms.int32(999),
                ptCut = cms.double(7.), 
                )

            mIDEmbedder = cms.EDProducer(
                "PATMuonZZIDEmbedder",
                src = step.getObjTag('m'),
                vtxSrc = step.getObjTag('v'),
                ptCut = cms.double(5.),
                idLabel = cms.string(self.getZZIDLabel()),
                )

            step.addModule("eZZIDEmbedder", eIDEmbedder, 'e')
            step.addModule("mZZIDEmbedder", mIDEmbedder, 'm')

        return step

    def getZZIDLabel(self):
        return 'ZZIDPass'
