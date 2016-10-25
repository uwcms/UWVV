#ifndef UWVV_Ntuplizer_FunctionLibrary_h
#define UWVV_Ntuplizer_FunctionLibrary_h


#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

#include "UWVV/Ntuplizer/interface/EventInfo.h"
#include "UWVV/Ntuplizer/interface/StringFunctionMaker.h"
#include "UWVV/Utilities/interface/helpers.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/deltaPhi.h"



namespace
{
  //// Separate templates to allow easier partial specialization

  template<typename B>
    struct GeneralFunctionList
    {
      // Null version for types we don't specify anything
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<B(const edm::Ptr<T>&,uwvv::EventInfo&,const std::string&)> >& addTo) {;}
    };

  template<>
    struct GeneralFunctionList<std::vector<float> >
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<std::vector<float>(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef std::vector<float> (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["jetPt"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.jets(option)->size(); ++i)
                                   out.push_back(evt.jets(option)->at(i).pt());

                                 return out;
                               });

        addTo["jetEta"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.jets(option)->size(); ++i)
                                   out.push_back(evt.jets(option)->at(i).eta());

                                 return out;
                               });

        addTo["jetPhi"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.jets(option)->size(); ++i)
                                   out.push_back(evt.jets(option)->at(i).phi());

                                 return out;
                               });

        addTo["jetRapidity"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.jets(option)->size(); ++i)
                                   out.push_back(evt.jets(option)->at(i).rapidity());

                                 return out;
                               });

        addTo["genJetPt"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.genJets(option)->size(); ++i)
                                   out.push_back(evt.genJets(option)->at(i).pt());

                                 return out;
                               });

        addTo["genJetEta"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.genJets(option)->size(); ++i)
                                   out.push_back(evt.genJets(option)->at(i).eta());

                                 return out;
                               });

        addTo["genJetPhi"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.genJets(option)->size(); ++i)
                                   out.push_back(evt.genJets(option)->at(i).phi());

                                 return out;
                               });

        addTo["genJetRapidity"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;

                                 for(size_t i = 0; i < evt.genJets(option)->size(); ++i)
                                   out.push_back(evt.genJets(option)->at(i).rapidity());

                                 return out;
                               });
        addTo["lheWeights"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 std::vector<float> out;
                                 
                                 if (!evt.lheEventInfo().isValid()) {
                                    std::cout << "NOT VALID!";
                                    return out;
                                    }
                                 for(const auto& weight : evt.lheEventInfo()->weights())
                                    //std::cout << weight << std::endl;
                                    out.push_back(weight.wgt);

                                 return out;
                               });
      }
    };

  template<>
    struct GeneralFunctionList<float>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<float(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef float (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["pvZ"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->z() : -999.);
                               });

        addTo["pvndof"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->ndof() : -999.);
                               });

        addTo["pvRho"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->position().Rho() : -999.);
                               });

        addTo["nTruePU"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return (evt.puInfo().isValid() && evt.puInfo()->size() > 0 ?
                                        evt.puInfo()->at(1).getTrueNumInteractions() :
                                        -1.);});

        addTo["type1_pfMETEt"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.met().pt();});

        addTo["type1_pfMETPhi"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.met().phi();});

        addTo["genWeight"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genEventInfo().isValid() ? evt.genEventInfo()->weight() : 0.);
                               });

        addTo["mtToMET"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 float totalEt = obj->et() + evt.met().et();
                                 float totalPt = (obj->p4() + evt.met().p4()).pt();
                                 float mtSqr = totalEt * totalEt - totalPt * totalPt;

                                 return std::sqrt(mtSqr);
                               });

        addTo["mjj"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 ?
                                         (evt.jets(option)->at(0).p4()+evt.jets(option)->at(1).p4()).mass() :
                                         -999.);
                               });

        addTo["ptjj"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 ?
                                         (evt.jets(option)->at(0).p4()+evt.jets(option)->at(1).p4()).pt() :
                                         -999.);
                               });

        addTo["etajj"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 ?
                                         (evt.jets(option)->at(0).p4()+evt.jets(option)->at(1).p4()).eta() :
                                         -999.);
                               });

        addTo["phijj"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 ?
                                         (evt.jets(option)->at(0).p4()+evt.jets(option)->at(1).p4()).phi() :
                                         -999.);
                               });

        addTo["deltaEtajj"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 ?
                                         fabs(evt.jets(option)->at(0).eta()-evt.jets(option)->at(1).eta()) :
                                         -999.);
                               });

        addTo["jet1QGLikelihood"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 1 && evt.jets(option)->at(0).hasUserFloat("qgLikelihood") ?
                                         evt.jets(option)->at(0).userFloat("qgLikelihood") :
                                         -1.);
                               });

        addTo["jet2QGLikelihood"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.jets(option)->size() >= 2 && evt.jets(option)->at(1).hasUserFloat("qgLikelihood") ?
                                         evt.jets(option)->at(1).userFloat("qgLikelihood") :
                                         -1.);
                               });

        addTo["mjjGen"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genJets(option)->size() >= 2 ?
                                         (evt.genJets(option)->at(0).p4()+evt.genJets(option)->at(1).p4()).mass() :
                                         -999.);
                               });

        addTo["ptjjGen"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genJets(option)->size() >= 2 ?
                                         (evt.genJets(option)->at(0).p4()+evt.genJets(option)->at(1).p4()).pt() :
                                         -999.);
                               });

        addTo["etajjGen"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genJets(option)->size() >= 2 ?
                                         (evt.genJets(option)->at(0).p4()+evt.genJets(option)->at(1).p4()).eta() :
                                         -999.);
                               });

        addTo["phijjGen"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genJets(option)->size() >= 2 ?
                                         (evt.genJets(option)->at(0).p4()+evt.genJets(option)->at(1).p4()).phi() :
                                         -999.);
                               });

        addTo["deltaEtajjGen"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return (evt.genJets(option)->size() >= 2 ?
                                         fabs(evt.genJets(option)->at(0).eta()-evt.genJets(option)->at(1).eta()) :
                                         -999.);
                               });


        addTo["zeppenfeld"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 if(evt.jets(option)->size() < 2)
                                   return -999.;

                                 return std::abs(obj->rapidity() -
                                                 (evt.jets(option)->at(0).rapidity() +
                                                  evt.jets(option)->at(0).rapidity()) / 2.);
                               });

        addTo["deltaPhiToJJ"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 if(evt.jets(option)->size() < 2)
                                   return -999.;

                                 float phiJJ = (evt.jets(option)->at(0).p4() + evt.jets(option)->at(1).p4()).phi();

                                 return std::abs(deltaPhi(obj->phi(), phiJJ));
                               });
      }
    };

  template<>
    struct GeneralFunctionList<bool>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<bool(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef bool (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["pvIsValid"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return evt.pv().isNonnull() && evt.pv()->isValid();
                               });

        addTo["pvIsFake"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return evt.pv().isNull() || evt.pv()->isFake();
                               });
      }
    };

  template<>
    struct GeneralFunctionList<int>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<int(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef int (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["Charge"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option) {return obj->charge();});

        addTo["PdgId"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option) {return obj->pdgId();});
      }
    };

  template<>
    struct GeneralFunctionList<unsigned>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<unsigned(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef unsigned (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["lumi"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.id().luminosityBlock();});

        addTo["run"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.id().run();});

        addTo["nvtx"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.nVertices();});

        addTo["nJets"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.jets(option)->size();});

        addTo["nGenJets"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.genJets(option)->size();});
      }
    };

  template<>
    struct GeneralFunctionList<unsigned long long>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<unsigned long long(const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&)> >& addTo)
      {
        typedef unsigned long long (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

        addTo["evt"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {return evt.id().event();});
      }
    };



  //// Functions specific to a particular object type

  // null if it's some type combination we don't have functions for
  template<typename B, class T>
    struct ObjectFunctionList
    {
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);
      static void addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo) {;}
    };

  template<>
    struct ObjectFunctionList<unsigned, pat::Electron>
    {
      // cheating with typedefs for standardization
      typedef pat::Electron T;
      typedef unsigned B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["MissingHits"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
                               });
      }
    };

  template<>
    struct ObjectFunctionList<float, pat::Electron>
    {
      // cheating with typedefs for standardization
      typedef pat::Electron T;
      typedef float B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SIP3D"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return fabs(obj->dB(T::PV3D)) / obj->edB(T::PV3D);
                               });

        addTo["PVDZ"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->gsfTrack()->dz(evt.pv()->position());
                               });

        addTo["PVDXY"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->gsfTrack()->dxy(evt.pv()->position());
                               });
      }
    };

  template<>
    struct ObjectFunctionList<float, pat::Muon>
    {
      // cheating with typedefs for standardization
      typedef pat::Muon T;
      typedef float B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SIP3D"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return fabs(obj->dB(T::PV3D)) / obj->edB(T::PV3D);
                               });

        addTo["PVDZ"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->muonBestTrack()->dz(evt.pv()->position());
                               });

        addTo["PVDXY"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->muonBestTrack()->dxy(evt.pv()->position());
                               });
      }
    };

  template<>
    struct ObjectFunctionList<bool, pat::Muon>
    {
      // cheating with typedefs for standardization
      typedef pat::Muon T;
      typedef bool B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["IsPFMuon"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option){return obj->isPFMuon();});

        addTo["IsGlobal"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option){return obj->isGlobalMuon();});

        addTo["IsTracker"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option){return obj->isTrackerMuon();});
      }
    };

  template<>
    struct ObjectFunctionList<unsigned, pat::Muon>
    {
      // cheating with typedefs for standardization
      typedef pat::Muon T;
      typedef unsigned B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["BestTrackType"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option){return obj->muonBestTrackType();});

        addTo["MatchedStations"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option){return obj->numberOfMatchedStations();});
      }
    };

  template<>
    struct ObjectFunctionList<float, pat::CompositeCandidate>
    {
      // cheating with typedefs for standardization
      typedef pat::CompositeCandidate T;
      typedef float B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["DR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return reco::deltaR(obj->daughter(0)->p4(),
                                                     obj->daughter(1)->p4());
                               });

        addTo["massNoFSR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return uwvv::helpers::p4WithoutFSR(obj).mass();
                               });

        addTo["ptNoFSR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return uwvv::helpers::p4WithoutFSR(obj).pt();
                               });

        addTo["etaNoFSR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return uwvv::helpers::p4WithoutFSR(obj).eta();
                               });

        addTo["phiNoFSR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return uwvv::helpers::p4WithoutFSR(obj).phi();
                               });

        addTo["energyNoFSR"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return uwvv::helpers::p4WithoutFSR(obj).energy();
                               });
      }
    };

  template<>
    struct ObjectFunctionList<bool, pat::CompositeCandidate>
    {
      // cheating with typedefs for standardization
      typedef pat::CompositeCandidate T;
      typedef bool B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&, const std::string&);

      static void
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SS"] =
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt, const std::string& option)
                               {
                                 return obj->daughter(0)->charge() == obj->daughter(1)->charge();
                               });
      }
    };

} // anonymous namespace


