from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow

import FWCore.ParameterSet.Config as cms


class ZZFinalStateBaseFlow(ZPlusXBaseFlow):
    def __init__(self, *args, **kwargs):
        super(ZZFinalStateBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZFinalStateBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding':
            self.addZZCreation(step)
            self.addAlternatePairInfo(step)

        return step


    def addZZCreation(self, step):
        '''
        Add modules to combine Zs into 4l candidates
        '''
        zz4EMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {0}'.format(step.getObjTagString('ze'))),
            roles = cms.vstring('ze1', 'ze2'),
            cut = cms.string('4. < daughter("ze1").masterClone.mass < 120. && '
                             '4. < daughter("ze2").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        zz2E2MuMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {1}'.format(step.getObjTagString('ze'), 
                                                step.getObjTagString('zm'))),
            roles = cms.vstring('ze1', 'zm1'),
            cut = cms.string('4. < daughter("ze1").masterClone.mass < 120. && '
                             '4. < daughter("zm1").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        zz4MuMod = cms.EDProducer(
            'PATCandViewShallowCloneCombiner',
            decay = cms.string('{0} {0}'.format(step.getObjTagString('zm'))),
            roles = cms.vstring('zm1', 'zm2'),
            cut = cms.string('4. < daughter("zm1").masterClone.mass < 120. && '
                             '4. < daughter("zm2").masterClone.mass < 120.'),
            checkCharge = cms.bool(False),
            setPdgId = cms.int32(25),
            )

        step.addModule("zz4ECreation", zz4EMod, 'zz4e')            
        step.addModule("zz2E2MuCreation", zz2E2MuMod, 'zz2e2m')            
        step.addModule("zz4MuCreation", zz4MuMod, 'zz4m')            


    def addAlternatePairInfo(self, step):
        '''
        Add modules to embed alternate lepton pair (e.g. e1+m1) info.
        '''
        alternatePairInfo4E = cms.EDProducer(
            'AlternateDaughterInfoEmbedder',
            src = step.getObjTag('zz4e'),
            names = cms.vstring('e{}'.format(i) for i in range(1,5)),
            )

        alternatePairInfo2E2Mu = cms.EDProducer(
            'AlternateDaughterInfoEmbedder',
            src = step.getObjTag('zz2e2m'),
            names = cms.vstring('e1','e2','m1','m2'),
            )

        alternatePairInfo4Mu = cms.EDProducer(
            'AlternateDaughterInfoEmbedder',
            src = step.getObjTag('zz4m'),
            names = cms.vstring('m{}'.format(i) for i in range(1,5)),
            )

        step.addModule("zz4EAlternatePairs", alternatePairInfo4E, 'zz4e')
        step.addModule("zz2E2MuAlternatePairs", alternatePairInfo2E2Mu, 'zz2e2m')
        step.addModule("zz4MuAlternatePairs", alternatePairInfo4Mu, 'zz4m')
