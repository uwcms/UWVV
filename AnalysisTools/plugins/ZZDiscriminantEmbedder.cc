//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    ZZDiscriminantEmbedder.cc                                             //
//                                                                          //
//    Takes composite cands for 4l final states and computes a bunch of     //
//    MELA/MEKD discriminants for them.                                     //
//                                                                          //
//    Author: Nate Woods, U. Wisconsin                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// system includes
#include <memory>
#include <vector>

// CMS includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

// ZZMatrixElement includes
#include "ZZMatrixElement/MEMCalculators/interface/MEMCalculators.h"
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

  // Calculate the matrix element for fs under process hypothesis proc using 
  // calculator calc
  const float getZZME(MEMNames::Processes proc, MEMNames::MEMCalcs calc, 
                      const std::vector<TLorentzVector>& p4s,
                      const std::vector<int>& ids);

  // Calculate the 4l SM Higgs and background probability under 
  // systematics assumption syst. Outputs by setting sigProb and bkgProb rather
  // than returning
  void getPm4l(MEMNames::SuperKDsyst syst, 
               const std::vector<TLorentzVector>& p4s,
               const std::vector<int>& ids,
               float& sigProb, float& bkgProb);

  // Get the 4-momenta of the leptons in the candidate
  std::vector<TLorentzVector> getLeptonP4s(const CCand& cand);

  // Get the PDG IDs of the leptons in the candidate
  std::vector<int> getLeptonIDs(const CCand& cand);

  edm::EDGetTokenT<edm::View<CCand> > src;
  edm::EDGetTokenT<edm::View<pat::Jet> > jetSrc;

  std::unique_ptr<MEMs> mem;
  const std::string fsrLabel;
  const std::string qgDiscriminatorLabel;
};