namespace uwvv
{

  template<typename B, class T>
  class BasicFunctionLibrary
  {
   public:

    // Declare signature of these functions as FType
    typedef B (FType) (const edm::Ptr<T>&, EventInfo&, const std::string&);
    // Outward-facing signature doesn't include option argument
    typedef B (FSig) (const edm::Ptr<T>&, EventInfo&);

    BasicFunctionLibrary()
      {
        ::GeneralFunctionList<B>::addFunctions(functions);
        ::ObjectFunctionList<B,T>::addFunctions(functions);
      }
    ~BasicFunctionLibrary() {;}

    std::function<FSig>
    getFunction(const std::string& f) const
      {
        // option indicated by '::', i.e. f="functionName::option"
        size_t sepStart = f.find("::");
        std::string fname = f.substr(0, sepStart);

        // if there's an option but the function is not in the library,
        // something is probably wrong, but we'll just let the
        // StringObjectFunction fail to compile
        if(functions.find(fname) == functions.end())
          return StringFunctionMaker::makeStringFunction<B, T, uwvv::EventInfo&>(f);

        std::string option = "";
        if(sepStart != std::string::npos && sepStart+2 < f.size())
          option = f.substr(sepStart+2);

        return std::bind(functions.at(fname), std::placeholders::_1,
                         std::placeholders::_2, option);
      }

