from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow
from UWVV.Utilities.helpers import mapObjects, parseChannels

import FWCore.ParameterSet.Config as cms


class ZPlusXFinalStateBaseFlow(ZPlusXBaseFlow):
    def __init__(self, *args, **kwargs):
        super(ZPlusXFinalStateBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZPlusXFinalStateBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'finalStateCreation':
            self.addZPlusXCreation(step)

        if stepName == 'finalStateEmbedding':
            self.addAlternatePairInfo(step)

        return step


    def addZPlusXCreation(self, step):
        '''
        Add modules to combine Zs into 3l candidates
        '''
        for chan in parseChannels('zl'):
            #z1Name = 'z{}1'.format(chan[0])
            #z2Name = 'z{}{}'.format(chan[2], 2 if chan[0] == chan[2] else 1)
            mod = cms.EDProducer(
                'PATCandViewShallowCloneCombiner',
                decay = cms.string('{0} {1}'.format(step.getObjTagString('ee' if chan.count('e') > 1 else 'mm'),
                                                    step.getObjTagString('e' if chan.count('e') in [1,3] else 'm'))),
                #roles = cms.vstring(z1Name, z2Name),
                cut = cms.string(""),
                #    ('daughter("{}").masterClone.mass < 150. && '
                #                  'daughter("{}").masterClone.mass < 150.').format(z1Name, z2Name)),
                checkCharge = cms.bool(False),
                #setPdgId = cms.int32(25),
                )
            
            step.addModule(chan+'Producer', mod, chan)


    def addAlternatePairInfo(self, step):
        '''
        Add modules to embed alternate lepton pair (e.g. e1+m1) info.
        '''
        for chan in parseChannels('zl'):
            mod = cms.EDProducer(
                'AlternateDaughterInfoEmbedder',
                src = step.getObjTag(chan),
                names = cms.vstring(*mapObjects(chan)),
                fsrLabel = cms.string(""),#"fsr"),
                )
            step.addModule(chan+'AlternatePairs', mod, chan)
