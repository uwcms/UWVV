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
                               'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
                               setupVIDElectronSelection)
        setupAllVIDIdsInModule(self.process,
                               'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
                               setupVIDElectronSelection)
        setupAllVIDIdsInModule(self.process,
                               'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff',
                               setupVIDElectronSelection)

        self.process.egmGsfElectronIDs.physicsObjectSrc = step.getObjTag('e')
        self.process.electronMVAValueMapProducer.srcMiniAOD = step.getObjTag('e')
        self.process.electronRegressionValueMapProducer.srcMiniAOD = step.getObjTag('e')

        step.addModule('egmGsfElectronIDSequence',
                       self.process.egmGsfElectronIDSequence)

        embedIDs = cms.EDProducer(
            "PATElectronValueMapEmbedder",
            src = step.getObjTag('e'),
            floatLabels = cms.untracked.vstring("MVAIDNonTrig"),
            floatVals = cms.untracked.VInputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16HZZV1Values"),
            boolLabels = cms.untracked.vstring("IsCBVIDTight", "IsCBVIDMedium", "IsCBVIDLoose", "IsCBVIDVeto", "IsCBVIDHLTSafe"),
            boolVals = cms.untracked.VInputTag(
                cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight"),
                cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium"),
                cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose"),
                cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto"),
                cms.InputTag("egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1"),
                )
            )

        step.addModule('electronIDEmbedding', embedIDs, 'e')







