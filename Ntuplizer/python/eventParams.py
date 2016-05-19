import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.Mixins import _ParameterTypeBase


_defaultEventParams = {
    'vtxSrc'          : 'offlineSlimmedPrimaryVertices',
    'eSrc'            : 'slimmedElectrons',
    'mSrc'            : 'slimmedMuons',
    'tSrc'            : 'slimmedTaus',
    'gSrc'            : 'slimmedPhotons',
    'jSrc'            : 'slimmedJets',
    'pfCandSrc'       : 'packedPFCandidates',
    'metSrc'          : 'slimmedMETs',
    'puSrc'           : 'slimmedAddPileupInfo',
    'genEventInfoSrc' : 'generator',
    }


def makeEventParams(flowOutputs, **newParams):
    '''
    Makes a PSet for event info contruction. Defaults above are always there
    unless overridden by something in the flowOutputs (assumed to be in the
    usual format from AnalysisFlowBase or newParams (which takes precedence).
    Anything that isn't already a CMS type is assumed to be an InputTag.
    '''
    params = _defaultEventParams.copy()

    objTypes = set(['e', 'm', 't', 'g', 'j'])
    for fo, tag in flowOutputs.iteritems():
        if fo in objTypes:
            params[fo+'Src'] = tag
        elif fo == 'v':
            params['vtxSrc'] = tag

    params.update(newParams)

    for p in params:
        if not isinstance(params[p], _ParameterTypeBase):
            params[p] = cms.InputTag(params[p])

    return cms.PSet(**params)
    
