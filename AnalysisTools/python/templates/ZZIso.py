from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class ZZIso(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ZZIso, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZIso, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding':
            leptonIsoEmbedding = cms.EDProducer(
                "PATLeptonZZIsoEmbedder",
                srcE = step.getObjTag('e'),
                srcMu = step.getObjTag('m'),
                isoDecisionLabel = cms.string(self.getZZIsoLabel()),
                isoValueLabel = cms.string(self.getZZIsoLabel().replace("Pass", "Val")),
                fsrLabel = cms.string(self.getFSRLabel()),
                fsrElecSelection = cms.string('userFloat("%s") > 0.5'%self.getZZIDLabel()),
                fsrMuonSelection = cms.string('userFloat("%s") > 0.5'%self.getZZIDLabel()),
                eaLabel = cms.string('EffectiveArea'),
                isoConeDRMaxE = cms.double(0.3),
                isoConeDRMaxMu = cms.double(0.3),
                isoCutE = cms.double(0.35),
                isoCutMu = cms.double(0.35),
                )
            step.addModule('leptonIsoEmbedding', leptonIsoEmbedding,
                           'e', 'm', e='electrons', m='muons')

        return step

    def getZZIsoLabel(self):
        return 'ZZIsoPass'
