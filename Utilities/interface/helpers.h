#ifndef UWVV_Utilities_helpers_h
#define UWVV_Utilities_helpers_h


#include <string>

#include "TLorentzVector.h"

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Candidate/interface/Candidate.h"

namespace uwvv
{

  namespace helpers
  {
    // Get an object's four-momentum with FSR included (if any).
    template<class T>
      math::XYZTLorentzVector p4WithoutFSR(const T& cand)
      {
        // if it's not a composite, FSR isn't included by definition
        if(cand.numberOfDaughters() == 0)
          return cand.p4();

        const pat::CompositeCandidate& ccand = dynamic_cast<const pat::CompositeCandidate&>(cand);
        return p4WithoutFSR(ccand);
      }

    template<>
      math::XYZTLorentzVector p4WithoutFSR(const pat::CompositeCandidate& cand)
      {
        math::XYZTLorentzVector out = p4WithoutFSR(*(cand.daughter(0)->masterClone().get()));
        if(cand.numberOfDaughters() >= 2)
          out += p4WithoutFSR(*(cand.daughter(1)->masterClone().get()));

        return out;
      }

    template<class T>
      math::XYZTLorentzVector p4WithoutFSR(const edm::Ptr<T>& cand)
      {
        return p4WithoutFSR(*cand);
      }

    float zMassDistance(const float m)
    {
      return std::abs(m - 91.1876);
    }

    float zMassDistance(const TLorentzVector& v)
    {
      return zMassDistance(v.M());
    }

    float zMassDistance(const math::XYZTLorentzVector& v)
    {
      return zMassDistance(v.mass());
    }

    // Return true if the first daughter is
    // farther from the nominal Z mass than the second daughter
    bool zsNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand)
    {
      math::XYZTLorentzVector p4a = cand->daughter(0)->p4();
      math::XYZTLorentzVector p4b = cand->daughter(1)->p4();

      return std::abs(p4b.mass() - 91.1876) < std::abs(p4a.mass() - 91.1876);
    }

    // Check if any final daughters of mother are within dR of cand.
    // "final daughters" means it checks the daughters of daughters if
    // applicable
    bool overlapWithAnyDaughter(const reco::Candidate& cand,
                                const reco::Candidate& mother, float dR)
    {
      if(!mother.numberOfDaughters()) // end recursion
        return reco::deltaR(cand.p4(), mother.p4()) < dR;

      for(size_t i = 0; i < mother.numberOfDaughters(); ++i)
        {
          if(overlapWithAnyDaughter(cand, *mother.daughter(i), dR))
            return true;
        }

      return false;
    }
  } // namespace helpers

} // namespace uwvv



#endif // header guard
