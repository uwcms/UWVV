from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow

import FWCore.ParameterSet.Config as cms


class ZZFinalStateBaseFlow(ZPlusXBaseFlow):
    def __init__(self, *args, **kwargs):
        super(ZZFinalStateBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZFinalStateBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding':
            self.addZZCreation(step)

        return step


    def addZZCreation(self, step):
        '''
        Add modules to combine Zs into 4l candidates
        '''
        zz4EMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {0}'.format(step.getObjTag('ze'))),
            roles = cms.vstring('ze1', 'ze2'),
            cut = cms.string('4. < daughter("ze1").masterClone.mass < 120. && '
                             '4. < daughter("ze2").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        zz2E2MuMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {1}'.format(step.getObjTag('ze'), 
                                                step.getObjTag('zm'))),
            roles = cms.vstring('ze1', 'zm1'),
            cut = cms.string('4. < daughter("ze1").masterClone.mass < 120. && '
                             '4. < daughter("zm1").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        zz4MuMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {0}'.format(step.getObjTag('zm'))),
            roles = cms.vstring('zm1', 'zm2'),
            cut = cms.string('4. < daughter("zm1").masterClone.mass < 120. && '
                             '4. < daughter("zm2").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        step.addModule("zz4ECreation", zz4EMod, 'zz4e')            
        step.addModule("zz2E2MuCreation", zz2E2MuMod, 'zz2e2m')            
        step.addModule("zz4MuCreation", zz4MuMod, 'zz4m')            
