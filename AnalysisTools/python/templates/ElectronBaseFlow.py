from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupAllVIDIdsInModule, \
    setupVIDElectronSelection, switchOnVIDElectronIdProducer, \
    DataFormat, setupVIDSelection

class ElectronBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ElectronBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ElectronBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preselection':
            step.addBasicSelector('e', 'pt > 7 && abs(eta) < 2.6', 'preselection')

        if stepName == 'embedding':
            self.addElectronIDEmbedding(step)
            self.addElectronEAEmbedding(step)
            self.addElectronRhoEmbedding(step)

        return step


    def addElectronIDEmbedding(self, step):
        #if not hasattr(self.process, 'egmGsfElectronIDs'):
        switchOnVIDElectronIdProducer(self.process, DataFormat.MiniAOD)

        setupAllVIDIdsInModule(self.process, 
                               'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring15_25ns_nonTrig_V1_cff',
                               setupVIDElectronSelection)

        step.addModule('egmGsfElectronIDSequence', 
                       self.process.egmGsfElectronIDSequence)

        embedIDs = cms.EDProducer(
            "PATElectronValueMapEmbedder",
            src = step.getObjTag('e'),
            label = cms.string("MVAIDNonTrig"),
            valueSrc = cms.InputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring15NonTrig25nsV1Values"),
            )

        step.addModule('electronMVAIDEmbedding', embedIDs, 'e')

    
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
    





