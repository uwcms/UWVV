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

_zzhelpers_object_maps_ = {}
def mapObjects(channel):
    '''
    Return a list of objects of the form ['e1','e2','m1','m2'] or ['e1','e2','m']
    Objects are in alphabetical/numerical order order
    '''
    global _zzhelpers_object_maps_

    try:
        return _zzhelpers_object_maps_[channel]
    except KeyError:
        nObjects = {}
        objects = []
    
        for obj in channel:
            if obj not in nObjects:
                nObjects[obj] = 1
            else:
                nObjects[obj] += 1
    
        for obj, num in nObjects.iteritems():
            if num == 1:
                objects.append(obj)
            else:
                for i in range(num):
                    objects.append(obj+str(i+1))
        
        objects.sort()

        _zzhelpers_object_maps_[channel] = objects
        return objects


