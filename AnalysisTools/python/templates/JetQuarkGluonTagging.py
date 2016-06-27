from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class JetQuarkGluonTagging(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(JetQuarkGluonTagging, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(JetQuarkGluonTagging, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary': 
            self.process.load("CondCore.CondDB.CondDB_cfi")

            # Make Q/G tag ValueMap
            QGPoolDBESSource = cms.ESSource(
                "PoolDBESSource",
                self.process.CondDB,
                toGet = cms.VPSet(
                    cms.PSet(
                        record = cms.string('QGLikelihoodRcd'),
                        tag = cms.string('QGLikelihoodObject_v1_AK4PFchs'),
                        label = cms.untracked.string('QGL_AK4PFchs'),
                        ),
                    ),
                )

            QGPoolDBESSource.connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS')

            step.addModule('QGPoolDBESSource', QGPoolDBESSource)

            self.process.es_prefer_qg = cms.ESPrefer('PoolDBESSource', 'QGPoolDBESSource')

            self.process.load('RecoJets.JetProducers.QGTagger_cfi')
            self.process.QGTagger.srcJets = step.getObjTag('j')
            self.process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')
            step.addModule('QGTagger', self.process.QGTagger)

            embedQGLikelihood = cms.EDProducer(
                "PATJetValueMapEmbedder",
                src = step.getObjTag('j'),
                label = cms.string(self.qgLikelihoodLabel()),
                valueSrc = cms.InputTag("QGTagger:qgLikelihood"),
                )
            step.addModule("qgLikelihoodEmbedding", embedQGLikelihood, 'j')            

        return step

    
    def qgLikelihoodLabel(self):
        return "qgLikelihood"

        





