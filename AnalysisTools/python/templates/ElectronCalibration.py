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
            from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
            self.process = regressionWeights(self.process)

            from EgammaAnalysis.ElectronTools.regressionApplication_cff import slimmedElectrons as eReg

            eReg.src = step.getObjTag('e')
            step.addModule('electronRegression', eReg, 'e')

            cutOnSCEta = cms.EDFilter(
                "PATElectronSelector",
                src = step.getObjTag('e'),
                cut = cms.string('pt >= 5. && abs(superCluster.eta) < 2.5'),
                )
            step.addModule('selectElectronsBeforeID', cutOnSCEta, 'e')

            if not hasattr(self.process, 'RandomNumberGeneratorService'):
                self.process.RandomNumberGeneratorService = cms.Service(
                    'RandomNumberGeneratorService',
                    )
            self.process.RandomNumberGeneratorService.calibratedPatElectrons = cms.PSet(
                initialSeed = cms.untracked.uint32(987),
                )

            correctionFile = 'EgammaAnalysis/ElectronTools/data/ScalesSmearings/Moriond17_23Jan_ele'

            calibratedPatElectrons = cms.EDProducer(
                "CalibratedPatElectronProducerRun2",
                electrons = step.getObjTag('e'),
                gbrForestName = cms.vstring('electron_eb_ECALTRK_lowpt',
                                            'electron_eb_ECALTRK',
                                            'electron_ee_ECALTRK_lowpt',
                                            'electron_ee_ECALTRK',
                                            'electron_eb_ECALTRK_lowpt_var',
                                            'electron_eb_ECALTRK_var',
                                            'electron_ee_ECALTRK_lowpt_var',
                                            'electron_ee_ECALTRK_var'),
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

        if stepName == 'selection':
            # need to re-sort now that we're calibrated
            eSort = cms.EDProducer(
                "PATElectronCollectionSorter",
                src = step.getObjTag('e'),
                function = cms.string('pt'),
                )
            step.addModule('electronSorting', eSort, 'e')

        return step
