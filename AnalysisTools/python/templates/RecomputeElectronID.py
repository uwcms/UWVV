from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

from PhysicsTools.SelectorUtils.tools.vid_id_tools import setupAllVIDIdsInModule, \
    setupVIDElectronSelection, switchOnVIDElectronIdProducer, \
    DataFormat, setupVIDSelection

class RecomputeElectronID(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(RecomputeElectronID, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(RecomputeElectronID, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary':
            self.addElectronIDEmbedding(step)

        return step


    def addElectronIDEmbedding(self, step):
        #if not hasattr(self.process, 'egmGsfElectronIDs'):
        switchOnVIDElectronIdProducer(self.process, DataFormat.MiniAOD)

        setupAllVIDIdsInModule(self.process, 
                               'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_V1_cff',
                               setupVIDElectronSelection)

        step.addModule('egmGsfElectronIDSequence', 
                       self.process.egmGsfElectronIDSequence)

        embedIDs = cms.EDProducer(
            "PATElectronValueMapEmbedder",
            src = step.getObjTag('e'),
            label = cms.string("MVAIDNonTrig"),
            valueSrc = cms.InputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16V1Values"),
            )

        step.addModule('electronMVAIDEmbedding', embedIDs, 'e')

        





