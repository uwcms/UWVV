# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
from rootpy.ROOT import gROOT
gROOT.SetBatch(True)
sys.argv = oldargv

import logging
from rootpy import log as rlog; rlog = rlog["/muonPtChoiceStudy"]
logging.basicConfig(level=logging.WARNING)
rlog["/ROOT.TCanvas.Print"].setLevel(rlog.WARNING)
rlog["/ROOT.TUnixSystem.SetDisplay"].setLevel(rlog.ERROR)

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

from rootpy.plotting import Hist, Hist2D, Graph, Canvas, Legend
from rootpy.plotting.utils import draw
from rootpy import asrootpy

#from multiprocessing import cpu_count, Pool, Manager
#import signal
from glob import glob

from UWVV.Utilities.PlotStyle import PlotStyle



style = PlotStyle()

class PtMatchChecker(object):
    '''
    Base class to check the difference between PF and TuneP muon pt
    '''
    idLabel = "ZZIDPassTightNoVtx"
    isoLabel = "ZZIsoPass"
    vtxCollection = 'vertexCleaning'
    ptCutHigh = 200.

    def __init__(self, name, files):
        self.hists = {
            'pfPtRes' : Hist(41,-.3,.3),
            'tunePPtRes' : Hist(41,-.3,.3),
            'resCompare' : Hist(31,-.2,.2),
            'pfPtResUncor' : Hist(41,-.3,.3),
            'tunePPtResUncor' : Hist(41,-.3,.3),
            'resCompareUncor' : Hist(31,-.2,.2),
            'resCompareBothCor' : Hist(31,-.2,.2),
            }
        self.hists2D = {
            'pfPtResVsPt' : Hist2D(43,140.,1000.,41,-.15,.15),
            'tunePPtResVsPt' : Hist2D(43,140.,1000.,41,-.15,.15),
            'resCompareVsPt' : Hist2D(43,140.,1000.,21,-.1,.1),
            }

        if isinstance(files, str):
            self.files = files.split(',')
        else:
            self.files = files

        self.name = name

    @classmethod
    def isGoodLepton(cls, lep):
        '''
        Is this lepton gen matched and does it pass ID and a high-pt cut?
        '''
        return cls.goodGen(lep) and \
            bool(lep.userFloat(cls.idLabel)) and \
            lep.pt() > cls.ptCutHigh

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

    def bookLepton(self, lep):
        if not self.__class__.isGoodLepton(lep):
            return

        gPt = lep.genParticleRef().pt()

        pfPtCor = lep.pt()
        if lep.hasUserFloat("ptUncorrected"):
            pfPtUncor = lep.userFloat("ptUncorrected")
        else:
            # they're the same
            pfPtUncor = pfPtCor

        tunePPtUncor = lep.tunePMuonBestTrack().pt()
        if lep.hasUserFloat("kalmanTunePPt"):
            tunePPtCor = lep.userFloat("kalmanTunePPt")
        else:
            # they're the same
            tunePPtCor = tunePPtUncor

        pfResCor = (pfPtCor - gPt) / gPt
        tunePResCor = (tunePPtCor - gPt) / gPt
        pfResUncor = (pfPtUncor - gPt) / gPt
        tunePResUncor = (tunePPtUncor - gPt) / gPt
        resComp = abs(pfResCor) - abs(tunePResUncor)
        resCompUncor = abs(pfResUncor) - abs(tunePResUncor)
        resCompBothCor = abs(pfResCor) - abs(tunePResCor)

        self.hists['pfPtRes'].fill(pfResCor)
        self.hists['tunePPtRes'].fill(tunePResCor)
        self.hists['resCompare'].fill(resComp)
        self.hists['pfPtResUncor'].fill(pfResUncor)
        self.hists['tunePPtResUncor'].fill(tunePResUncor)
        self.hists['resCompareUncor'].fill(resCompUncor)
        self.hists['resCompareBothCor'].fill(resCompBothCor)
        self.hists2D['pfPtResVsPt'].fill(gPt,pfResCor)
        self.hists2D['tunePPtResVsPt'].fill(gPt,tunePResUncor)
        self.hists2D['resCompareVsPt'].fill(gPt,resComp)

    def processSample(self):
        events = Events(self.files)

        vertices = Handle("std::vector<reco::Vertex>")
        leps = Handle("std::vector<{}>".format(self.__class__.lepTypeStr))

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

            for lep in leps.product():
                self.bookLepton(lep)

        print "{} done! {} events processed.".format(self.name, iEv)


class MuonPtMatchChecker(PtMatchChecker):
    '''
    SIP checker for muons only.
    '''
    pdgId = 13
    ptCut = 5.
    etaCut = 2.4
    lepCollection = 'leptonIsoEmbedding:muons'
    lepTypeStr = "pat::Muon"



