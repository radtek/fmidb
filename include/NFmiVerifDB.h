#ifndef __NFMIVERIFDB_H__
#define __NFMIVERIFDB_H__

#include <string>
#include <vector>
#include <map>
#include "NFmiOracle.h"

class NFmiVerifDB : public NFmiOracle {

public:

  NFmiVerifDB();
  ~NFmiVerifDB();
       
  static NFmiVerifDB & Instance();
  
  std::map<std::string, std::string> GetStationInfo(unsigned long station_id, bool aggressive_cache = true);
  std::map<int, std::map<std::string, std::string> > GetStationListForArea(unsigned long producer_id, 
                                                                           double max_latitude, 
                                                                           double min_latitude, 
                                                                           double max_longitude, 
                                                                           double min_longitude);
  
  std::map<std::string, std::string> GetParameterDefinition(unsigned long producer_id, unsigned long universal_id);
  std::map<std::string, std::string> GetProducerDefinition(const std::string &producer);
  

  int ParamId(const std::string & theParameter);
  int StatId(const std::string & theStat);
  int PeriodTypeId(const std::string & thePeriod);
  int PeriodId(const std::string & thePeriodName);
  void Initialize(void);

private:

  std::map<unsigned long, std::map<unsigned long, std::map<std::string, std::string> > > parameterinfo;
  std::map<unsigned long, std::map<std::string, std::string> > stations;
  std::map<std::string, std::map<std::string, std::string> > producerinfo;
  
  struct Metadata {

    bool instantiated;

    std::map <std::string, int> periodTypeIds;
    std::map <std::string, int> statIds;
    std::map <std::string, int> paramIds;
    std::map <std::string, int> periodIds;

    Metadata()
  	  : instantiated(false)
     { }

  };

  Metadata metadata;

};
#endif
