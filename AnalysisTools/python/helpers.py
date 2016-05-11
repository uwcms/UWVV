



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
