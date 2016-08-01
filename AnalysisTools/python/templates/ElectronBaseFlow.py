from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class ElectronBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ElectronBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ElectronBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preselection':
            step.addBasicSelector('e', 'pt > 5 && abs(eta) < 2.6', 'preselection')

        if stepName == 'embedding':
            self.addElectronEAEmbedding(step)
            self.addElectronRhoEmbedding(step)
            self.addElectronIDCounters(step)

        return step


    def addElectronEAEmbedding(self, step):
        mod = cms.EDProducer(
            'PATElectronEAEmbedder',
            src = step.getObjTag('e'),
            label = cms.string('EffectiveArea'),
            configFile = cms.FileInPath('RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt'),
            )

        step.addModule('electronEAEmbedding', mod, 'e')

    def addElectronRhoEmbedding(self, step):
        mod = cms.EDProducer(
            "PATElectronDoubleEmbedder",
            src = step.getObjTag('e'),
            label = cms.string('rho_fastjet'),
            valueSrc = cms.InputTag("fixedGridRhoFastjetAll"),
            )

        step.addModule('electronRhoEmbedding', mod, 'e')
    
    def addElectronIDCounters(self, step):
        mod = cms.EDProducer(
            "PATElectronCountEmbedder",
            src = step.getObjTag('e'),
            label = cms.string('VIDCBtight'),
            cut = cms.untracked.string('userInt("CBVIDtight")'),
            )

        step.addModule('tightElectronCounter', mod)




