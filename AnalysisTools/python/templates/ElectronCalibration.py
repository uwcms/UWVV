from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class ElectronCalibration(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        if not hasattr(self, 'isSync'):
            self.isSync = self.isMC and kwargs.pop('isSync', False)

        eesShift = kwargs.pop('electronScaleShift', 0) if self.isMC else 0
        eerRhoShift = kwargs.pop('electronRhoResShift', 0) if self.isMC else 0
        eerPhiShift = kwargs.pop('electronPhiResShift', 0) if self.isMC else 0
        if not hasattr(self, 'electronScaleShift'):
            self.electronScaleShift = eesShift
        if not hasattr(self, 'electronRhoResShift'):
            self.electronRhoResShift = eerRhoShift
        if not hasattr(self, 'electronPhiResShift'):
            self.electronPhiResShift = eerPhiShift

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

            correctionFile = 'EgammaAnalysis/ElectronTools/data/ScalesSmearings/Winter_2016_reReco_v1_ele'

            calibratedPatElectrons = cms.EDProducer(
                "CalibratedPatElectronProducerRun2",
                electrons = step.getObjTag('e'),
                gbrForestName = cms.string("gedelectron_p4combination_25ns"),
                isMC = cms.bool(self.isMC),
                isSynchronization = cms.bool(self.isSync),
                correctionFile = cms.string(correctionFile),
                )

            step.addModule('calibratedPatElectrons', calibratedPatElectrons, 'e')

            if self.electronScaleShift or self.electronRhoResShift or self.electronPhiResShift:
                self.process.RandomNumberGeneratorService.electronSystematicShift = cms.PSet(
                    initialSeed = cms.untracked.uint32(345),
                    )

                shiftMod = cms.EDProducer(
                    "PATElectronSystematicShifter",
                    src = step.getObjTag('e'),
                    correctionFile = cms.string(correctionFile),
                    scaleShift = cms.double(self.electronScaleShift),
                    rhoResShift = cms.double(self.electronRhoResShift),
                    phiResShift = cms.double(self.electronPhiResShift),
                    )

                step.addModule('electronSystematicShift', shiftMod, 'e')

        return step
