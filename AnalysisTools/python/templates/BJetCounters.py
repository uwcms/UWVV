from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

from UWVV.Utilities.helpers import parseChannels

import FWCore.ParameterSet.Config as cms


class BJetCounters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(BJetCounters, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        step = super(BJetCounters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            # Recommendations via https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
            jetCounters = {
                "CSVv2L" : '? bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.5426 ? 1 : 0',
                "CSVv2M" : '? bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.8484 ? 1 : 0',
                "CSVv2T" : '? bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.9535 ? 1 : 0',
                "CMVAv2L" : '? bDiscriminator("pfCombinedMVAV2BJetTags") > -0.5884 ? 1 : 0',
                "CMVAv2M" : '? bDiscriminator("pfCombinedMVAV2BJetTags") > 0.4432 ? 1 : 0',
                "CMVAv2T" : '? bDiscriminator("pfCombinedMVAV2BJetTags") > 0.9432 ? 1 : 0',
                #https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
                "DeepCSV_loose" : '? bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") > 0.1522 ? 1 : 0',  #loose
                "DeepCSV_med" : '? bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") > 0.4941 ? 1 : 0',  #medium
                "DeepCSV_tight" : '? bDiscriminator("pfDeepCSVDiscriminatorsJetTags:BvsAll") > 0.8001 ? 1 : 0',  #tight
            }
            mod = cms.EDProducer(
                "PATJetCounter",
                src = step.getObjTag('j'),
                labels = cms.vstring(*['nJet'+label for label in jetCounters.keys()]),
                cuts = cms.vstring(*[jetCounters[key] for key in jetCounters.keys()])
                )
            step.addModule("jetCounter", mod)

            labels = ['nJet'+name for name in jetCounters.keys()]
            tags = [cms.InputTag("jetCounter:nJet"+name) for name in jetCounters.keys()]

            for chan in parseChannels('zl'):
                countEmbedding = cms.EDProducer(
                    'PATCompositeCandidateValueEmbedder',
                    src = step.getObjTag(chan),
                    intLabels = cms.vstring(*labels),
                    intSrc = cms.VInputTag(*tags),
                    )
                step.addModule(chan+'JetCountEmbedding', countEmbedding, chan)

        return step

