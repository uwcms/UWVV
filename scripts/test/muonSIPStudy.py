# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
from rootpy.ROOT import gROOT
gROOT.SetBatch(True)
sys.argv = oldargv

import logging
from rootpy import log as rlog; rlog = rlog["/leptonEfficiencyHighMass"]
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

class SIPChecker(object):
    '''
    Base class to check the SIP of good leptons in a sample.
    '''
    idLabel = "ZZIDPassTightNoVtx"
    isoLabel = "ZZIsoPass"
    vtxCollection = 'vertexCleaning'

    def __init__(self, name, files):
        self.hists = {
            'sip'   : Hist(32,-8.,8.),
            'dxy'   : Hist(40,-.4,.4),
            'dz'    : Hist(40,-.6,.6),
            'ip'    : Hist(40,-.01,.01),
            'ipErr' : Hist(40,0.,.01),
            }

        self.nPassSIP = 0
        self.nTot = 0

        if isinstance(files, str):
            self.files = files.split(',')
        else:
            self.files = files

        self.name = name

    @classmethod
    def isGoodLepton(cls, lep):
        '''
        Is this lepton gen matched and does it pass ID (minus 
        vertex requirements) and isolation?
        '''
        return cls.goodGen(lep) and \
            bool(lep.userFloat(cls.idLabel)) and \
            bool(lep.userFloat(cls.isoLabel))

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

    @staticmethod
    def getIP3D(lep):
        return lep.dB(lep.__class__.PV3D)

    @staticmethod
    def getIP3DErr(lep):
        return lep.edB(lep.__class__.PV3D)

    @classmethod
    def getDXY(cls, lep, pv):
        return cls.getTrack(lep).dxy(pv.position())

    @classmethod
    def getDZ(cls, lep, pv):
        return cls.getTrack(lep).dz(pv.position())

    def bookLepton(self, lep, pv):
        if not self.__class__.isGoodLepton(lep):
            return

        self.nTot += 1

        ip = self.__class__.getIP3D(lep)
        ipErr = self.__class__.getIP3DErr(lep)
        sip = ip / ipErr
        if abs(sip) < 4.:
            self.nPassSIP += 1

        self.hists['sip'].fill(sip)
        self.hists['dxy'].fill(self.__class__.getDXY(lep, pv))
        self.hists['dz'].fill(self.__class__.getDZ(lep, pv))
        self.hists['ip'].fill(ip)
        self.hists['ipErr'].fill(ipErr)

    def processSample(self):
        events = Events(self.files)

        leps = Handle("std::vector<{}>".format(self.__class__.lepTypeStr))
        vertices = Handle("std::vector<reco::Vertex>")

        for iEv, ev in enumerate(events):
            if iEv % 5000 == 0:
                print "{} processing event {}".format(self.name, iEv)

            ev.getByLabel(self.__class__.vtxCollection, vertices)
            try:
                pv = vertices.product()[0]
            except:
                print "WARNING: {} found no good vertices in event {}. Skipping.".format(self.name, iEv)
                continue

            ev.getByLabel(self.__class__.lepCollection, leps)

            for lep in leps.product():
                self.bookLepton(lep, pv)

        print "{} done! {} events processed.".format(self.name, iEv)


class MuonSIPChecker(SIPChecker):
    '''
    SIP checker for muons only.
    '''
    pdgId = 13
    ptCut = 5.
    etaCut = 2.4
    lepCollection = 'leptonIsoEmbedding:muons'
    lepTypeStr = "pat::Muon"

    @classmethod
    def getTrack(cls, mu):
        return mu.muonBestTrack()


_colors = {
    125 : 'black',
    350 : 'magenta',
    500 : 'purple',
    750 : 'red',
    900 : 'green',
    1500 : 'teal',
    2500 : 'orange',
    }
def getColor(m):
    return _colors[m]


