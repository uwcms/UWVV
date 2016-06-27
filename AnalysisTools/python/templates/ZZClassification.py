from UWVV.AnalysisTools.templates.JetQuarkGluonTagging import JetQuarkGluonTagging

import FWCore.ParameterSet.Config as cms


class ZZClassification(JetQuarkGluonTagging):
    def __init__(self, *args, **kwargs):
        super(ZZClassification, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZClassification, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'finalStateEmbedding':
            meEmbedding4e = cms.EDProducer(
                "ZZDiscriminantEmbedderEEEE",
                src = step.getObjTag('eeee'),
                jetSrc = step.getObjTag('j'),
                fsrLabel = cms.string(self.getFSRLabel()),
                qgDiscriminator = cms.string(self.qgLikelihoodLabel()),
                )
            step.addModule('meEmbedding4e', meEmbedding4e, 'eeee')
                
            meEmbedding2e2m = cms.EDProducer(
                "ZZDiscriminantEmbedderEEMM",
                src = step.getObjTag('eemm'),
                jetSrc = step.getObjTag('j'),
                fsrLabel = cms.string(self.getFSRLabel()),
                qgDiscriminator = cms.string(self.qgLikelihoodLabel()),
                )
            step.addModule('meEmbedding2e2m', meEmbedding2e2m, 'eemm')
                
            meEmbedding4m = cms.EDProducer(
                "ZZDiscriminantEmbedderMMMM",
                src = step.getObjTag('mmmm'),
                jetSrc = step.getObjTag('j'),
                fsrLabel = cms.string(self.getFSRLabel()),
                qgDiscriminator = cms.string(self.qgLikelihoodLabel()),
                )
            step.addModule('meEmbedding4m', meEmbedding4m, 'mmmm')
                
        return step

    def getZZIsoLabel(self):
        return 'ZZIsoPass'
