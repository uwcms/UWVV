from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

from UWVV.Utilities.helpers import parseChannels

import FWCore.ParameterSet.Config as cms


class WZLeptonCounters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(WZLeptonCounters, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZLeptonCounters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            muCounters = ["TightMuon", "MediumMuonICHEP",]
                         
            mod = cms.EDProducer(
                "PATMuonCounter",
                src = step.getObjTag('m'),
                labels = cms.vstring(*[label for label in muCounters]),
                cuts = cms.vstring(*['userInt("is%s")' % label for label in muCounters]),
                )
            step.addModule("muCounter", mod)

            eCounters = ["CBVIDtight", "CBVIDmedium","CBVIDloose",]
                         
            mod = cms.EDProducer(
                "PATElectronCounter",
                src = step.getObjTag('e'),
                labels = cms.vstring(*[label for label in eCounters]),
                cuts = cms.vstring(*['userFloat("is%s") > 0.5' % label for label in eCounters]),
                )
            step.addModule("elecCounter", mod)

            counters = {'n'+label : 'muCounter:'+label for label in muCounters}
            counters.update({'n'+label+'Elec' : 'elecCounter:'+label for label in eCounters})

            labels = list(counters.keys())
            tags = [cms.InputTag(counters[label]) for label in labels]

            for chan in parseChannels('zl'):
                countEmbedding = cms.EDProducer(
                    'PATCompositeCandidateValueEmbedder',
                    src = step.getObjTag(chan),
                    intLabels = cms.vstring(*labels),
                    intSrc = cms.VInputTag(*tags),
                    )
                step.addModule(chan+'CountEmbedding', countEmbedding, chan)

        return step

