from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

from UWVV.Utilities.helpers import parseChannels

import FWCore.ParameterSet.Config as cms


class WZLeptonCounters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(WZLeptonCounters, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZLeptonCounters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            muCounters = {"TightMuon" : 'pt() > 10 && userInt("isTightMuon")',
                    "MediumMuonICHEP" : 'pt() > 10 && userInt("isMediumMuonICHEP")',
                    "WZMediumMuon"   : self.getWZMediumMuonID(),
                    "WZTightMuon"   : self.getWZMediumMuonID(),
            }
                         
            mod = cms.EDProducer(
                "PATMuonCounter",
                src = step.getObjTag('m'),
                labels = cms.vstring(*muCounters.keys()),
                cuts = cms.vstring(*muCounters.values()),
                )
            step.addModule("muCounter", mod)

            eCounters = {"CBVIDtightElec" : 'pt() > 10 && userFloat("isCBVIDtight")',
                "CBVInDmediumElec" :  'pt() > 10 && userFloat("isCBVIDmedium")',
                "CBVIDlooseElec" :  'pt() > 10 && userFloat("isCBVIDloose")',
            }
                         
            mod = cms.EDProducer(
                "PATElectronCounter",
                src = step.getObjTag('e'),
                labels = cms.vstring(*eCounters.keys()),
                cuts = cms.vstring(*eCounters.values()),
                )
            step.addModule("elecCounter", mod)

            counters = {'n'+label : 'muCounter:'+label for label in muCounters.keys()}
            counters.update({'n'+label : 'elecCounter:'+label for label in eCounters.keys()})

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

