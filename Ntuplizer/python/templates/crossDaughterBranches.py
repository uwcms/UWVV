import FWCore.ParameterSet.Config as cms

from itertools import combinations

from UWVV.Ntuplizer.helpers import dict2PSet
from UWVV.AnalysisTools.helpers import mapObjects


def makeCrossDaughterBranches(channel):
    '''
    Make a PSet of branches for di-object variables outside of Zs, 
    e.g. e1_m2_Mass.
    '''
    objects = mapObjects(channel)

    params = {'floats' : {}, 'bools' : {}}
    for pair in combinations(objects, 2):
        # don't include regular Zs
        if pair[0][0] == pair[1][0] and int(pair[0][1]) % 2 == 1 and int(pair[0][1]) + 1 == int(pair[1][1]):
            continue

        name = '_'.join([pair[0], pair[1], ''])

        params['floats'][name + 'Mass'] = cms.string('userFloat("{}")'.format(name+'Mass'))
        params['floats'][name + 'DR'] = cms.string('userFloat("{}")'.format(name+'DR'))

        params['bools'][name + 'SS'] = cms.string('userFloat("{}")'.format(name+'SS'))

        params['floats'][name + 'MassFSR'] = cms.string('? hasUserFloat("{0}") ? '
                                                        'userFloat("{0}") : -999.'.format(name+'MassFSR'))

    return dict2PSet(params)