template<class T12, class T34>
ZZDiscriminantEmbedder<T12,T34>::ZZDiscriminantEmbedder(const edm::ParameterSet& pset) :
  src(consumes<edm::View<CCand> >(pset.getParameter<edm::InputTag>("src"))),
  jetSrc(consumes<edm::View<pat::Jet> >(pset.exists("jetSrc") ? 
                                        pset.getParameter<edm::InputTag>("jetSrc") :
                                        edm::InputTag("slimmedJets"))),
  mem(new MEMs(13, 125, "CTEQ6L")),
  fsrLabel(pset.exists("fsrLabel") ? 
           pset.getParameter<std::string>("fsrLabel") : 
           ""),
  qgDiscriminatorLabel(pset.getParameter<std::string>("qgDiscriminator"))
{
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

  TLorentzVector j1P4(0.,0.,0.,0.), j2P4(0.,0.,0.,0.);
  float j1PgOverPq = -1.;
  float j2PgOverPq = -1.;

  if(jets->size() >= 1)
    {
      const pat::Jet& j = jets->at(0);
      j1P4.SetPxPyPzE(j.px(), j.py(), j.pz(), j.energy());

      if(j.hasUserFloat(qgDiscriminatorLabel))
        j1PgOverPq = (1./j.userFloat(qgDiscriminatorLabel) - 1.);
    }

  if(jets->size() >= 2)
    {
      const pat::Jet& j = jets->at(1);
      j2P4.SetPxPyPzE(j.px(), j.py(), j.pz(), j.energy());

      if(j.hasUserFloat(qgDiscriminatorLabel))
        j2PgOverPq = (1./j.userFloat(qgDiscriminatorLabel) - 1.);
    }

  for(size_t i = 0; i < cands->size(); ++i)
    {
      out->push_back(cands->at(i));
      CCand& cand = out->back(); // just for convenience

      std::vector<TLorentzVector> p4s = getLeptonP4s(cand);
      std::vector<int> ids = getLeptonIDs(cand);
      
      float p0plus_VAJHU = getZZME(MEMNames::kSMHiggs,
                                   MEMNames::kJHUGen, p4s, ids);
      float p0minus_VAJHU = getZZME(MEMNames::k0minus,
                                    MEMNames::kJHUGen, p4s, ids);
      float Dgg10_VAMCFM = getZZME(MEMNames::kggHZZ_10,
                                   MEMNames::kMCFM, p4s, ids);
      float bkg_VAMCFM = getZZME(MEMNames::kqqZZ,
                                 MEMNames::kMCFM, p4s, ids);

      float p0plus_m4l, bkg_m4l;
      getPm4l(MEMNames::kNone, p4s, ids, p0plus_m4l, bkg_m4l);

      cand.addUserFloat("p0plus_m4l", p0plus_m4l);
      cand.addUserFloat("bkg_m4l", bkg_m4l);
      cand.addUserFloat("p0plus_VAJHU", p0plus_VAJHU);
      cand.addUserFloat("p0minus_VAJHU", p0minus_VAJHU);
      cand.addUserFloat("Dgg10_VAMCFM", Dgg10_VAMCFM);
      cand.addUserFloat("bkg_VAMCFM", bkg_VAMCFM);
      cand.addUserFloat("D_bkg_kin", 
                        p0plus_VAJHU / (p0plus_VAJHU + bkg_VAMCFM));
      cand.addUserFloat("D_bkg", 
                        p0plus_VAJHU * p0plus_m4l / 
                        (p0plus_VAJHU * p0plus_m4l + bkg_VAMCFM * bkg_m4l));
      cand.addUserFloat("D_g4", p0plus_VAJHU / (p0plus_VAJHU + p0minus_VAJHU));
      
      float phjj_VAJHU = -1.;
      float pvbf_VAJHU = -1.;
      float pwh_hadronic_VAJHU = -1.;
      float pzh_hadronic_VAJHU = -1.;
      float Djet_VAJHU = -1.;
      float D_WHh_VAJHU = -1.;
      float D_ZHh_VAJHU = -1.;
      float pAux_vbf_VAJHU = -1.;
      float phj_VAJHU = -1.;
      float D_VBF1j_VAJHU = -1.;
      float D_VBF1j = -1.;
      float D_VBF2j = -1.;
      float D_WHh = -1.;
      float D_ZHh = -1.;

      if(jets->size())
        {
          Mela* mela = mem->m_MELA;

          TLorentzVector p4(0.,0.,0.,0.);
          for(auto& v : p4s)
            p4 += v;

          if(jets->size() > 1)
            {
              p4s.push_back(j1P4);
              p4s.push_back(j2P4);
              ids.push_back(0);
              ids.push_back(0);

              phjj_VAJHU = getZZME(MEMNames::kJJ_SMHiggs_GG, MEMNames::kJHUGen,
                                   p4s, ids);
              pvbf_VAJHU = getZZME(MEMNames::kJJ_SMHiggs_VBF, 
                                   MEMNames::kJHUGen, p4s, ids);
              mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::WH);
              mela->computeProdP(j1P4, 0, j2P4, 0, p4, pwh_hadronic_VAJHU);
              mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::ZH);
              mela->computeProdP(j1P4, 0, j2P4, 0, p4, pzh_hadronic_VAJHU);

              Djet_VAJHU = pvbf_VAJHU / (pvbf_VAJHU + phjj_VAJHU);
              D_WHh_VAJHU = pwh_hadronic_VAJHU / (pwh_hadronic_VAJHU + 100000.*phjj_VAJHU);
              D_ZHh_VAJHU = pzh_hadronic_VAJHU / (pzh_hadronic_VAJHU + 10000.*phjj_VAJHU);

              D_VBF2j = 1. / (1. + (1. / Djet_VAJHU - 1.) * 
                              TMath::Power(j1PgOverPq * j2PgOverPq, 1./3.));
              D_WHh = 1. / (1. + (1. / D_WHh_VAJHU - 1.) * j1PgOverPq * j2PgOverPq);
              D_ZHh = 1. / (1. + (1. / D_ZHh_VAJHU - 1.) * j1PgOverPq * j2PgOverPq);
            }
          else
            {
              mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::JH);
              mela->computeProdP(j1P4, 0, j2P4, 0, p4, phj_VAJHU);
              mela->setProcess(TVar::HSMHiggs, TVar::JHUGen, TVar::JJVBF);
              mela->computeProdP(j1P4, 0, j2P4, 0, p4, pvbf_VAJHU);
              mela->get_PAux(pAux_vbf_VAJHU);

              D_VBF1j_VAJHU = pvbf_VAJHU * pAux_vbf_VAJHU / 
                (pvbf_VAJHU * pAux_vbf_VAJHU + 5. * phj_VAJHU);

              D_VBF1j = 1. / (1. + (1. / D_VBF1j_VAJHU - 1.) * TMath::Power(j1PgOverPq, 1./3.));
            }
        }

      cand.addUserFloat("phjj_VAJHU", phjj_VAJHU);
      cand.addUserFloat("pvbf_VAJHU", pvbf_VAJHU);
      cand.addUserFloat("pwh_hadronic_VAJHU", pwh_hadronic_VAJHU);
      cand.addUserFloat("pzh_hadronic_VAJHU", pzh_hadronic_VAJHU);
      cand.addUserFloat("Djet_VAJHU", Djet_VAJHU);
      cand.addUserFloat("D_WHh_VAJHU", D_WHh_VAJHU);
      cand.addUserFloat("D_ZHh_VAJHU", D_ZHh_VAJHU);
      cand.addUserFloat("pAux_vbf_VAJHU", pAux_vbf_VAJHU);
      cand.addUserFloat("phj_VAJHU", phj_VAJHU);
      cand.addUserFloat("D_VBF1j_VAJHU", D_VBF1j_VAJHU);
      cand.addUserFloat("D_VBF1j", D_VBF1j);
      cand.addUserFloat("D_VBF2j", D_VBF2j);
      cand.addUserFloat("D_WHh", D_WHh);
      cand.addUserFloat("D_ZHh", D_ZHh);
    }

  iEvent.put(std::move(out));
}


