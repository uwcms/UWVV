



_objectList = {
    'e' : 'electron',
    'm' : 'muon',
    't' : 'tau',
    'g' : 'photon',
    'j' : 'jet',
}

def getObjTypes():
    return _objectList.keys()

def getObjName(obj, capitalize=False):
    '''
    Get the spelled out name of the object type ('electron' for 'e' etc.).
    '''
    name = _objectList[obj[0]]
    if capitalize:
        name = name[0].upper() + name[1:]
    return name

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
