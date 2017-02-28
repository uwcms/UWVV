from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase
from UWVV.Utilities.helpers import parseChannels
import FWCore.ParameterSet.Config as cms

# From https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
class AddMetFilters(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(AddMetFilters, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(AddMetFilters, self).makeAnalysisStep(stepName, **inputs)
        
        if stepName == 'initialStateEmbedding':
            from RecoMET.METFilters.BadPFMuonFilter_cfi import BadPFMuonFilter 
            BadPFMuonFilter.muons = step.getObjTag('m') 
            BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
            step.addModule("BadPFMuonFilter", BadPFMuonFilter)
            
            from RecoMET.METFilters.BadChargedCandidateFilter_cfi import BadChargedCandidateFilter
            BadChargedCandidateFilter.muons = step.getObjTag('m') 
            BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
            step.addModule("BadChargedCandidateFilter", BadChargedCandidateFilter)

            for chan in parseChannels('wz'):
                filterEmbedding = cms.EDProducer(
                    'PATCompositeCandidateValueEmbedder',
                    src = step.getObjTag(chan),
                    boolLabels = cms.vstring("Flag_BadPFMuonFilterPass", "Flag_BadChargedCandidateFilterPass"),
                    boolSrc = cms.VInputTag("BadPFMuonFilter", "BadChargedCandidateFilter"),
                    )
                step.addModule(chan+'filterEmbedding', filterEmbedding, chan)

        return step


