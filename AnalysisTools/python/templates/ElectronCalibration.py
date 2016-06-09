from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class ElectronCalibration(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.get('isMC', True)
        if not hasattr(self, 'isSync'):
            self.isSync = self.isMC and kwargs.get('isSync', False)
        super(ElectronCalibration, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ElectronCalibration, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary':
            if not hasattr(self.process, 'RandomNumberGeneratorService'):
                self.process.RandomNumberGeneratorService = cms.Service(
                    'RandomNumberGeneratorService',
                    )
            self.process.RandomNumberGeneratorService.calibratedPatElectrons = cms.PSet(
                initialSeed = cms.untracked.uint32(987),
                )
            
            self.process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi')

            self.process.calibratedPatElectrons.electrons = step.getObjTag('e')
            self.process.calibratedPatElectrons.isMC = cms.bool(self.isMC)
            self.process.calibratedPatElectrons.isSynchronization = cms.bool(self.isSync)

            step.addModule('calibratedPatElectrons', 
                           self.process.calibratedPatElectrons,
                           'e')

        return step


    





