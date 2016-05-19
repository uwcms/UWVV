#ifndef UWVV_Ntuplizer_FunctionLibrary_h
#define UWVV_Ntuplizer_FunctionLibrary_h


#include <functional>
#include <unordered_map>
#include <string>

#include "UWVV/Ntuplizer/interface/EventInfo.h"
#include "UWVV/Ntuplizer/interface/StringFunctionMaker.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Common/interface/Ptr.h"



namespace
{
  //// Separate templates to allow easier partial specialization

  template<typename B>
    struct GeneralFunctionList
    {
      // Null version for types we don't specify anything
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<B(const edm::Ptr<T>&,uwvv::EventInfo&)> >& addTo) {;}
    };

  template<>
    struct GeneralFunctionList<float>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<float(const edm::Ptr<T>&, uwvv::EventInfo&)> >& addTo)
      {
        typedef float (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

        addTo["pvZ"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) 
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->z() : -999.);
                               });

        addTo["pvndof"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) 
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->ndof() : -999.);
                               });

        addTo["pvRho"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) 
                               {
                                 return (evt.pv().isNonnull() ? evt.pv()->position().Rho() : -999.);
                               });

        addTo["nTruePU"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return (evt.puInfo().isValid() && evt.puInfo()->size() > 0 ?
                                        evt.puInfo()->at(1).getTrueNumInteractions() :
                                        -1.);});

        addTo["type1_pfMetEt"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.met().pt();});

        addTo["type1_pfMetPhi"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.met().phi();});

        addTo["genWeight"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return (evt.genEventInfo().isValid() ? evt.genEventInfo()->weight() : 0.);
                               });
      }
    };
        
  template<>
    struct GeneralFunctionList<bool>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<bool(const edm::Ptr<T>&, uwvv::EventInfo&)> >& addTo)
      {
        typedef bool (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

        addTo["pvIsValid"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) 
                               {
                                 return evt.pv().isNonnull() && evt.pv()->isValid();
                               });

        addTo["pvIsFake"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) 
                               {
                                 return evt.pv().isNull() || evt.pv()->isFake();
                               });
      }
    };

  template<>
    struct GeneralFunctionList<int>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<int(const edm::Ptr<T>&, uwvv::EventInfo&)> >& addTo)
      {
        typedef int (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

        addTo["Charge"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) {return obj->charge();});
        
        addTo["PdgId"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt) {return obj->pdgId();});  
      }      
    };

  template<>
    struct GeneralFunctionList<unsigned>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<unsigned(const edm::Ptr<T>&, uwvv::EventInfo&)> >& addTo)
      {
        typedef unsigned (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

        addTo["lumi"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.id().luminosityBlock();});

        addTo["run"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.id().run();});
  
        addTo["nvtx"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.nVertices();});
      }
    };

  template<>
    struct GeneralFunctionList<unsigned long long>
    {
      template<class T> static void
      addFunctions(std::unordered_map<std::string, std::function<unsigned long long(const edm::Ptr<T>&, uwvv::EventInfo&)> >& addTo)
      {
        typedef unsigned long long (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

        addTo["evt"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {return evt.id().event();});
      }
    };



  //// Functions specific to a particular object type

  // null if it's some type combination we don't have functions for
  template<typename B, class T> 
    struct ObjectFunctionList
    {
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);
      static void addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo) {;}
    }; 

  template<>
    struct ObjectFunctionList<unsigned, pat::Electron>
    {
      // cheating with typedefs for standardization
      typedef pat::Electron T;
      typedef unsigned B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["MissingHits"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
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
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SIP3D"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return fabs(obj->dB(T::PV3D)) / obj->edB(T::PV3D);
                               });
  
        addTo["PVDZ"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return obj->gsfTrack()->dz(evt.pv()->position());
                               });
  
        addTo["PVDXY"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
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
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SIP3D"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return fabs(obj->dB(T::PV3D)) / obj->edB(T::PV3D);
                               });
  
        addTo["PVDZ"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return obj->muonBestTrack()->dz(evt.pv()->position());
                               });
  
        addTo["PVDXY"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
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
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["IsPFMuon"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt){return obj->isPFMuon();});
  
        addTo["IsGlobal"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt){return obj->isGlobalMuon();});
  
        addTo["IsTracker"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt){return obj->isTrackerMuon();});
      }
    };
  
  template<>
    struct ObjectFunctionList<unsigned, pat::Muon>
    {
      // cheating with typedefs for standardization
      typedef pat::Muon T;
      typedef unsigned B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["BestTrackType"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt){return obj->muonBestTrackType();});
  
        addTo["MatchedStations"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt){return obj->numberOfMatchedStations();});
      }
    };
  
  template<>
    struct ObjectFunctionList<float, pat::CompositeCandidate>
    {
      // cheating with typedefs for standardization
      typedef pat::CompositeCandidate T;
      typedef float B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["DR"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return reco::deltaR(obj->daughter(0)->p4(),
                                                     obj->daughter(1)->p4());
                               });
      }
    };
  
  template<>
    struct ObjectFunctionList<bool, pat::CompositeCandidate>
    {
      // cheating with typedefs for standardization
      typedef pat::CompositeCandidate T;
      typedef bool B;
      typedef B (FType) (const edm::Ptr<T>&, uwvv::EventInfo&);

      static void 
        addFunctions(std::unordered_map<std::string, std::function<FType> >& addTo)
      {
        addTo["SS"] = 
          std::function<FType>([](const edm::Ptr<T>& obj, uwvv::EventInfo& evt)
                               {
                                 return obj->daughter(0)->charge() == obj->daughter(1)->charge();
                               });
      }
    };
  
}


