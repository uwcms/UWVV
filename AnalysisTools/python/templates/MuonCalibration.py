from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class MuonCalibration(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        if not hasattr(self, 'isSync'):
            self.isSync = self.isMC and kwargs.pop('isSync', False)
        if not hasattr(self, 'muonClosureShift'):
            self.muonClosureShift = kwargs.pop('muonClosureShift', 0) if self.isMC else 0
        super(MuonCalibration, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(MuonCalibration, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary':
            if self.isMC:
                calibType = 'MC_80X_13TeV'
            else:
                calibType = 'DATA_80X_13TeV'

            muCalibrator = cms.EDProducer(
                "PATMuonKalmanCorrector",
                src = step.getObjTag('m'),
                calibType = cms.string(calibType),
                isMC = cms.bool(self.isMC),
                isSync = cms.bool(self.isSync),
                maxPt = cms.double(200),
                closureShift = cms.int32(self.muonClosureShift),
                )

            step.addModule('calibratedPatMuons', muCalibrator, 'm')

            # need to re-sort now that we're calibrated
            mSort = cms.EDProducer(
                "PATMuonCollectionSorter",
                src = step.getObjTag('m'),
                function = cms.string('pt'),
                )
            step.addModule('muonSorting', mSort, 'm')

        return step