def main(args):

    fileName = '/data/nawoods/lepsForSIP/lepsForSIP_M2500.root'

    checker = MuonPtMatchChecker('Check Pt', [fileName])

    checker.processSample()
    
    hs = checker.hists.copy()

    for var, h in hs.iteritems():

        h.Sumw2()
        h.drawstyle = 'HIST'
        h.legendstyle = 'L'
        h.linewidth = 2
        h.color = 'red'

    cRes = Canvas(1000,1000)
    hs['pfPtRes'].color = 'black'
    hs['pfPtRes'].title = 'PF'
    hs['tunePPtResUncor'].title = 'Tune P'

    (xax,yax), lims = draw([hs['pfPtRes'], hs['tunePPtResUncor']], cRes, 
                           xtitle='#frac{reco p_T - gen p_T}{gen p_T}', 
                           ytitle='Muons')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    legRes = Legend([hs['pfPtRes'], hs['tunePPtResUncor']], cRes)
    legRes.Draw("same")
    cRes.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptRes.png')

    cResComp = Canvas(1000,1000)
    (xax,yax), lims = draw(hs['resCompare'], cResComp, ytitle='Muons',
                           xtitle='#frac{|PF p_T - gen p_T|}{gen p_T} - #frac{|tuneP p_T - gen p_T|}{gen p_T}')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    cResComp.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptResComp.png')
    
    
    cResCor = Canvas(1000,1000)
    hs['pfPtRes'].color = 'black'
    hs['pfPtRes'].title = 'PF'
    hs['tunePPtRes'].title = 'Tune P'

    (xax,yax), lims = draw([hs['pfPtRes'], hs['tunePPtRes']], cResCor, 
                           xtitle='#frac{reco p_T - gen p_T}{gen p_T}', 
                           ytitle='Muons')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    legResCor = Legend([hs['pfPtRes'], hs['tunePPtRes']], cResCor)
    legResCor.Draw("same")
    cResCor.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptResCor.png')

    cResCompCor = Canvas(1000,1000)
    (xax,yax), lims = draw(hs['resCompareBothCor'], cResCompCor, ytitle='Muons',
                           xtitle='#frac{|PF p_T - gen p_T|}{gen p_T} - #frac{|tuneP p_T - gen p_T|}{gen p_T}')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    cResCompCor.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptResCompCor.png')
    
    
    cResUncor = Canvas(1000,1000)
    hs['pfPtResUncor'].color = 'black'
    hs['pfPtResUncor'].title = 'PF'
    hs['tunePPtResUncor'].title = 'Tune P'

    (xax,yax), lims = draw([hs['pfPtResUncor'], hs['tunePPtResUncor']], cResUncor, 
                           xtitle='#frac{reco p_T - gen p_T}{gen p_T}', 
                           ytitle='Muons')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    legResUncor = Legend([hs['pfPtResUncor'], hs['tunePPtResUncor']], cResUncor)
    legResUncor.Draw("same")
    cResUncor.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptResUncor.png')

    cResCompUncor = Canvas(1000,1000)
    (xax,yax), lims = draw(hs['resCompareUncor'], cResCompUncor, ytitle='Muons',
                           xtitle='#frac{|PF p_T - gen p_T|}{gen p_T} - #frac{|tuneP p_T - gen p_T|}{gen p_T}')
    xax.SetTitleSize(0.4 * xax.GetTitleSize())
    xax.SetTitleOffset(1.6)
    xax.SetLabelSize(0.7 * xax.GetLabelSize())
    cResCompUncor.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/ptResCompUncor.png')
    
    
    hs2 = checker.hists2D.copy()

    hs2['pfPtResVsPt'].yaxis.title = '#frac{PF p_T - gen p_T}{gen p_T}'
    hs2['tunePPtResVsPt'].yaxis.title = '#frac{tuneP p_T - gen p_T}{gen p_T}'
    hs2['pfPtResVsPt'].yaxis.title = '#frac{|PF p_T - gen p_T|}{gen p_T} - #frac{|tuneP p_T - gen p_T|}{gen p_T}'

    for var, h in hs2.iteritems():
        c = Canvas(1000,1000)
        h.xaxis.title = 'gen p_T'
        h.drawstyle = 'COLZ'
        h.xaxis.SetTitleSize(0.5 * h.xaxis.GetTitleSize())
        h.xaxis.SetLabelSize(0.7 * h.xaxis.GetLabelSize())
        h.xaxis.SetTitleOffset(1.4)
        h.yaxis.SetTitleSize(0.5 * h.yaxis.GetTitleSize())
        h.yaxis.SetLabelSize(0.7 * h.yaxis.GetLabelSize())
        h.yaxis.SetTitleOffset(1.4)
        h.draw()
        c.Print('/afs/cern.ch/user/n/nawoods/www/ptStudy/{}.png'.format(var))
    
    

if __name__ == "__main__":
    main(0)
