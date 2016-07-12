from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class JetEnergySmearing(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.get('isMC', True)
        super(JetEnergySmearing, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(JetEnergySmearing, self).makeAnalysisStep(stepName, **inputs)

        if self.isMC and stepName == 'preliminary': 
            jetSmearing = cms.EDProducer(
                "PATJetSmearing",
                src = step.getObjTag('j'),
                rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                )
            step.addModule("jetSmearing", jetSmearing, 'j')            

        return step

    
    def qgLikelihoodLabel(self):
        return "qgLikelihood"

        





