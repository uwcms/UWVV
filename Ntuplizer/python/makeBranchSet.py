import FWCore.ParameterSet.Config as cms

from UWVV.Ntuplizer.templates.eventBranches import eventBranches
from UWVV.Ntuplizer.templates.initialStateBranches import initialStateBranches, kinFitBranches
from UWVV.Ntuplizer.templates.zBranches import zBranches
from UWVV.Ntuplizer.templates.objectBranches import objectBranches
from UWVV.Ntuplizer.templates.leptonBranches import leptonBranches
from UWVV.Ntuplizer.templates.electronBranches import electronBranches
from UWVV.Ntuplizer.templates.muonBranches import muonBranches
from UWVV.Ntuplizer.templates.zzDiscriminantBranches import zzDiscriminantBranches
from UWVV.Ntuplizer.templates.crossDaughterBranches import makeCrossDaughterBranches

from UWVV.Utilities.helpers import pset2Dict, dict2PSet, \
    recursiveMerge, combinePSets, mapObjects




def makeLepBranchSet(lep):
    branches = [objectBranches, leptonBranches]
    
    if lep[0] == 'e':
        branches.append(electronBranches)
    if lep[0] == 'm':
        branches.append(muonBranches)

    return combinePSets(*branches)

def makeZBranchSet(lep, n, addName=False):
    '''
    lep: 'e' or 'm'
    n: 1 or 2 (so we know whether to make, e.g., e1 and e2 or e3 and e4)
    addNameToZBranches: workaround for 2l final states: put 'e1_e2' 
    (or whatever) at the start of the branch names so these branches can have
    the necessary names while the object stays nameless for event variables.
    '''
    branchSet = combinePSets(objectBranches, zBranches)

    daughterNames = [lep[0]+str(i) for i in [n*2-1,n*2]]

    if addName:
        renamedBranches = {}
        for bName, b in branchSet.parameters_().iteritems():
            renamedBranches['_'.join(daughterNames+[bName])] = b
        branchSet = cms.PSet(**renamedBranches)

    daughterBranchSet = makeLepBranchSet(lep)
    branchSet.daughterParams = cms.VPSet(
        daughterBranchSet,
        daughterBranchSet.clone(),
        )
    branchSet.daughterNames = cms.vstring(*daughterNames)

    return branchSet

def makeBranchSet(channel):
    branches = [eventBranches, initialStateBranches, objectBranches]

    finalObjects = mapObjects(channel)

    if len(channel) == 2: # single Z
        assert channel[0] == channel[1], '{} does not make a valid Z'.format(channel)
        branches.append(makeZBranchSet(channel[0], 1, False))
    else:
        branches.append(objectBranches)
        
    if len(channel) == 3: # Z+l
        if channel[0] != channel[1]: # emm -> mme
            assert channel[1] == channel[2], "Invalid channel {}".format(channel)
            channel = channel[1:] + channel[0]
            finalObjects = finalObjects[1:] + finalObjects[:1]

        assert channel[0] == channel[1], "Invalid channel {}".format(channel)

        branches.append(makeCrossDaughterBranches(channel))

        daughterSets = [makeZBranchSet(channel[0], 1), makeLepBranchSet(channel[2])]
        daughterNames = [
            '_'.join(finalObjects[:2]+['']),
            finalObjects[2],
            ]

    elif len(channel) == 4:
        assert channel[0] == channel[1] and channel[2] == channel[3], \
            'Invalid channel {}'.format(channel)

        branches.append(makeCrossDaughterBranches(channel))

        branches.append(zzDiscriminantBranches)

        branches.append(kinFitBranches)

        # include FSR when ordering Zs
        branches.append(cms.PSet(fsrLabel=cms.string("fsr")))
            
        daughterSets = [
            makeZBranchSet(channel[0], 1),
            makeZBranchSet(channel[2], int(finalObjects[3][1])/2),
            ]
        daughterNames = [
            '_'.join(finalObjects[:2]+['']),
            '_'.join(finalObjects[2:]+['']),
            ]

    else:
        daughterSets = []
        daughterNames = []
        
    branchSet = combinePSets(*branches)

    if daughterSets:
        branchSet.daughterParams = cms.VPSet(*daughterSets)
    if daughterNames:
        branchSet.daughterNames = cms.vstring(*daughterNames)

    return branchSet



