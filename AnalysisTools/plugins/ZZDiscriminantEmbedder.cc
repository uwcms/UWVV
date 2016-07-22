//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    ZZDiscriminantEmbedder.cc                                             //
//                                                                          //
//    Takes composite cands for 4l final states and computes a bunch of     //
//    MELA/MEKD discriminants for them, embedded as userFloats.             //
//                                                                          //
//    To speed things up, labels for userFloats indicating decisions on ID, //
//    isolation, etc., may be passed in, and only candidates with all       //
//    daughters passing these will be considered (the rest have nothing     //
//    embedded, so be sure to check downstream).                            //
//                                                                          //
//    Author: Nate Woods, U. Wisconsin                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>
#include <string>
#include <cmath>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

// ZZMatrixElement includes
#include "ZZMatrixElement/MELA/interface/Mela.h"


typedef pat::CompositeCandidate CCand;
typedef pat::CompositeCandidateRef CCandRef;

// Template arguments should be <Electron,Electron>, <Electron,Muon>, 
// or <Muon,Muon> for 4e, 2e2mu, and 4mu, respectively
template<class T12, class T34>
class ZZDiscriminantEmbedder : public edm::one::EDProducer<>
{

public:
  explicit ZZDiscriminantEmbedder(const edm::ParameterSet& pset);
  virtual ~ZZDiscriminantEmbedder(){;}

private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);

  // Check if all daughters of a candidate pass the decision userFloats
  bool candPasses(const CCand& cand) const;

  // Helper to check one of the daughters
  template<class T>
  bool daughterPasses(const T& dau) const;

  // Calculate the matrix element of type me for the final state currently set 
  // in mela, under model proc, for production mode prod
  float getP(TVar::Process proc, TVar::MatrixElement me,
             TVar::Production prod);

  // Calculate the matrix element of type me for the final state and jets 
  // currently set in mela, under model proc, for production mode prod
  float getProdP(TVar::Process proc, TVar::MatrixElement me,
                 TVar::Production prod);

  // Calculate the probability for matrix element of type me for the final 
  // state currently set in mela, under model proc, for production mode prod,
  // with systematic syst
  float getPM4l(TVar::Process proc, TVar::MatrixElement me,
                TVar::Production prod, TVar::SuperMelaSyst syst);

  // Get the 4-momenta of the leptons in the candidate
  std::vector<TLorentzVector> getLeptonP4s(const CCand& cand) const;

  // Get the PDG IDs of the leptons in the candidate
  std::vector<int> getLeptonIDs(const CCand& cand) const;

  //// Bunch of helper functions taken from twiki
  float getDbkgkinConstant(float m4l) const;
  float getDbkgConstant(float m4l) const;
  float getDVBF2jetsConstant(float m4l) const;
  float getDVBF1jetConstant(float m4l) const;


  edm::EDGetTokenT<edm::View<CCand> > src;
  edm::EDGetTokenT<edm::View<pat::Jet> > jetSrc;

  std::unique_ptr<Mela> mela;
  const std::string fsrLabel;
  const std::string qgDiscriminatorLabel;
  const std::vector<std::string> skimDecisionLabels;
};


template<class T12, class T34>
ZZDiscriminantEmbedder<T12,T34>::ZZDiscriminantEmbedder(const edm::ParameterSet& pset) :
  src(consumes<edm::View<CCand> >(pset.getParameter<edm::InputTag>("src"))),
  jetSrc(consumes<edm::View<pat::Jet> >(pset.exists("jetSrc") ? 
                                        pset.getParameter<edm::InputTag>("jetSrc") :
                                        edm::InputTag("slimmedJets"))),
  mela(new Mela(13, 125, TVar::SILENT)),
  fsrLabel(pset.exists("fsrLabel") ? 
           pset.getParameter<std::string>("fsrLabel") : 
           ""),
  qgDiscriminatorLabel(pset.getParameter<std::string>("qgDiscriminator")),
  skimDecisionLabels(pset.exists("skimDecisionLabels") ? 
                     pset.getParameter<std::vector<std::string> >("skimDecisionLabels") : 
                     std::vector<std::string>())
{
  mela->setCandidateDecayMode(TVar::CandidateDecay_ZZ);

  produces<std::vector<CCand> >();
}


