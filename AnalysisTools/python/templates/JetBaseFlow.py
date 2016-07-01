from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

class JetBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.get('isMC', True)
        super(JetBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(JetBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary':
            # Jet energy corrections
            corrections = ['L1FastJet', 'L2Relative', 'L3Absolute',]
            if not self.isMC:
                corrections.append('L2L3Residual')
            updateJetCollection(
                self.process,
                jetSource = step.getObjTag('j'),
                labelName = 'UpdatedJEC',
                jetCorrections = ('AK4PFchs', cms.vstring(corrections), 'None'),
                )
            self.process.jecSequence = cms.Sequence(
                self.process.patJetCorrFactorsUpdatedJEC
                * self.process.updatedPatJetsUpdatedJEC
                )
            step.addModule('jecSequence', 
                           self.process.jecSequence,
                           'j')

            jetIDEmbedding = cms.EDProducer(
                "PATJetIDEmbedder",
                src = step.getObjTag('j'),
                )
            step.addModule('jetIDEmbedding', jetIDEmbedding, 'j')

        if stepName == 'preselection':
            step.addBasicSelector('j', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')

        return step
    