namespace uwvv
{

  template<typename B, class T>
  class FunctionLibrary
  {
   public:

    // Declare signature of these functions as FType
    typedef B (FType) (const edm::Ptr<T>&, EventInfo&);
    
    FunctionLibrary()
      {
        ::GeneralFunctionList<B>::addFunctions(functions);
        ::ObjectFunctionList<B,T>::addFunctions(functions);
      }
    ~FunctionLibrary() {;}
  
    std::function<FType>
    getFunction(const std::string& f)
      {
        if(functions.find(f) != functions.end())
          return functions.at(f);
        return StringFunctionMaker::makeStringFunction<B, T, uwvv::EventInfo&>(f);
      }
  
   private:
    std::unordered_map<std::string, 
      std::function<FType> > functions;
  };
  

} // namespace


#endif // header guard


// template<class T>
// class FunctionLibrary
// {
//  public:
//   FunctionLibrary()
//     {
//       setupGeneralFunctions();
//       setupSpecificFunctions();
//     }
//   ~FunctionLibrary() {;}
// 
//   std::function<float(const edm::Ptr<T>&, EventInfo&)>
//   getFunctionFloat(const std::string& f)
//     {
//       if(functionsFloat.find(f) != std::unordered_map::end)
//         return functionsFloat.at(f);
//       return StringFunctionMaker::makeStringFunction<float, T>(f);
//     }
//   std::function<bool(const edm::Ptr<T>&, EventInfo&)>
//   getFunctionBool(const std::string& f)
//     {
//       if(functionsBool.find(f) != std::unordered_map::end)
//         return functionsBool.at(f);
//       return StringFunctionMaker::makeStringFunction<bool, T>(f);
//     }
//   std::function<int(const edm::Ptr<T>&, EventInfo&)>
//   getFunctionInt(const std::string& f)
//     {
//       if(functionsInt.find(f) != std::unordered_map::end)
//         return functionsInt.at(f);
//       return StringFunctionMaker::makeStringFunction<int, T>(f);
//     }
//   std::function<unsigned(const edm::Ptr<T>&, EventInfo&)>
//   getFunctionUInt(const std::string& f)
//     {
//       if(functionsUInt.find(f) != std::unordered_map::end)
//         return functionsUInt.at(f);
//       return StringFunctionMaker::makeStringFunction<unsigned, T>(f);
//     }
//   std::function<unsigned long long(const edm::Ptr<T>&, EventInfo&)>
//   getFunctionULL(const std::string& f)
//     {
//       if(functionsULL.find(f) != std::unordered_map::end)
//         return functionsULL.at(f);
//       return StringFunctionMaker::makeStringFunction<unsigned long long, T>(f);
//     }
// 
//  private:
//   void setupGeneralFunctions();
//   void setupSpecificFunctions();
// 
//   template<typename Ret>
//   std::function<Ret(const edm::Ptr<T>&, EventInfo&)>
//   makeFunction(Ret(const edm::Ptr<T>&,EventInfo&) f)
//     {
//       return std::function<Ret(const edm::Ptr<T>&,EventInfo&)>(f);
//     }
// 
//   std::unordered_map<std::string, 
//     std::function<float(const edm::Ptr<T>&, EventInfo&)> > functionsFloat;
//   std::unordered_map<std::string, 
//     std::function<bool(const edm::Ptr<T>&, EventInfo&)> > functionsBool;
//   std::unordered_map<std::string, 
//     std::function<int(const edm::Ptr<T>&, EventInfo&)> > functionsInt;
//   std::unordered_map<std::string, 
//     std::function<unsigned(const edm::Ptr<T>&, EventInfo&)> > functionsUInt;
//   std::unordered_map<std::string, 
//     std::function<unsigned long long(const edm::Ptr<T>&, EventInfo&)> > functionsULL;
// };
// 
// 
// template<class T> void
// FunctionLibrary<T>::setupGeneralFunctions()
// {
//   functionsInt["Charge"] = 
//     makeFunction<int> ([](const edm::Ptr<T>& obj, EventInfo& evt) {return obj->charge();});
// 
//   functionsInt["PdgId"] = 
//     makeFunction<int> ([](const edm::Ptr<T>& obj, EventInfo& evt) {return obj->PdgId();});  
// 
//   functionsULL["evt"] = 
//     makeFunction<unsigned long long>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                                      {return evt.id().event();});
//   functionsUInt["lumi"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {return evt.id().luminosityBlock();});
//   functionsUInt["run"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {return evt.id().run();});
// 
//   functionsUInt["nvtx"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {return evt.nVertices();});
// 
//   functionsUInt["nTruePU"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {return (evt.puInfo().isValid() && evt.puInfo()->size() > 0 ?
//                                     evt.puInfo()->at(1).getTrueNumInteractions() :
//                                     -1);});
// }
// 
// 
// template<class T> void
// FunctionLibrary<T>::setupSpecificFunctions()
// {;}
// 
// 
// template<> void
// FunctionLibrary<pat::Electron>::setupSpecificFunctions()
// {
//   functionsUInt["MissingHits"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return obj->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
//                            });
// 
//   functionsFloat["SIP3D"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return fabs(obj->dB(pat::Electron::PV3D)) / obj->edB(pat::Electron::PV3D);
//                            });
// 
//   functionsFloat["PVDZ"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return obj->gsfTrack()->dz(evt.pv()->position());
//                            });
// 
//   functionsFloat["PVDXY"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return obj->gsfTrack()->dxy(evt.pv()->position());
//                            });
//   
// }
// 
// 
// template<> void
// FunctionLibrary<pat::Muon>::setupSpecificFunctions()
// {
//   functionsBool["isPFMuon"] = 
//     makeFunction<bool>([](const edm::Ptr<T>& obj, EventInfo& evt){return obj->isPFMuon()};);
// 
//   functionsBool["isGlobal"] = 
//     makeFunction<bool>([](const edm::Ptr<T>& obj, EventInfo& evt){return obj->isGlobalMuon()};);
// 
//   functionsBool["isTracker"] = 
//     makeFunction<bool>([](const edm::Ptr<T>& obj, EventInfo& evt){return obj->isTrackerMuon()};);
// 
//   functionsUInt["BestTrackType"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt){return obj->muonBestTrackType()};);
// 
//   functionsUInt["MatchedStations"] = 
//     makeFunction<unsigned>([](const edm::Ptr<T>& obj, EventInfo& evt){return obj->numberOfMatchedStations()};);
// 
//   functionsFloat["SIP3D"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return fabs(obj->dB(pat::Muon::PV3D)) / obj->edB(pat::Muon::PV3D);
//                            });
// 
//   functionsFloat["PVDZ"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return obj->muonBestTrack()->dz(evt.pv()->position());
//                            });
// 
//   functionsFloat["PVDXY"] = 
//     makeFunction<float>([](const edm::Ptr<T>& obj, EventInfo& evt)
//                            {
//                              return obj->muonBestTrack()->dxy(evt.pv()->position());
//                            });
  
