from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase


def createFlow(*bases):
    '''
    Given a list of classes inheriting from AnalysisFlowBase, construct a class
    that inherits from all of them, and does all their things.
    '''
    if not bases:
        return AnalysisFlowBase
    
    try:
        basesOk = all(issubclass(b, AnalysisFlowBase) for b in bases)
    except TypeError:
        print "Error: Non-class object passed as Analysis Flow component"
        raise

    if not basesOk:
        for b in bases:
            if not issubclass(b, AnalysisFlowBase):
                BadClass = b
                break
        raise TypeError("{} does not inherit from AnalysisFlowBase".format(BadClass.__name__))

    if len(bases) == 1:
        return bases[0]

    newClassName = '_'.join(b.__name__ for b in bases)

    C = type(newClassName, bases, {})

    return C
