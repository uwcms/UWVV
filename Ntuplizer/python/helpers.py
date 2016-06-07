import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.Mixins import _Parameterizable




def pset2Dict(ps):
    '''
    Turns a PSet into a dict of (copies of) its elements. 
    Acts recursively, such that a PSet in a PSet becomes a dict in a dict.
    '''
    d = ps.parameters_()
    for p in d:
        if isinstance(d[p], _Parameterizable):
            d[p] = pset2Dict(d[p])

    return d

def dict2PSet(d):
    '''
    Opposite of above.
    '''
    for p in d:
        if isinstance(d[p], dict):
            d[p] = dict2PSet(d[p])

    return cms.PSet(**d)

def recursiveMerge(a, b={}, path=[]):
    '''
    Merge dict b into dict a, recursively in the case of nested dicts.
    '''
    for k in b:
        if k in a:
            if isinstance(a[k], dict) and isinstance(b[k], dict):
                recursiveMerge(a[k], b[k], path+[str(k)])
            elif a[k] == b[k]:
                pass
            else:
                raise Exception("Can't merge parameters: {}".format('.'.join(path+[str(k)])))
        else:
            a[k] = b[k]

    return a

def combinePSets(ps, *more):
    '''
    Get a PSet that is a deep copy of one or more PSets. If multiple PSets
    have an item with the same name, the last argument to this function wins.
    '''
    if not more:
        return ps.copy()

    allParams = [pset2Dict(p) for p in [ps]+list(more)]
    params = reduce(recursiveMerge, allParams)

    return dict2PSet(params)

def parseChannels(channels):
    '''
    Take a string or list of strings and return a list of channels.
    '''
    if type(channels) == str:
        channels = channels.lower()
        if channels in ['4l', 'zz']:
            return ['eeee', 'eemm', 'mmmm']
        elif channels in ['3l', 'zl', 'z+l']:
            return ['eee', 'eem', 'emm', 'mmm']
        elif channels in ['z', '2l', 'll']:
            return ['ee', 'mm']
        else:
            chanList = channels.split(',')
            assert all(all(letter in ['e','m','t','g','j'] for letter in ch) and len(ch) <= 4 for ch in chanList),\
                'Invalid channel ' + channels
            return chanList
    else:
        assert hasattr(channels, '__iter__'), 'Channels must be a list or a string'
        out = []
        for ch in channels:
            out += parseChannels(ch)
        return out

_expandedObjNames = {
    'e' : 'E',
    'm' : 'Mu',
    't' : 'Tau',
    'g' : 'Pho',
    'j' : 'Jet',
    }
def expandChannelName(channel):
    return ''.join([_expandedObjNames[ob] for ob in channel.lower()])
