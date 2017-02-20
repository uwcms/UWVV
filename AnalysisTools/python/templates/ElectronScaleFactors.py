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
                               'eleSelectionSF_HZZ_Moriond17.root')
            sfFileGap = path.join(UWVV_BASE_PATH, 'data', 'LeptonScaleFactors',
                                  'eleSelectionSFGap_HZZ_Moriond17.root')

            scaleFactorEmbedder = cms.EDProducer(
                "PATElectronScaleFactorEmbedder",
                src = step.getObjTag('e'),
                fileName = cms.string(sfFile),
                histName = cms.string('EGamma_SF2D'),
                label = cms.string("effScaleFactor"),
                xValue = cms.string('superCluster.eta'),
                yValue = cms.string('pt'),
                useError = cms.bool(True),
                )
            step.addModule('scaleFactorEmbedderE', scaleFactorEmbedder, 'e')

            scaleFactorEmbedderGap = cms.EDProducer(
                "PATElectronScaleFactorEmbedder",
                src = step.getObjTag('e'),
                fileName = cms.string(sfFileGap),
                histName = cms.string('EGamma_SF2D'),
                label = cms.string("effScaleFactorGap"),
                xValue = cms.string('superCluster.eta'),
                yValue = cms.string('pt'),
                useError = cms.bool(True),
                )
            step.addModule('scaleFactorEmbedderEGap', scaleFactorEmbedderGap, 'e')

            trkRecoSFFile = path.join(UWVV_BASE_PATH, 'data', 'LeptonScaleFactors',
                                      'eleRecoSF_HZZ_Moriond17.root')

            gsfTrackRecoScaleFactorEmbedder = cms.EDProducer(
                "PATElectronScaleFactorEmbedder",
                src = step.getObjTag('e'),
                fileName = cms.string(trkRecoSFFile),
                histName = cms.string('EGamma_SF2D'),
                label = cms.string("trkRecoEffScaleFactor"),
                xValue = cms.string('superCluster.eta'),
                yValue = cms.string('pt'),
                useError = cms.bool(True),
                )
            step.addModule('gsfTrackRecoScaleFactorEmbedder',
                           gsfTrackRecoScaleFactorEmbedder, 'e')

            gsfTrackRecoExtraUncertaintyEmbedder = cms.EDProducer(
                "PATElectronExpressionEmbedder",
                src = step.getObjTag('e'),
                labels = cms.untracked.vstring('trkRecoEffScaleFactorExtraError'),
                functions = cms.untracked.vstring('? pt < 20. || pt > 75. ? 0.01 : 0.'),
                )
            step.addModule('gsfTrackRecoExtraUncertaintyEmbedder',
                           gsfTrackRecoExtraUncertaintyEmbedder, 'e')

        return step
