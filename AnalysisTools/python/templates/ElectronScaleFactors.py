from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase
from UWVV.Utilities.helpers import UWVV_BASE_PATH

from os import path

import FWCore.ParameterSet.Config as cms

class ElectronScaleFactors(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        super(ElectronScaleFactors, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ElectronScaleFactors, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding' and self.isMC:

            sfFile = path.join(UWVV_BASE_PATH, 'data', 'LeptonScaleFactors',
                               'ele_scale_factors_v1.root')

            scaleFactorEmbedder = cms.EDProducer(
                "PATElectronScaleFactorEmbedder",
                src = step.getObjTag('e'),
                fileName = cms.string(sfFile),
                histName = cms.string('ele_scale_factors'),
                label = cms.string("effScaleFactor"),
                xValue = cms.string('abs(eta)'),
                yValue = cms.string('pt'),
                )
            step.addModule('scaleFactorEmbedderE', scaleFactorEmbedder, 'e')

            scaleFactorEmbedderGap = cms.EDProducer(
                "PATElectronScaleFactorEmbedder",
                src = step.getObjTag('e'),
                fileName = cms.string(sfFile),
                histName = cms.string('ele_scale_factors_gap'),
                label = cms.string("effScaleFactorGap"),
                xValue = cms.string('abs(eta)'),
                yValue = cms.string('pt'),
                )
            step.addModule('scaleFactorEmbedderEGap', scaleFactorEmbedderGap, 'e')

        return step