template<class T12, class T34>
void
ZZDiscriminantEmbedder<T12,T34>::produce(edm::Event& iEvent, 
                                         const edm::EventSetup& iSetup)
{
  edm::Handle<edm::View<CCand> > cands;
  iEvent.getByToken(src, cands);

  std::unique_ptr<std::vector<CCand> > out(new std::vector<CCand>);

  edm::Handle<edm::View<pat::Jet> > jets;
  iEvent.getByToken(jetSrc, jets);

  SimpleParticleCollection_t simpleJets;
  std::vector<float> pgOverPq;
  for(size_t iJet = 0; iJet < 2 && iJet < jets->size(); ++iJet)
    {
      const pat::Jet& jet = jets->at(iJet);

      TLorentzVector v(jet.px(), jet.py(), jet.pz(), jet.energy());
      simpleJets.push_back(SimpleParticle_t(0, v));

      if(jet.hasUserFloat(qgDiscriminatorLabel))
        pgOverPq.push_back(1./jet.userFloat(qgDiscriminatorLabel) - 1.);
      else
        pgOverPq.push_back(-1.);
    }

  for(size_t i = 0; i < cands->size(); ++i)
    {
      out->push_back(cands->at(i));
      CCand& cand = out->back(); // just for convenience

      // Only do all this slow stuff on a good candidate
      if(!(skimDecisionLabels.size() == 0 || candPasses(cand)))
        continue;

      std::vector<TLorentzVector> p4s = getLeptonP4s(cand);
      TLorentzVector p4Tot = p4s.at(0);
      for(size_t ip4 = 1; ip4 < p4s.size(); ++ip4)
        p4Tot += p4s.at(ip4);
      float m4l = p4Tot.M();

      std::vector<int> ids = getLeptonIDs(cand);
      SimpleParticleCollection_t daughters;
      for(size_t dau = 0; dau < 4; ++dau)
        daughters.push_back(SimpleParticle_t(ids.at(dau), p4s.at(dau)));

      mela->setInputEvent(&daughters, &simpleJets, 0, 0);
      mela->setCurrentCandidateFromIndex(0);

      float p0plus_VAJHU = getP(TVar::HSMHiggs, TVar::JHUGen, 
                                   TVar::ZZGG);
      float p0minus_VAJHU = getP(TVar::H0minus, TVar::JHUGen, 
                                    TVar::ZZGG);

      float pg1g4_VAJHU = -1.;
      mela->setProcess(TVar::SelfDefine_spin0, TVar::JHUGen, TVar::ZZGG);
      (mela->selfDHggcoupl)[0][0] = 1.;
      (mela->selfDHzzcoupl)[0][0][0] = 1.;
      (mela->selfDHzzcoupl)[0][3][0] = 1.;
      mela->computeP(pg1g4_VAJHU, true);
      pg1g4_VAJHU -= p0plus_VAJHU + p0minus_VAJHU;

      float bkg_VAMCFM = getP(TVar::bkgZZ, TVar::MCFM, TVar::ZZQQB);

      float Dgg10_VAMCFM = -1.;
      mela->computeD_gg(TVar::MCFM, TVar::D_gg10, Dgg10_VAMCFM);

      float p0plus_m4l = getPM4l(TVar::HSMHiggs, TVar::JHUGen, TVar::ZZGG, 
                                 TVar::SMSyst_None);
      float bkg_m4l = getPM4l(TVar::bkgZZ, TVar::JHUGen, TVar::ZZGG, 
                              TVar::SMSyst_None);

      cand.addUserFloat("p0plus_m4l", p0plus_m4l);
      cand.addUserFloat("bkg_m4l", bkg_m4l);
      cand.addUserFloat("p0plus_VAJHU", p0plus_VAJHU);
      cand.addUserFloat("p0minus_VAJHU", p0minus_VAJHU);
      cand.addUserFloat("pg1g4_VAJHU", pg1g4_VAJHU);
      cand.addUserFloat("bkg_VAMCFM", bkg_VAMCFM);
      cand.addUserFloat("Dgg10_VAMCFM", Dgg10_VAMCFM);
      cand.addUserFloat("D_sel_kin", 
                        p0plus_VAJHU / (p0plus_VAJHU + bkg_VAMCFM));
      cand.addUserFloat("D_bkg_kin", 
                        p0plus_VAJHU / (p0plus_VAJHU + 
                                        this->getDbkgkinConstant(m4l) * bkg_VAMCFM));
      cand.addUserFloat("D_bkg", 
                        p0plus_VAJHU * p0plus_m4l / 
                        (p0plus_VAJHU * p0plus_m4l + 
                         this->getDbkgConstant(m4l) * bkg_VAMCFM * bkg_m4l));
      cand.addUserFloat("D_g4", p0plus_VAJHU / (p0plus_VAJHU + 
                                                (2.521 * 2.521 * // since g4=1
                                                 p0minus_VAJHU)));
      cand.addUserFloat("D_g1g4", (pg1g4_VAJHU * 2.521 / // 2.521 since g1=g4=1
                                   (p0plus_VAJHU + 
                                    2.521 * 2.521 *
                                    p0minus_VAJHU)));

      float phjj_VAJHU = -1.;
      float pvbf_VAJHU = -1.;
      float pwh_hadronic_VAJHU = -1.;
      float pzh_hadronic_VAJHU = -1.;
      float pAux_vbf_VAJHU = -1.;
      float phj_VAJHU = -1.;
      float D_VBF1j = -1.;
      float D_VBF2j = -1.;
      float D_WHh = -1.;
      float D_ZHh = -1.;
      float D_VBF1j_QG = -1.;
      float D_VBF2j_QG = -1.;
      float D_WHh_QG = -1.;
      float D_ZHh_QG = -1.;

      if(jets->size())
        {
          if(jets->size() > 1)
            {
              phjj_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen, TVar::JJQCD);
              pvbf_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen, TVar::JJVBF);
              pwh_hadronic_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen,
                                            TVar::Had_WH);
              pzh_hadronic_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen,
                                            TVar::Had_ZH);

              D_VBF2j = pvbf_VAJHU / (pvbf_VAJHU + this->getDVBF2jetsConstant(m4l) * phjj_VAJHU);
              D_WHh = pwh_hadronic_VAJHU / (pwh_hadronic_VAJHU + 100000.*phjj_VAJHU);
              D_ZHh = pzh_hadronic_VAJHU / (pzh_hadronic_VAJHU + 10000.*phjj_VAJHU);

              D_VBF2j_QG = 1. / (1. + (1. / D_VBF2j - 1.) * 
                                 std::pow(pgOverPq.at(0) * 
                                          pgOverPq.at(1), 1./3.));
              D_WHh_QG = 1. / (1. + (1. / D_WHh - 1.) * pgOverPq.at(0) * 
                               pgOverPq.at(1));
              D_ZHh_QG = 1. / (1. + (1. / D_ZHh - 1.) * pgOverPq.at(0) * 
                               pgOverPq.at(1));
            }
          else
            {
              phj_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen, TVar::JQCD);
              pvbf_VAJHU = getProdP(TVar::HSMHiggs, TVar::JHUGen, TVar::JJVBF);
              mela->getPAux(pAux_vbf_VAJHU);

              D_VBF1j = pvbf_VAJHU * pAux_vbf_VAJHU / 
                (pvbf_VAJHU * pAux_vbf_VAJHU + 
                 this->getDVBF1jetConstant(m4l) * phj_VAJHU);

              D_VBF1j_QG = 1. / (1. + (1. / D_VBF1j - 1.) * 
                                 std::pow(pgOverPq.at(0), 1./3.));
            }
        }

      cand.addUserFloat("phjj_VAJHU", phjj_VAJHU);
      cand.addUserFloat("pvbf_VAJHU", pvbf_VAJHU);
      cand.addUserFloat("pwh_hadronic_VAJHU", pwh_hadronic_VAJHU);
      cand.addUserFloat("pzh_hadronic_VAJHU", pzh_hadronic_VAJHU);
      cand.addUserFloat("pAux_vbf_VAJHU", pAux_vbf_VAJHU);
      cand.addUserFloat("phj_VAJHU", phj_VAJHU);
      cand.addUserFloat("D_VBF1j", D_VBF1j);
      cand.addUserFloat("D_VBF2j", D_VBF2j);
      cand.addUserFloat("D_WHh", D_WHh);
      cand.addUserFloat("D_ZHh", D_ZHh);
      cand.addUserFloat("D_VBF1j_QG", D_VBF1j_QG);
      cand.addUserFloat("D_VBF2j_QG", D_VBF2j_QG);
      cand.addUserFloat("D_WHh_QG", D_WHh_QG);
      cand.addUserFloat("D_ZHh_QG", D_ZHh_QG);

      mela->resetInputEvent();
    }

  iEvent.put(std::move(out));
}


