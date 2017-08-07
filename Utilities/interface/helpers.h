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
      math::XYZTLorentzVector p4WithoutFSR(const T& cand);

    template<>
      math::XYZTLorentzVector p4WithoutFSR(const pat::CompositeCandidate& cand);

    template<class T>
      math::XYZTLorentzVector p4WithoutFSR(const edm::Ptr<T>& cand);

    float zMassDistance(const float m);

    float zMassDistance(const TLorentzVector& v);

    float zMassDistance(const math::XYZTLorentzVector& v);

    // Return true if the first daughter is
    // farther from the nominal Z mass than the second daughter
    bool zsNeedReorder(const edm::Ptr<pat::CompositeCandidate>& cand);

    // Check if any final daughters of mother are within dR of cand.
    // "final daughters" means it checks the daughters of daughters if
    // applicable
    bool overlapWithAnyDaughter(const reco::Candidate& cand,
                                const reco::Candidate& mother, float dR);
  } // namespace helpers

} // namespace uwvv



#endif // header guard
