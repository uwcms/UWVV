# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
from rootpy.ROOT import gROOT
gROOT.SetBatch(True)
sys.argv = oldargv

import logging
from rootpy import log as rlog; rlog = rlog["/muonTrackSharingStudy"]
logging.basicConfig(level=logging.WARNING)
rlog["/ROOT.TCanvas.Print"].setLevel(rlog.WARNING)
rlog["/ROOT.TUnixSystem.SetDisplay"].setLevel(rlog.ERROR)

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

from UWVV.Utilities.helpers import deltaR

from rootpy.plotting import Hist, Hist2D, Graph, Canvas, Legend
from rootpy.plotting.utils import draw
from rootpy import asrootpy

#from multiprocessing import cpu_count, Pool, Manager
#import signal
from glob import glob

from UWVV.Utilities.PlotStyle import PlotStyle



style = PlotStyle()


class PFCandHolder(object):
    '''
    Class to hold PF cands and sub-collections of them.
    '''
    def __init__(self, handle):
        self.handle = handle

        self.chHadCands = []
        self.nHadCands = []
        self.phoCands = []

    def getChargedHadrons(self):
        if not self.chHadCands:
            for p in self.handle.product():
                if abs(p.pdgId()) == 211:
                    self.chHadCands.append(p)

        return self.chHadCands

    def getNeutralHadrons(self):
        if not self.nHadCands:
            for p in self.handle.product():
                if abs(p.pdgId()) == 130:
                    self.nHadCands.append(p)

        return self.nHadCands

    def getPhotons(self):
        if not self.phoCands:
            for p in self.handle.product():
                if abs(p.pdgId()) == 22:
                    self.phoCands.append(p)

        return self.phoCands


class CandMatchChecker(object):
    '''
    Base class to check the difference between PF and TuneP muon pt
    '''
    idLabel = "ZZIDPassTight"
    isoLabel = "ZZIsoPass"
    vtxCollection = 'vertexCleaning'
    pfCandCollection = 'packedPFCandidates'
    
    def __init__(self, name, files):
        if isinstance(files, str):
            self.files = files.split(',')
        else:
            self.files = files
            
        self.name = name
        
        self.hists = {
            'pfCandPt' : Hist(50,0.,500.),
            'pfCandPtVsGenPt' : Hist2D(50,0.,1000.,40,0.,200.),
            }

        self.nTot = 0
        self.nFailPF = 0
        self.nPFMatched = 0
            
    @classmethod
    def isGoodLepton(cls, lep):
        '''
        Is this lepton gen matched and does it pass ID and a high-pt cut?
        '''
        return cls.goodGen(lep) and \
            bool(lep.userFloat(cls.idLabel))
            
    @classmethod
    def goodGen(cls, lep):
        '''
        Matched to a good gen lepton?
        '''
        if lep.genParticleRef().isNull():
            return False

        gp = lep.genParticleRef()

        return gp.fromHardProcessFinalState() and \
            abs(gp.pdgId()) == cls.pdgId and \
            abs(gp.eta()) < cls.etaCut and gp.pt() > cls.ptCut

    def bookLepton(self, lep, pfCands):
        if not self.__class__.isGoodLepton(lep):
            return

        self.nTot += 1

        if self.__class__.isPF(lep):
            return

        self.nFailPF += 1
            
        for c in pfCands.getChargedHadrons():
            if deltaR(c.pseudoTrack().eta(), c.pseudoTrack().phi(),
                      lep.innerTrack().eta(), lep.innerTrack().phi()) < 0.0001:
                self.nPFMatched += 1
                self.hists['pfCandPt'].fill(c.pt())
                self.hists['pfCandPtVsGenPt'].fill(lep.genParticleRef().pt(), c.pt())

    def processSample(self):
        self.nTot = 0
        self.nFailPF = 0
        self.nPFMatched = 0

        print self.files
        events = Events(self.files)

        vertices = Handle("std::vector<reco::Vertex>")
        leps = Handle("std::vector<{}>".format(self.__class__.lepTypeStr))
        pfCands = Handle("std::vector<pat::PackedCandidate>")

        for iEv, ev in enumerate(events):
            if iEv % 5000 == 0:
                print "{} processing event {}".format(self.name, iEv)

            ev.getByLabel(self.__class__.vtxCollection, vertices)
            try:
                pv = vertices.product()[0]
            except:
                print "WARNING: No good vertices in event {}. Skipping.".format(iEv)
                continue

            ev.getByLabel(self.__class__.lepCollection, leps)
            ev.getByLabel(self.__class__.pfCandCollection, pfCands)

            candHolder = PFCandHolder(pfCands)

            for lep in leps.product():
                self.bookLepton(lep, candHolder)

        print "{} done! {} events processed.".format(self.name, iEv)


class MuonCandMatchChecker(CandMatchChecker):
    '''
    SIP checker for muons only.
    '''
    pdgId = 13
    ptCut = 5.
    etaCut = 2.4
    lepCollection = 'leptonIsoEmbedding:muons'
    lepTypeStr = "pat::Muon"

    @classmethod
    def isPF(cls, mu):
        '''
        Does this muon pass PFID?
        '''
        return mu.isPFMuon()



def main(args):

    fileName = '/data/nawoods/lepsForSIP/lepsForSIP_M2500.root'

    checker = MuonCandMatchChecker('Check PF cand matching', [fileName])

    checker.processSample()
    
    h = checker.hists['pfCandPt']

    h.Sumw2()
    h.drawstyle = 'HIST'
    h.legendstyle = 'L'
    h.linewidth = 2
    h.color = 'red'

    c = Canvas(1000,1000)
    (xax,yax), lims = draw(h, c, ytitle='PF Cands',
                           xtitle='PF Cand p_T',)
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    c.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/matchedCandPt.png')
    
    
    h2 = checker.hists['pfCandPtVsGenPt']
    c2 = Canvas(1000,1000)
    h2.xaxis.SetTitleOffset(1.8)
    h2.yaxis.SetTitleOffset(1.7)
    h2.xaxis.SetTitleSize(0.5 * h2.xaxis.GetTitleSize())
    h2.yaxis.SetTitleSize(0.5 * h2.yaxis.GetTitleSize())
    h2.xaxis.SetLabelSize(0.6 * h2.xaxis.GetLabelSize())
    h2.yaxis.SetLabelSize(0.6 * h2.yaxis.GetLabelSize())
    h2.xaxis.SetTitle("Gen #mu p_{T}")
    h2.yaxis.SetTitle("PF cand p_{T}")
    h2.Draw("COLZ")
    c2.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/matchedCandPtVsGenPt.png')
    

    print "Out of {} good muons, {} failed PF ID ({}%).".format(checker.nTot, checker.nFailPF, 
                                                                100. * float(checker.nFailPF) / checker.nTot)
    print ("Out of those failing PF, {} shared a track with a PF charged hadron "
           "({}%, {}% of the total)").format(checker.nPFMatched, 
                                             100. * checker.nPFMatched / checker.nFailPF,
                                             100. * checker.nPFMatched / checker.nTot)
    

if __name__ == "__main__":
    main(0)