def main(args):

    fileList = glob('/data/nawoods/lepsForSIP/*.root')
    files = {int(f.split('_M')[1].split('.')[0]) : f for f in fileList}

    checkers = {m : MuonSIPChecker('m={}'.format(m), [f]) for m,f in files.iteritems()}
    sipRMS = Graph(len(fileList), type='errors')
    sipHists = []
    dxyRMS = Graph(len(fileList), type='errors')
    dxyHists = []
    dzRMS = Graph(len(fileList), type='errors')
    dzHists = []

    ipHists = []
    ipErrHists = []

    fracFailing = Graph(len(fileList), type='errors')
    for i, m in enumerate(sorted(files.keys())):

        checkers[m].processSample()

        checkers[m].hists['sip'].Sumw2()
        sipRMS.SetPoint(i, float(m), checkers[m].hists['sip'].GetRMS())
        sipRMS.SetPointError(i, 0., checkers[m].hists['sip'].GetRMSError())
        sipHists.append(checkers[m].hists['sip'])
        sipHists[-1].color = getColor(m)
        sipHists[-1].title = "m_{{H}} = {}".format(m)
        sipHists[-1].drawstyle = 'hist'
        sipHists[-1].legendstyle = 'L'
        sipHists[-1].linewidth = 2
        sipHists[-1].scale(1./sipHists[-1].integral())

        ipHists.append(checkers[m].hists['ip'])
        ipHists[-1].color = getColor(m)
        ipHists[-1].title = "m_{{H}} = {}".format(m)
        ipHists[-1].drawstyle = 'hist'
        ipHists[-1].legendstyle = 'L'
        ipHists[-1].linewidth = 2
        ipHists[-1].scale(1./ipHists[-1].integral())

        ipErrHists.append(checkers[m].hists['ipErr'])
        ipErrHists[-1].color = getColor(m)
        ipErrHists[-1].title = "m_{{H}} = {}".format(m)
        ipErrHists[-1].drawstyle = 'hist'
        ipErrHists[-1].legendstyle = 'L'
        ipErrHists[-1].linewidth = 2
        ipErrHists[-1].scale(1./ipErrHists[-1].integral())

        checkers[m].hists['dxy'].Sumw2()
        dxyRMS.SetPoint(i, float(m), checkers[m].hists['dxy'].GetRMS())
        dxyRMS.SetPointError(i, 0., checkers[m].hists['dxy'].GetRMSError())
        dxyHists.append(checkers[m].hists['dxy'])
        dxyHists[-1].color = getColor(m)
        dxyHists[-1].title = "m_{{H}} = {}".format(m)
        dxyHists[-1].drawstyle = 'hist'
        dxyHists[-1].legendstyle = 'L'
        dxyHists[-1].linewidth = 2
        dxyHists[-1].scale(1./dxyHists[-1].integral())
        
        checkers[m].hists['dz'].Sumw2()
        dzRMS.SetPoint(i, float(m), checkers[m].hists['dz'].GetRMS())
        dzRMS.SetPointError(i, 0., checkers[m].hists['dz'].GetRMSError())
        dzHists.append(checkers[m].hists['dz'])
        dzHists[-1].color = getColor(m)
        dzHists[-1].title = "m_{{H}} = {}".format(m)
        dzHists[-1].drawstyle = 'hist'
        dzHists[-1].legendstyle = 'L'
        dzHists[-1].linewidth = 2
        dzHists[-1].scale(1./dzHists[-1].integral())

        fracFailing.SetPoint(i, float(m),
                             1. - float(checkers[m].nPassSIP) / checkers[m].nTot)

    
    cSIP = Canvas(1000,1000)
    draw(sipHists, cSIP, xtitle='#frac{IP_{3D}}{#sigma_{IP_{3D}}}',
         ytitle='arb.')
    legSIP = Legend(sipHists, cSIP, textsize=.03, entrysep=0.015, leftmargin=0.6, entryheight=0.04)
    legSIP.Draw("same")
    cSIP.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/sips.png')
    cSIPLog = Canvas(1000,1000)
    draw(sipHists, cSIPLog, xtitle='#frac{IP_{3D}}{#sigma_{IP_{3D}}}',
         ytitle='arb.', logy=True)
    legSIPLog = Legend(sipHists, cSIPLog, textsize=.027, entrysep=0.012, leftmargin=0.6, entryheight=0.03)
    legSIPLog.Draw("same")
    cSIPLog.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/sipsLog.png')
    
    cSIPRMS = Canvas(1000, 1000)
    sipRMS.color = 'b'
    sipRMS.drawstyle = 'PE'
    sipRMS.legendstyle = 'PE'
    draw(sipRMS, cSIPRMS, xtitle="m_{H}", ytitle="RMS(SIP_{3D})", xlimits=(0.,2600.))
    cSIPRMS.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/sipRMS.png')
    
    cIP = Canvas(1000,1000)
    draw(ipHists, cIP, xtitle='IP_{3D}',
         ytitle='arb.')
    legIP = Legend(ipHists, cIP, textsize=.03, entrysep=0.015, leftmargin=0.6, entryheight=0.04)
    legIP.Draw("same")
    cIP.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/ips.png')
    cIPLog = Canvas(1000,1000)
    draw(ipHists, cIPLog, xtitle='#frac{IP_{3D}}{#sigma_{IP_{3D}}}',
         ytitle='arb.', logy=True)
    legIPLog = Legend(ipHists, cIPLog, textsize=.027, entrysep=0.012, leftmargin=0.6, entryheight=0.03)
    legIPLog.Draw("same")
    cIPLog.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/ipsLog.png')
    
    cIPErr = Canvas(1000,1000)
    draw(ipErrHists, cIPErr, xtitle='#sigma_{IP_{3D}}',
         ytitle='arb.')
    legIPErr = Legend(ipErrHists, cIPErr, textsize=.03, entrysep=0.015, leftmargin=0.6, entryheight=0.04)
    legIPErr.Draw("same")
    cIPErr.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/ipErrs.png')
    cIPErrLog = Canvas(1000,1000)
    draw(ipErrHists, cIPErrLog, xtitle='#sigma_{IP_{3D}}',
         ytitle='arb.', logy=True)
    legIPErrLog = Legend(ipErrHists, cIPErrLog, textsize=.027, entrysep=0.012, leftmargin=0.6, entryheight=0.03)
    legIPErrLog.Draw("same")
    cIPErrLog.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/ipErrsLog.png')
    
    cFail = Canvas(1000, 1000)
    fracFailing.color = 'b'
    fracFailing.drawstyle = 'PE'
    fracFailing.legendstyle = 'PE'
    draw(fracFailing, cSIPRMS, xtitle="m_{H}", ytitle="Fraction failing SIP", xlimits=(0.,2600.))
    cSIPRMS.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/fracFailing.png')
    
    cDXY = Canvas(1000,1000)
    draw(dxyHists, cDXY, xtitle='#Delta_{xy}',
         ytitle='arb.')
    legDXY = Legend(dxyHists, cDXY, textsize=.03, entrysep=0.015, leftmargin=0.6, entryheight=0.04)
    legDXY.Draw("same")
    cDXY.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/dxys.png')

    cDXYRMS = Canvas(1000, 1000)
    dxyRMS.color = 'b'
    dxyRMS.drawstyle = 'PE'
    dxyRMS.legendstyle = 'PE'
    draw(dxyRMS, cDXYRMS, xtitle="m_{H}", ytitle="RMS(#Delta_{xy})", xlimits=(0.,2600.))
    cDXYRMS.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/dxyRMS.png')
    
    cDZ = Canvas(1000,1000)
    draw(dzHists, cDZ, xtitle='#Delta_{z}',
         ytitle='arb.')
    legDZ = Legend(dzHists, cDZ, textsize=.03, entrysep=0.015, leftmargin=0.6, entryheight=0.04)
    legDZ.Draw("same")
    cDZ.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/dzs.png')

    cDZRMS = Canvas(1000, 1000)
    dzRMS.color = 'b'
    dzRMS.drawstyle = 'PE'
    dzRMS.legendstyle = 'PE'
    draw(dzRMS, cDZRMS, xtitle="m_{H}", ytitle="RMS(#Delta_{z})", xlimits=(0.,2600.))
    cDZRMS.Print('/afs/cern.ch/user/n/nawoods/www/sipStudy/dzRMS.png')
    

if __name__ == "__main__":
    main(0)
