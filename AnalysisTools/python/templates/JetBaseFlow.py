from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

class JetBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
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

            if self.isMC:
                # shift corrections up and down for systematics
                jesShifts = cms.EDProducer(
                    "PATJetEnergyScaleShifter",
                    src = step.getObjTag('j'),
                    )
                step.addModule('jesShifts', jesShifts, 'j_jesUp', 'j_jesDown',
                               j_jesUp='jesUp', j_jesDown='jesDown')

            jetIDEmbedding = cms.EDProducer(
                "PATJetIDEmbedder",
                src = step.getObjTag('j'),
                )
            step.addModule('jetIDEmbedding', jetIDEmbedding, 'j')

            if self.isMC:
                jetIDEmbedding_jesUp = cms.EDProducer(
                    "PATJetIDEmbedder",
                    src = step.getObjTag('j_jesUp'),
                    )
                step.addModule('jetIDEmbeddingJESUp', jetIDEmbedding_jesUp, 'j_jesUp')
                jetIDEmbedding_jesDown = cms.EDProducer(
                    "PATJetIDEmbedder",
                    src = step.getObjTag('j_jesDown'),
                    )
                step.addModule('jetIDEmbeddingJESDown', jetIDEmbedding_jesDown, 'j_jesDown')
                
                
                jetSmearing = cms.EDProducer(
                    "PATJetSmearing",
                    src = step.getObjTag('j'),
                    rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                    systematics = cms.bool(True),
                    )
                step.addModule("jetSmearing", jetSmearing, 'j', 'j_jerUp', 
                               'j_jerDown', j_jerUp='jerUp', j_jerDown='jerDown')
                
                jetSmearing_jesUp = jetSmearing.clone(src = step.getObjTag('j_jesUp'),
                                                      systematics = cms.bool(False))
                step.addModule("jetSmearingJESUp", jetSmearing_jesUp, 'j_jesUp')
                jetSmearing_jesDown = jetSmearing.clone(src = step.getObjTag('j_jesDown'),
                                                      systematics = cms.bool(False))
                step.addModule("jetSmearingJESDown", jetSmearing_jesDown, 'j_jesDown')

        if stepName == 'preselection':
            step.addBasicSelector('j', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')
            if self.isMC:
                step.addBasicSelector('j_jesUp', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')
                step.addBasicSelector('j_jesDown', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')
                step.addBasicSelector('j_jerUp', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')
                step.addBasicSelector('j_jerDown', 'pt>30. && abs(eta) < 4.7 && userFloat("idLoose") > 0.5')

        return step
    