template<class T12, class T34>
bool
ZZDiscriminantEmbedder<T12,T34>::candPasses(const CCand& cand) const
{
  const reco::Candidate* z1 = cand.daughter(0);
  
  const T12* l1 = static_cast<const T12*>(z1->daughter(0)->masterClone().get());
  if(!daughterPasses(*l1))
    return false;

  const T12* l2 = static_cast<const T12*>(z1->daughter(1)->masterClone().get());
  if(!daughterPasses(*l2))
    return false;

  const reco::Candidate* z2 = cand.daughter(1);

  const T34* l3 = static_cast<const T34*>(z2->daughter(0)->masterClone().get());
  if(!daughterPasses(*l3))
    return false;

  const T34* l4 = static_cast<const T34*>(z2->daughter(1)->masterClone().get());
  if(!daughterPasses(*l4))
    return false;

  return true;
}


template<class T12, class T34>
template<class T>
bool
ZZDiscriminantEmbedder<T12,T34>::daughterPasses(const T& dau) const
{
  for(auto& label : skimDecisionLabels)
    {
      if(!(dau.hasUserFloat(label) && bool(dau.userFloat(label))))
        return false;
    }

  return true;
}


template<class T12, class T34>
float 
ZZDiscriminantEmbedder<T12,T34>::getP(TVar::Process proc, 
                                      TVar::MatrixElement me, 
                                      TVar::Production prod)
{
  float out;
  mela->setProcess(proc, me, prod);
  mela->computeP(out, true);

  return out;
}


