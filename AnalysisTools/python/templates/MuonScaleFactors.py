from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase
from UWVV.Utilities.helpers import UWVV_BASE_PATH

from os import path

import FWCore.ParameterSet.Config as cms

class MuonScaleFactors(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        super(MuonScaleFactors, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(MuonScaleFactors, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding' and self.isMC:
            
            sfFile = path.join(UWVV_BASE_PATH, 'data', 'LeptonScaleFactors',
                               'muSelectionAndRecoSF_HZZ_Moriond17.root')
            sfName = 'FINAL'

            scaleFactorEmbedder = cms.EDProducer(
                "PATMuonScaleFactorEmbedder",
                src = step.getObjTag('m'),
                fileName = cms.string(sfFile),
                histName = cms.string(sfName),
                label = cms.string("effScaleFactor"),
                xValue = cms.string('eta'),
                yValue = cms.string('pt'),
                )
            step.addModule('scaleFactorEmbedderM', scaleFactorEmbedder, 'm')

            sfErrName = 'ERROR'

            scaleFactorErrorEmbedder = cms.EDProducer(
                "PATMuonScaleFactorEmbedder",
                src = step.getObjTag('m'),
                fileName = cms.string(sfFile),
                histName = cms.string(sfErrName),
                label = cms.string("effScaleFactorError"),
                xValue = cms.string('eta'),
                yValue = cms.string('pt'),
                )
            step.addModule('scaleFactorErrorEmbedderM',
                           scaleFactorErrorEmbedder, 'm')

        return step








