import FWCore.ParameterSet.Config as cms


zzDiscriminantBranches = cms.PSet(
    floats = cms.PSet(
        D_bkg_kin = cms.string('? hasUserFloat("D_bkg_kin") ? userFloat("D_bkg_kin") : -999.'),
        D_bkg = cms.string('? hasUserFloat("D_bkg") ? userFloat("D_bkg") : -999.'),
        D_gg = cms.string('? hasUserFloat("Dgg10_VAMCFM") ? userFloat("Dgg10_VAMCFM") : -999.'),
        D_g4 = cms.string('? hasUserFloat("D_g4") ? userFloat("D_g4") : -999.'), # D_0-
        D_VBF2j = cms.string('? hasUserFloat("D_VBF2j") ? userFloat("D_VBF2j") : -999.'),
        D_VBF1j = cms.string('? hasUserFloat("D_VBF1j") ? userFloat("D_VBF1j") : -999.'),
        D_WHh = cms.string('? hasUserFloat("D_WHh") ? userFloat("D_WHh") : -999.'),
        D_ZHh = cms.string('? hasUserFloat("D_ZHh") ? userFloat("D_ZHh") : -999.'),
        D_VBF2j_QG = cms.string('? hasUserFloat("D_VBF2j_QG") ? userFloat("D_VBF2j_QG") : -999.'),
        D_VBF1j_QG = cms.string('? hasUserFloat("D_VBF1j_QG") ? userFloat("D_VBF1j_QG") : -999.'),
        D_WHh_QG = cms.string('? hasUserFloat("D_WHh_QG") ? userFloat("D_WHh_QG") : -999.'),
        D_ZHh_QG = cms.string('? hasUserFloat("D_ZHh_QG") ? userFloat("D_ZHh_QG") : -999.'),
        
        jet1QGLikelihood = cms.string('jet1QGLikelihood'),
        jet2QGLikelihood = cms.string('jet2QGLikelihood'),
        ),
    uints = cms.PSet(
        ZZCategory = cms.string('userInt("ZZCategory")'),
        ZZCategoryQG = cms.string('userInt("ZZCategoryQG")'),
        ),
    )
