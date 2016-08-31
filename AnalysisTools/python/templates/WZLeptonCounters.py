from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

from UWVV.Utilities.helpers import parseChannels

import FWCore.ParameterSet.Config as cms


class WZLeptonCounters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(WZLeptonCounters, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZLeptonCounters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            muCounters = {"tightMuonCounter" : "TightMuon",
                          "medMuonCounter" : "MediumMuonICHEP",
                          }
            for moduleName, cut in muCounters.iteritems():
                mod = cms.EDProducer(
                    "PATMuonCounter",
                    src = step.getObjTag('m'),
                    label = cms.string(cut),
                    cut = cms.untracked.string('userInt("is%s")' % cut),
                    )
                step.addModule(moduleName, mod)

            eCounters = {"tightElectronCounter" : "CBVIDtight",
                         "medElectronCounter" : "CBVIDmedium",
                         "looseElectronCounter" :"CBVIDloose",
                         }
            for moduleName, cut in eCounters.iteritems():
                mod = cms.EDProducer(
                    "PATElectronCounter",
                    src = step.getObjTag('e'),
                    label = cms.string(cut),
                    cut = cms.untracked.string('userFloat("is%s") > 0.5' % cut),
                    )
                step.addModule(moduleName, mod)

            counters = {'n'+label : name+':'+label for name, label in muCounters.iteritems()}
            counters.update({'n'+label+'Elec' : name+':'+label for name, label in eCounters.iteritems()})

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

