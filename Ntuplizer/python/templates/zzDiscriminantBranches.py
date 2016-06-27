import FWCore.ParameterSet.Config as cms


zzDiscriminantBranches = cms.PSet(
    floats = cms.PSet(
        D_bkg_kin = cms.string('userFloat("D_bkg_kin")'),
        D_bkg = cms.string('userFloat("D_bkg")'),
        D_gg = cms.string('userFloat("Dgg10_VAMCFM")'),
        D_HJJ_VBF = cms.string('userFloat("Djet_VAJHU")'),
        D_g4 = cms.string('userFloat("D_g4")'), # D_0-
        D_HJ_VBF = cms.string('userFloat("D_VBF1j_VAJHU")'),
        D_HJJ_WHh = cms.string('userFloat("D_WHh_VAJHU")'),
        D_HJJ_ZHh = cms.string('userFloat("D_ZHh_VAJHU")'),
        D_VBF2j = cms.string('userFloat("D_VBF2j")'),
        D_VBF1j = cms.string('userFloat("D_VBF1j")'),
        D_WHh = cms.string('userFloat("D_WHh")'),
        D_ZHh = cms.string('userFloat("D_ZHh")'),
        
        jet1QGLikelihood = cms.string('jet1QGLikelihood'),
        jet2QGLikelihood = cms.string('jet2QGLikelihood'),
        ),
    ints = cms.PSet(
        ZZCategory = cms.string('userInt("ZZCategory")'),
        ),
    )
