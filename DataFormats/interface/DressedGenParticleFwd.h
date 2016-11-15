#ifndef DressedGenParticleFwd_h
#define DressedGenParticleFwd_h
#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"

class DressedGenParticle;

/// collection of DressedGenParticle objects
typedef std::vector<DressedGenParticle> DressedGenParticleCollection;

/// reference to an object in a collection of DressedGenParticle objects
typedef edm::Ref<DressedGenParticleCollection> DressedGenParticleRef;

/// reference to a collection of DressedGenParticle objects
typedef edm::RefProd<DressedGenParticleCollection> DressedGenParticleRefProd;

/// vector of objects in the same collection of DressedGenParticle objects
typedef edm::RefVector<DressedGenParticleCollection> DressedGenParticleRefVector;

/// iterator over a vector of reference to DressedGenParticle objects
typedef DressedGenParticleRefVector::iterator dressedgenpart_iterator;

#endif