    // for testing purposes
    // const std::unordered_map<std::string, std::function<FType> >&
    //   getAllFunctions() const {return functions;}

   protected:
    std::unordered_map<std::string,
      std::function<FType> > functions;
  };


  // Most function libraries need only what's in BasicFunctionLibrary
  template<typename B, class T>
  class FunctionLibrary : public BasicFunctionLibrary<B,T> {};


  // Function libraries returning vectors need some specialization
  template<typename B, class T>
  class FunctionLibrary<std::vector<B>,T> : public BasicFunctionLibrary<std::vector<B>,T>
  {
   public:
    typedef typename BasicFunctionLibrary<std::vector<B>,T>::FSig FSig;

    using BasicFunctionLibrary<std::vector<B>,T>::getFunction;

    std::function<FSig>
    getFunction(const std::vector<std::string>& fs) const
      {
        if(fs.size() == 1)
          {
            size_t sepStart = fs.at(0).find("::");
            std::string fname = fs.at(0).substr(0,sepStart);

            if(this->functions.find(fname) != this->functions.end())
              return getFunction(fs.at(0));
          }

        // Otherwise, make a new function that returns a vector
        std::vector<std::function<typename FunctionLibrary<B,T>::FSig> > needed;
        for(const auto& f : fs)
          needed.push_back(baseLib.getFunction(f));

        auto out = std::function<FSig>([needed](const edm::Ptr<T>& obj,
                                                uwvv::EventInfo& evt)
                                       {
                                         std::vector<B> out;
                                         for(const auto& fun : needed)
                                           out.push_back(fun(obj, evt));

                                         return out;
                                       });

        return out;
      }

   private:
    const FunctionLibrary<B,T> baseLib;
  };

} // namespace uwvv


#endif // header guard