template<class T12, class T34>
float 
ZZDiscriminantEmbedder<T12,T34>::getProdP(TVar::Process proc, 
                                          TVar::MatrixElement me, 
                                          TVar::Production prod)
{
  float out;
  mela->setProcess(proc, me, prod);
  mela->computeProdP(out, true);

  return out;
}


template<class T12, class T34>
float
ZZDiscriminantEmbedder<T12,T34>::getPM4l(TVar::Process proc, 
                                         TVar::MatrixElement me, 
                                         TVar::Production prod,
                                         TVar::SuperMelaSyst syst)
{
  float out;
  mela->setProcess(proc, me, prod);
  mela->computePM4l(syst, out);

  return out;
}


template<class T12, class T34>
std::vector<TLorentzVector>
ZZDiscriminantEmbedder<T12,T34>::getLeptonP4s(const CCand& cand) const
{
  std::vector<TLorentzVector> out;
  
  const CCandRef& z1 = cand.daughter(0)->masterClone().castTo<CCandRef>();
  const CCandRef& z2 = cand.daughter(1)->masterClone().castTo<CCandRef>();
  
  edm::Ptr<T12> l1 = z1->daughter(0)->masterClone().castTo<edm::Ptr<T12> >();
  out.push_back(TLorentzVector(l1->p4().x(), l1->p4().y(), l1->p4().z(), l1->p4().t()));
  if(l1->hasUserCand(fsrLabel))
    {
      edm::Ptr<reco::Candidate> fsr = l1->userCand(fsrLabel);
      out.back() += TLorentzVector(fsr->p4().x(), fsr->p4().y(), fsr->p4().z(), fsr->p4().t());
    }

  edm::Ptr<T12> l2 = z1->daughter(1)->masterClone().castTo<edm::Ptr<T12> >();
  out.push_back(TLorentzVector(l2->p4().x(), l2->p4().y(), l2->p4().z(), l2->p4().t()));
  if(l2->hasUserCand(fsrLabel))
    {
      edm::Ptr<reco::Candidate> fsr = l2->userCand(fsrLabel);
      out.back() += TLorentzVector(fsr->p4().x(), fsr->p4().y(), fsr->p4().z(), fsr->p4().t());
    }

  edm::Ptr<T34> l3 = z2->daughter(0)->masterClone().castTo<edm::Ptr<T34> >();
  out.push_back(TLorentzVector(l3->p4().x(), l3->p4().y(), l3->p4().z(), l3->p4().t()));
  if(l3->hasUserCand(fsrLabel))
    {
      edm::Ptr<reco::Candidate> fsr = l3->userCand(fsrLabel);
      out.back() += TLorentzVector(fsr->p4().x(), fsr->p4().y(), fsr->p4().z(), fsr->p4().t());
    }

  edm::Ptr<T34> l4 = z2->daughter(1)->masterClone().castTo<edm::Ptr<T34> >();
  out.push_back(TLorentzVector(l4->p4().x(), l4->p4().y(), l4->p4().z(), l4->p4().t()));
  if(l4->hasUserCand(fsrLabel))
    {
      edm::Ptr<reco::Candidate> fsr = l4->userCand(fsrLabel);
      out.back() += TLorentzVector(fsr->p4().x(), fsr->p4().y(), fsr->p4().z(), fsr->p4().t());
    }

  return out;
}


