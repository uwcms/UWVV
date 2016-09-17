from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

from UWVV.Utilities.helpers import parseChannels, getObjName

import FWCore.ParameterSet.Config as cms


class ZZLeptonCounters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ZZLeptonCounters, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZLeptonCounters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            counters = {
                'Loose' : 'userFloat("{}") > 0.5'.format(self.getZZIDLabel()),
                'Tight' : 'userFloat("{}") > 0.5'.format(self.getZZIDLabel()+'Tight'),
                'TightIso' : ('userFloat("{}") > 0.5 && '
                              'userFloat("{}") > 0.5').format(self.getZZIDLabel()+'Tight', 
                                                              self.getZZIsoLabel())
                }
            countTags = []
            countLabels = []
            for lep in 'e','m':
                for name, cut in counters.iteritems():
                    label = 'nZZ'+name+getObjName(lep, True)+'s'
                    countLabels.append(label)
                    mod = cms.EDProducer(
                        "PAT{}Counter".format(getObjName(lep, True)),
                        src = step.getObjTag(lep),
                        labels = cms.vstring(label),
                        cuts = cms.vstring(cut),
                        )
                    step.addModule(label, mod)

                    countTags.append(cms.InputTag('{0}:{0}'.format(label)))

            for chan in parseChannels('zz')+parseChannels('zl')+parseChannels('z'):
                if chan not in step.outputs:
                    continue
                countEmbedding = cms.EDProducer(
                    'PATCompositeCandidateValueEmbedder',
                    src = step.getObjTag(chan),
                    intLabels = cms.vstring(*countLabels),
                    intSrc = cms.VInputTag(*countTags),
                    )
                step.addModule(chan+'ZZCountEmbedding', countEmbedding, chan)

        return step

