from UWVV.AnalysisTools.ElectronBaseFlow import ElectronBaseFlow

import FWCore.ParameterSet.Config as cms

from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupAllVIDIdsInModule, \
    setupVIDElectronSelection, switchOnVIDElectronIdProducer, \
    DataFormat, setupVIDSelection

class RecomputeElectronID(ElectronBaseFlow):
    def __init__(self, *args, **kwargs):
        super(RecomputeElectronID, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(RecomputeElectronID, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preselection':
            step.addBasicSelector('e', 'pt > 5 && abs(eta) < 2.6', 'preselection')

        if stepName == 'embedding':
            self.addElectronIDEmbedding(step)

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

        





