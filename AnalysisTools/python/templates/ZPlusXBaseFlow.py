from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class ZPlusXBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ZPlusXBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZPlusXBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'intermediateStateCreation':
            self.addZCreation(step)

        return step


    def addZCreation(self, step):
        '''
        Add modules to make Z candidates
        '''
        zEEMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0}@+ {0}@-'.format(step.getObjTagString('e'))),
            roles = cms.vstring('e1', 'e2'),
            cut = cms.string('daughter("e1").pt > 7 && '
                             'daughter("e2").pt > 7 && '
                             'abs(daughter("e1").eta) < 2.5 && '
                             'abs(daughter("e2").eta) < 2.5'),
            checkCharge = cms.bool(True),
            setPdgId = cms.int32(23),
            )

        zMuMuMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0}@+ {0}@-'.format(step.getObjTagString('m'))),
            roles = cms.vstring('m1', 'm2'),
            cut = cms.string('daughter("m1").masterClone.pt > 5 && '
                             'daughter("m2").masterClone.pt > 5 && '
                             'abs(daughter("m1").masterClone.eta) < 2.4 && '
                             'abs(daughter("m2").masterClone.eta) < 2.4'),
            checkCharge = cms.bool(True),
            setPdgId = cms.int32(23),
            )

        step.addModule("zEECreation", zEEMod, 'ze')
        step.addModule("zMuMuCreation", zMuMuMod, 'zm')