template<class T12, class T34>
std::vector<int>
ZZDiscriminantEmbedder<T12,T34>::getLeptonIDs(const CCand& cand) const
{
  std::vector<int> out;
  
  out.push_back(cand.daughter(0)->daughter(0)->pdgId());
  out.push_back(cand.daughter(0)->daughter(1)->pdgId());
  out.push_back(cand.daughter(1)->daughter(0)->pdgId());
  out.push_back(cand.daughter(1)->daughter(1)->pdgId());

  return out;
}


//////////////////////////////////////////////////////////////////////////////
//    
//    Essentially everything that follows is taken from the HZZ twiki
//    I cleaned it up a little, but not much
//    https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsZZ4l2016#Kinematic_Discriminants
//    
//////////////////////////////////////////////////////////////////////////////

template<class T12, class T34>
float ZZDiscriminantEmbedder<T12,T34>::getDVBF2jetsConstant(float m4l) const
{
  const float par[9]=
    {
      1.876,
      -55.488,
      403.32,
      0.3906,
      80.8,
      27.7,
      -0.06,
      54.97,
      309.96
    };

  float kappa =
    pow(1.-atan((m4l-par[1])/par[2])*2./TMath::Pi(), par[0])
    + par[3]*exp(-pow((m4l-par[4])/par[5], 2))
    + par[6]*exp(-pow((m4l-par[7])/par[8], 2));

  float constant = kappa/(1.-kappa);
  return constant;
}


