from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class ElectronCalibration(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        if not hasattr(self, 'isSync'):
            self.isSync = self.isMC and kwargs.pop('isSync', False)
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
            
            calibratedPatElectrons = cms.EDProducer(
                "CalibratedPatElectronProducerRun2",
                electrons = step.getObjTag('e'),
                gbrForestName = cms.string("gedelectron_p4combination_25ns"),
                isMC = cms.bool(self.isMC),
                isSynchronization = cms.bool(self.isSync),
                correctionFile = cms.string('EgammaAnalysis/ElectronTools/data/ScalesSmearings/80X_DCS05July_plus_Golden22'),
                )

            step.addModule('calibratedPatElectrons', calibratedPatElectrons, 'e')

        return step


    





