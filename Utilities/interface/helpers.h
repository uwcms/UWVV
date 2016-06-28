#ifndef UWVV_Utilities_helpers_h
#define UWVV_Utilities_helpers_h


#include <string>

#include "TLorentzVector.h"

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/LorentzVector.h"


namespace uwvv
{

  namespace helpers
  {
    // Get an object's four-momentum with FSR included (if any).
    template<class T>
      math::XYZTLorentzVector p4WithFSR(const T& cand,
                                        const std::string& fsrLabel)
      {
        math::XYZTLorentzVector p4 = cand.p4();
        if(cand.hasUserCand(fsrLabel))
          p4 += cand.userCand(fsrLabel)->p4();

        return p4;
      }

    template<>
      math::XYZTLorentzVector p4WithFSR(const pat::CompositeCandidate& cand,
                                        const std::string& fsrLabel)
      {
        math::XYZTLorentzVector p4 = cand.p4();
        if(cand.hasUserInt("n"+fsrLabel+"Cands") && cand.userInt("n"+fsrLabel+"Cands") > 0)
          {
            for(size_t i = 0; i < size_t(cand.userInt("n"+fsrLabel+"Cands")); ++i)
              p4 += cand.userCand(fsrLabel+std::to_string(i))->p4();
          }

        return p4;
      }

    template<class T>
      math::XYZTLorentzVector p4WithFSR(const edm::Ptr<T>& cand,
                                        const std::string& fsrLabel)
      {
        return p4WithFSR(*cand, fsrLabel);
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

    // Return true if the first daughter (made of leptons of type T12) is 
    // farther from the nominal Z mass than the second daughter (made of 
    // leptons of type T34). FSR is included if fsrLabel is non-empty.
    template<class T12, class T34>
      bool zsNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand, 
                         const std::string& fsrLabel)
    {
      math::XYZTLorentzVector p4a = cand->daughter(0)->p4();
      math::XYZTLorentzVector p4b = cand->daughter(1)->p4();
    
      if(!fsrLabel.empty())
        {
          const T12* z1l1 = static_cast<const T12*>(cand->daughter(0)->daughter(0)->masterClone().get());
          if(z1l1->hasUserCand(fsrLabel))
            p4a += z1l1->userCand(fsrLabel)->p4();
      
          const T12* z1l2 = static_cast<const T12*>(cand->daughter(0)->daughter(1)->masterClone().get());
          if(z1l2->hasUserCand(fsrLabel))
            p4a += z1l2->userCand(fsrLabel)->p4();
      
          const T34* z2l1 = static_cast<const T34*>(cand->daughter(1)->daughter(0)->masterClone().get());
          if(z2l1->hasUserCand(fsrLabel))
            p4b += z2l1->userCand(fsrLabel)->p4();
      
          const T34* z2l2 = static_cast<const T34*>(cand->daughter(1)->daughter(1)->masterClone().get());
          if(z2l2->hasUserCand(fsrLabel))
            p4b += z2l2->userCand(fsrLabel)->p4();
        }
  
      return std::abs(p4b.mass() - 91.1876) < std::abs(p4a.mass() - 91.1876);
    }

  } // namespace helpers

} // namespace uwvv



#endif // header guard