template<class T12, class T34>
const float 
ZZDiscriminantEmbedder<T12,T34>::getZZME(MEMNames::Processes proc, 
                                         MEMNames::MEMCalcs calc, 
                                         const std::vector<TLorentzVector>& p4s,
                                         const std::vector<int>& ids)
{
  double ME;
  mem->computeME(proc, calc, p4s, ids, ME);

  return ME;
}

// Assigns the answers to sigProb and bkgProb rather than returning them
template<class T12, class T34>
void 
ZZDiscriminantEmbedder<T12,T34>::getPm4l(MEMNames::SuperKDsyst syst, 
                                         const std::vector<TLorentzVector>& p4s,
                                         const std::vector<int>& ids,
                                         float& sigProb, float& bkgProb)
{
  double pSig, pBkg;
  mem->computePm4l(p4s, ids, syst, pSig, pBkg);

  sigProb = float(pSig);
  bkgProb = float(pBkg);
}


template<class T12, class T34>
std::vector<TLorentzVector>
ZZDiscriminantEmbedder<T12,T34>::getLeptonP4s(const CCand& cand)
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
ZZDiscriminantEmbedder<T12,T34>::getLeptonIDs(const CCand& cand)
{
  std::vector<int> out;
  
  out.push_back(cand.daughter(0)->daughter(0)->pdgId());
  out.push_back(cand.daughter(0)->daughter(1)->pdgId());
  out.push_back(cand.daughter(1)->daughter(0)->pdgId());
  out.push_back(cand.daughter(1)->daughter(1)->pdgId());

  return out;
}


typedef ZZDiscriminantEmbedder<pat::Electron, pat::Electron> ZZDiscriminantEmbedderEEEE;
typedef ZZDiscriminantEmbedder<pat::Electron, pat::Muon> ZZDiscriminantEmbedderEEMM;
typedef ZZDiscriminantEmbedder<pat::Muon, pat::Muon> ZZDiscriminantEmbedderMMMM;

DEFINE_FWK_MODULE(ZZDiscriminantEmbedderEEEE);
DEFINE_FWK_MODULE(ZZDiscriminantEmbedderEEMM);
DEFINE_FWK_MODULE(ZZDiscriminantEmbedderMMMM);