template<class T12, class T34>
float ZZDiscriminantEmbedder<T12,T34>::getDVBF1jetConstant(float m4l) const
{
  const float par[8]=
    {
      0.395,
      -0.07,
      85.,
      30.,
      -0.691,
      -5659.47,
      5734.37,
      0.75
    };

  float kappa =
    par[0]
    + par[1]*exp(-pow((m4l-par[2])/par[3], 2))
    + par[4]*pow(log((m4l-par[5])/par[6]), par[7])*(m4l>=(par[5]+par[6]));

  float constant = kappa/(1.-kappa);
  return constant;
}


template<class T12, class T34>
float ZZDiscriminantEmbedder<T12,T34>::getDbkgkinConstant(float m4l) const
{
  const float par[14]=
    {
      0.775,
      -0.565,
      70.,
      5.90,
      -0.235,
      130.1,
      13.25,
      -0.33,
      191.04,
      16.05,
      187.47,
      -0.21,
      1700.,
      400.
    };

  float kappa =
    par[0]
    +par[1]*exp(-pow(((m4l-par[2])/par[3]), 2))
    +par[4]*exp(-pow(((m4l-par[5])/par[6]), 2))
    +par[7]*(
             exp(-pow(((m4l-par[8])/par[9]), 2))*(m4l<par[8])
             + exp(-pow(((m4l-par[8])/par[10]), 2))*(m4l>=par[8])
             )
    + par[11]*exp(-pow(((m4l-par[12])/par[13]), 2));

  float constant = kappa/(1.-kappa);
  return constant;
}

// For 4l. Same as others except for one constant
template<>
float ZZDiscriminantEmbedder<pat::Electron,pat::Electron>::getDbkgkinConstant(float m4l) const
{
  const float par[14]=
    {
      0.775,
      -0.565,
      70.,
      5.90,
      -0.235,
      130.1,
      13.25,
      -0.33,
      191.04,
      16.05,
      187.47,
      -0.42, // this is the different one
      1700.,
      400.
    };

  float kappa =
    par[0]
    +par[1]*exp(-pow(((m4l-par[2])/par[3]), 2))
    +par[4]*exp(-pow(((m4l-par[5])/par[6]), 2))
    +par[7]*(
             exp(-pow(((m4l-par[8])/par[9]), 2))*(m4l<par[8])
             + exp(-pow(((m4l-par[8])/par[10]), 2))*(m4l>=par[8])
             )
    + par[11]*exp(-pow(((m4l-par[12])/par[13]), 2));

  float constant = kappa/(1.-kappa);
  return constant;
}


template<>
float ZZDiscriminantEmbedder<pat::Electron,pat::Electron>::getDbkgConstant(float m4l) const
{
  float cbkgkin = getDbkgkinConstant(m4l);
  return cbkgkin*35.6;
}

template<>
float ZZDiscriminantEmbedder<pat::Electron,pat::Muon>::getDbkgConstant(float m4l) const
{
  float cbkgkin = getDbkgkinConstant(m4l);
  return cbkgkin*41.8;
}

template<>
float ZZDiscriminantEmbedder<pat::Muon,pat::Muon>::getDbkgConstant(float m4l) const
{
  float cbkgkin = getDbkgkinConstant(m4l);
  return cbkgkin*22.8;
}



typedef ZZDiscriminantEmbedder<pat::Electron, pat::Electron> ZZDiscriminantEmbedderEEEE;
typedef ZZDiscriminantEmbedder<pat::Electron, pat::Muon> ZZDiscriminantEmbedderEEMM;
typedef ZZDiscriminantEmbedder<pat::Muon, pat::Muon> ZZDiscriminantEmbedderMMMM;

DEFINE_FWK_MODULE(ZZDiscriminantEmbedderEEEE);
DEFINE_FWK_MODULE(ZZDiscriminantEmbedderEEMM);
DEFINE_FWK_MODULE(ZZDiscriminantEmbedderMMMM);
