from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class ZZID(AnalysisFlowBase):
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
                bdtLabel = cms.string('MVAIsoID'),
                idCutLowPtLowEta = cms.double(0.5739521065342641),
                idCutLowPtMedEta = cms.double(0.5504628790992929),
                idCutLowPtHighEta = cms.double(0.5924627534389098),
                idCutHighPtLowEta = cms.double(-0.03391387993354392),
                idCutHighPtMedEta = cms.double(-0.018451958064666783),
                idCutHighPtHighEta = cms.double(-0.38565459150737535),
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
