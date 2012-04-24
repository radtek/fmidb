#include <NFmiNeonsDB.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <stdexcept>
#include <algorithm>

using namespace std;

NFmiNeonsDB & NFmiNeonsDB::Instance() {
  static NFmiNeonsDB instance_;
  return instance_; 
}

NFmiNeonsDB::NFmiNeonsDB(short theId) : NFmiOracle(), itsId(theId) {
  connected_ = false;
  user_ = "neons_client";
  password_ = "kikka8si";
  database_ = "neons";  
}

NFmiNeonsDB::~NFmiNeonsDB() {
  Disconnect();              
}

/*
 * GetGridLevelName(long, long, long, ,long)
 *
 * Replaces old proC function GetGridLvlNameFromNeons.
 *
 * GetGridLevellName will convert the given InLvlId on the given
 * InCodeTableVer to the correct lvl_type on the given OutCodeTableVer
 * for the given parameter InParmId.
 *
 */

string NFmiNeonsDB::GetGridLevelName(long InParmId, long InLvlId, long InCodeTableVer,long OutCodeTableVer) {

  string lvl_name;
  string parm_name;
  string univ_id;
  string lvl_id = boost::lexical_cast<string>(InLvlId);
  string no_vers = boost::lexical_cast<string>(InCodeTableVer);
  string no_vers2 = boost::lexical_cast<string>(OutCodeTableVer);
  string parm_id = boost::lexical_cast<string> (InParmId);

  // Implement caching since this function is quite expensive

  string key = parm_id + "_" + lvl_id + "_" + no_vers + "_" + no_vers2;

  if (levelinfo.find(key) != levelinfo.end())
    return levelinfo[key];

  parm_name = GetGridParameterName(InParmId,InCodeTableVer,OutCodeTableVer);

  string query = "SELECT lvl_type "
                   "FROM grid_lvl_grib "
                   "WHERE lvl_id = " + lvl_id + " "
                   "AND no_vers2 = " +no_vers;

  Query(query);
  vector<string> row = FetchRow();

  if (!row.empty()) {
    lvl_name = row[0];
  }
  else {
    levelinfo[key] = "";
    levelinfo[key];
  }

  query = "SELECT univ_id "
            "FROM grid_lvl_xref "
            "WHERE lvl_type like '" +lvl_name + "'";
            "AND no_vers2 = " +no_vers;

  Query(query);
  vector<string> id = FetchRow();

  if (!id.empty()) {
    univ_id = id[0];
  }

  query = "SELECT lvl_type "
            "FROM grid_lvl_xref "
            "WHERE parm_name = '" +parm_name +"' "
            "AND no_vers2 = " +no_vers +" "
            "AND univ_id = " +univ_id;

  Query(query);
  vector<string> prod_class = FetchRow();
  string pclass;

  if (!prod_class.empty()) {
    lvl_name = prod_class[0];
  }
  else {
    parm_name = "ALL_OTHERS";
    query = "SELECT lvl_type "
            "FROM grid_lvl_xref "
            "WHERE parm_name = '" +parm_name +"' "
            "AND no_vers2 = " +no_vers +" "
            "AND univ_id = " +univ_id;
    Query(query);
    vector<string> lvltype = FetchRow();

    if (!lvltype.empty()) {
      lvl_name = lvltype[0];
    }
  }

  levelinfo[key] = lvl_name;
  return levelinfo[key];
}

/*
 * GetGridParameterName(long, long, long)
 *
 * Replaces old proC function GetGridParNameFromNeons.
 *
 * GetGridParameterName will convert the InParmId on the given
 * InCodeTableVer to the correct ParName on the given OutCodeTableVer.
 *
 */

string NFmiNeonsDB::GetGridParameterName(long InParmId, long InCodeTableVer, long OutCodeTableVer) {

  string parm_name;
  string univ_id;
  string parm_id = boost::lexical_cast<string>(InParmId);
  string no_vers = boost::lexical_cast<string>(InCodeTableVer);
  string no_vers2 = boost::lexical_cast<string>(OutCodeTableVer);

  // Implement some sort of caching: this function is quite expensive

  string key = parm_id + "_" + no_vers + "_" + no_vers2;

  if (gridparameterinfo.find(key) != gridparameterinfo.end())
    return gridparameterinfo[key];

  string query = "SELECT parm_name "
                 "FROM grid_param_grib "
                 "WHERE parm_id = " + parm_id + " "
                 "AND no_vers = " +no_vers;

  Query(query);
  vector<string> row = FetchRow();

  if (!row.empty())
    parm_name = row[0];

  else
    return "";

  if (InCodeTableVer == OutCodeTableVer) {
    gridparameterinfo[key] = parm_name;
    return gridparameterinfo[key];
  }
  else {
    /* We do some further digging */
    query = "SELECT univ_id "
            "FROM grid_param_xref "
            "WHERE parm_name like '" +parm_name + "'";
            "AND no_vers = " +no_vers;

    Query(query);
    vector<string> id = FetchRow();

    if (!id.empty()) {
      univ_id = id[0];
    }

    /* Finally dig out the parm_name on OutCodeTableVer */
    query = "SELECT parm_name "
            "FROM grid_param_xref "
            "WHERE univ_id = " +univ_id + " "
            "AND no_vers = "+boost::lexical_cast<string>(OutCodeTableVer);

    Query(query);
    vector<string> param = FetchRow();

    if (!param.empty()) {
      parm_name = param[0];
    }

    query = "SELECT max(producer_class) "
            "FROM fmi_producers "
            "WHERE no_vers = " +boost::lexical_cast<string>(OutCodeTableVer);

    Query(query);
    vector<string> prod_class = FetchRow();
    string pclass;

    if (!prod_class.empty()) {
      pclass = prod_class[0];
      /* Switch the -'s to _'s for point producers */
      if( (pclass == "2") | (pclass == "3") ) {
        parm_name.replace(parm_name.find("-"), 1, "_");
      }
    }
  }

  gridparameterinfo[key] = parm_name;
  return gridparameterinfo[key];
}


/*
 * GetGrib2Parameter(long, long, long)
 *
 * Get GRIB 2 parameter id from newbase id and producer id.
 * 
 * TODO: caching
 *
 */

pair<int, int> NFmiNeonsDB::GetGrib2Parameter(unsigned long producerId, unsigned long parameterId) {

  string query = "SELECT category, param "
                   "FROM grid_param_grib2 g, grid_param_xref x, fmi_producers f "
                   "WHERE g.parm_name = x.parm_name AND f.no_vers = x.no_vers "
                   "AND x.univ_id = " + boost::lexical_cast<string> (parameterId);
                   "AND f.producer_id = " + boost::lexical_cast<string> (producerId);

  Query(query);
  
  vector<string> row = FetchRow();

  pair<int, int> p = make_pair (-1, -1);  
  
  if (row.empty())
    return p;

  p = make_pair(boost::lexical_cast<int> (row[0]),
                boost::lexical_cast<int> (row[1]));
                     
  return p;
}

/*
 * GetUnivId(long, string)
 *
 * Get universal parameter id from producer/parameter name combination.
 * This function is used with non-grib data types (most notably with netcdf).
 *
 * http://cf-pcmdi.llnl.gov/documents/cf-standard-names/ecmwf-grib-mapping
 *
 */

std::string NFmiNeonsDB::GetGribParameterNameFromNetCDF(unsigned long producerId, const std::string &nc_param) {

  string query = "SELECT producer_id, parm_name, nc_name "
                   "FROM grid_param_nc "
                   "WHERE nc_name = '" + nc_param + "' "
                   "AND producer_id = " + boost::lexical_cast<string> (producerId);

  Query(query);

  vector<string> row = FetchRow();

  if (row.empty())
    return "";

  return row[1];
}

map<string, string> NFmiNeonsDB::GetParameterDefinition(unsigned long producer_id, const string &parm_name) {

  map <string, string> producer_info = GetProducerDefinition(producer_id);

  string query = "SELECT univ_id FROM grid_param_xref WHERE no_vers = " + producer_info["no_vers"] + " AND parm_name = '" + parm_name + "'";

  Query(query);

  vector<string> row = FetchRow();

  if (row.empty()) {
    map<string, string> ret;
    return ret;
  }

  return GetParameterDefinition(producer_id, boost::lexical_cast<unsigned long> (row[0]));

}

/*
 * GetParameterDefinition()
 * 
 * Retrieves parameter definition from NEONS meta-tables.
 * 
 * Replaces pro*c function 
 *
 * int CheckColumnFromParameter( DBParamDef *ParamDef, DBProducerDef *ProdDef)
 * 
 * defined at objdb/pcsource/CheckColumnFromParameter.pc
 *
 */

map<string, string> NFmiNeonsDB::GetParameterDefinition(unsigned long producer_id, unsigned long universal_id) {

  if (parameterinfo.find(producer_id) != parameterinfo.end())
    if (parameterinfo[producer_id].find(universal_id) != parameterinfo[producer_id].end())
      return parameterinfo[producer_id][universal_id];
  
  map <string, string> ret;
  
  string prod_id = boost::lexical_cast<string> (producer_id);
  string univ_id = boost::lexical_cast<string> (universal_id);
  
  map <string, string> producer_info = GetProducerDefinition(producer_id);

  string forecast_type = producer_info["seq_type_prfx"];
  int producer_class = boost::lexical_cast<int> (producer_info["producer_class"]);
  string no_vers = producer_info["no_vers"];
  int dbclass_id = boost::lexical_cast<int> (producer_info["dbclass_id"]);

  string query = "SELECT "
          "x.parm_name, "
          "x.base, "
          "x.scale, "
          "u.unit_name, "
          "nvl(g.parm_desc,'No Description') AS parm_desc, "
          "nvl(u.unit_desc,'No Description') AS unit_desc, "
          "replace(x.parm_name,'-','_') AS col_name, "
          "x.univ_id "
          "FROM grid_param g, grid_unit u, grid_param_xref x "
          "WHERE u.unit_id = g.unit_id AND x.parm_name = g.parm_name"
          " AND x.univ_id = " + univ_id +
          " AND x.no_vers = " + no_vers;

  Query(query);
   
  vector<string> row = FetchRow();
  
  if (row.empty())
    return ret;
    
  ret["parm_name"] = row[0];
  ret["base"] = row[1];
  ret["scale"] = row[2];
  ret["unit_name"] = row[3];
  ret["parm_desc"] = row[4];
  ret["unit_desc"] = row[5];
  ret["col_name"] = row[6];
  ret["univ_id"] = row[7]; // Fetch univ_id in cased called from GetParameterDefinition(ulong, string)
    
  switch (producer_class) {
    case 1:
      /* grid */
      break;
      
    case 2:
      /* lltbufr */
  
      if (dbclass_id == 11)
      
        /* For Climatic database we dont have the table descriptions.
         * For Climatic database-producer DBCLASS_ID is 11 in the
         * in the FMI_PRODUCER-table 
         */ 
  
        break;
        
      if (producer_id == 1005 || producer_id == 1034)
       
        /* For temp-data there is no openened columns in the database. */
        /* So we can't make the parameter name and database column name conversion */
      
        break;
  
      query = "SELECT 1 FROM lltbufr_seq_col WHERE col_name = replace('" + ret["parm_name"] + "','-','_') AND seq_type = '" + forecast_type + "'";
  
      Query(query);
      
      row = FetchRow();
      
      if (row.empty()) {
        ret.clear();
      }
        
      break;
  
    case 3:
      /* previ */

      query = "SELECT 1 FROM previ_col WHERE col_name = replace( '" + ret["parm_name"] + "','-','_') AND previ_type = '" + forecast_type + "'";
    
      Query(query);
      
      row = FetchRow();
      
      if (row.empty()) {
        ret.clear();
      }    

      break;

    }

  parameterinfo[producer_id][universal_id] = ret;
  
  return ret;
  
}

/*
 * GetProducerDefinition(int)
 * 
 * Retrieves producer definition from neons meta-tables.
 * 
 * Replaces pro*c function 
 *
 * int GetDBProducerDef( DBProducerDef *ProdDef)
 * 
 * defined at fmidbu/pcsource/getdbproducerdef.pc
 *
 */

map<string, string> NFmiNeonsDB::GetProducerDefinition(unsigned long producer_id) {

  if (producerinfo.count(producer_id) > 0)
    return producerinfo[producer_id];

  string query = "SELECT" 
                 " producer_id,"
                 " ref_prod,"
                 " seq_type_prfx,"
                 " producer_class,"
                 " no_vers,"
                 " dbclass_id,"
                 " nvl(hours_for_latest,24) "
                 "FROM fmi_producers "
                 "WHERE producer_id = " + boost::lexical_cast<string> (producer_id); 

  map <string, string> ret;
  
  Query(query);

  vector<string> row = FetchRow();

  if (!row.empty()) {
    ret["producer_id"] = row[0];
    ret["ref_prod"] = row[1];
    ret["seq_type_prfx"] = row[2];
    ret["producer_class"] = row[3];
    ret["no_vers"] = row[4];
    ret["dbclass_id"] = row[5];
    ret["hours_for_latest"] = row[6];
  
    producerinfo[producer_id] = ret;
  }
  
  return ret;
  
}

/*
 * GetProducerDefinition(string)
 * 
 * Retrieves producer definition from neons meta-tables.
 * 
 * Replaces pro*c function 
 *
 * int GetDBProducerDef( DBProducerDef *ProdDef)
 * 
 * defined at fmidbu/pcsource/getdbproducerdef.pc
 *
 */

map<string, string> NFmiNeonsDB::GetProducerDefinition(const string &producer_name) {

  string query = "SELECT"
                 " producer_id "
                 "FROM fmi_producers "
                 "WHERE ref_prod = " + producer_name;

  Query(query);
  
  vector<string> row = FetchRow();
  
  unsigned long int producer_id = boost::lexical_cast<unsigned long> (row[0]);
  
  if (producerinfo.find(producer_id) != producerinfo.end())
    return producerinfo[producer_id];
  
  return GetProducerDefinition(producer_id);
    
}

/*
 * GetGridModelDefinition(int)
 *
 * GetGridModelDefinition will pick up the model information for
 * the given model in the Database.
 *
 */

map<string, string> NFmiNeonsDB::GetGridModelDefinition(unsigned long producer_id) {

  map <string, string> ret;

  string query = "SELECT fmi_producers.ref_prod, "
                 "fmi_producers.no_vers, "
                 "grid_model.model_name, "
                 "grid_model.flag_mod, "
                 "grid_model_name.model_desc, "
                 "grid_num_model_grib.model_id, "
                 "grid_model_ident.ident_name, "
                 "grid_model_ident.ident_id "
                 "FROM fmi_producers, grid_model, grid_model_name, grid_num_model_grib, grid_model_ident "
                 "WHERE fmi_producers.producer_id = " +boost::lexical_cast<string>(producer_id) + " "
                 "AND fmi_producers.producer_class = 1 "
                 "AND fmi_producers.ref_prod = grid_model.model_type "
                 "AND grid_model_name.model_name = grid_model.model_name "
                 "AND grid_num_model_grib.model_name = grid_model.model_name "
                 "AND grid_model_ident.ident_id = grid_num_model_grib.ident_id";

  Query(query);

  vector<string> row = FetchRow();

  if (!row.empty()) {

    ret["ref_prod"] = row[0];
    ret["no_vers"] = row[1];
    ret["model_name"] = row[2];
    ret["flag_mod"] = row[3];
    ret["model_desc"] = row[4];
    ret["model_id"] = row[5];
    ret["ident_name"] = row[6];
    ret["ident_id"] = row[7];
  }

return ret;
}


/*
 * GetNeonsTables(string, string, string)
 * 
 * In Neons one table contains data for one day only. If a query spans multiple
 * days, we must make a UNION over all the tables in our sql query. Function
 * GetNeonsTables() return all intermediate tables when given start and end
 * date and producer name.
 */

vector<string> NFmiNeonsDB::GetNeonsTables(const string &start_time, const string &end_time, const string &producer_name) {

  vector<string> ret;
  
  string query = "SELECT tbl_name FROM as_lltbufr WHERE min_dat <= '" + end_time + "' AND max_dat >= '" + start_time +"' AND seq_type = '" + producer_name + "'";
  
  Query(query);
  
  while (true) {
    
    vector<string> row = FetchRow();
    
    if (row.empty())
      break;
      
    ret.push_back(row[0]);  
  }
  
  return ret;  
}

/*
 * GetGeometryDefinition(string)
 *
 * Retrieves geometry definition from neons meta-tables.
 *
 * Replaces pro*c function
 *
 * int GetDBGeomDef(DBGribGeomDef *GeomDef)
 *
 * defined at fmidbu/pcsource/getdbgeomdef.pc
 *
 */

map<string, string> NFmiNeonsDB::GetGeometryDefinition(const string &geometry_name) {

  if (geometryinfo.find(geometry_name) != geometryinfo.end())
    return geometryinfo[geometry_name];

  string query = "SELECT"
                 " prjn_name,"
                 " row_cnt,"
                 " col_cnt,"
                 " lat_orig,"
                 " long_orig,"
                 " orig_row_num,"
                 " orig_col_num,"
                 " pas_longitude,"
                 " pas_latitude,"
                 " geom_parm_1,"
                 " geom_parm_2,"
                 " geom_parm_3 "
                 "FROM grid_reg_geom "
                 "WHERE geom_name like '" +geometry_name + "'";

  map <string, string> ret;

  Query(query);

  vector<string> row = FetchRow();

  if (!row.empty()) {
    ret["prjn_name"] = row[0];
    ret["row_cnt"] = row[1];
    ret["col_cnt"] = row[2];
    ret["lat_orig"] = row[3];
    ret["long_orig"] = row[4];
    ret["orig_row_num"] = row[5];
    ret["orig_col_num"] = row[6];
    ret["pas_longitude"] = row[7];
    ret["pas_latitude"] = row[8];
    ret["geom_parm_1"] = row[9];
    ret["geom_parm_2"] = row[10];
    ret["geom_parm_3"] = row[11];

    geometryinfo[geometry_name] = ret;
  }

  return ret;

}

/*
 * GetStationInfo(int)
 * 
 * Retrieves station information from Neons table station. 
 * 
 * By default this function uses aggressive caching: when first station 
 * is requested, all stations are read to memory. As more often that not a
 * large number of stations are requested, this will reduce the amount
 * of SQL queries from N to 1. On the other hand when only a few stations
 * are requested, it will be an overkill. This behaviour can be disabled 
 * by setting input argument aggressive_caching to false.
 * 
 * Results are placed in an STL map, with currently three mandatory fields:
 * - station name field must be present and named "station_name"
 * - latitude must be present and named "latitude"
 * - longitude must be present and named "longitude"
 * 
 * Replaces function
 * 
 * int GetDbStationDef(DBStation*)
 * 
 * defined at 
 * 
 * fmidbu/pcsource/getdbstationdef.pc
 * 
 */

map<string, string> NFmiNeonsDB::GetStationInfo(unsigned long wmo_id, bool aggressive_cache) {

  if (stationinfo.find(wmo_id) != stationinfo.end())
    return stationinfo[wmo_id];

  string query = "SELECT "
                 "indicatif_omm, "
                 "nom_station, " 
                 "round(lat/100000, 4) as lat, "
                 "round(lon/100000, 4) as lon, "
                 "lpnn, "
                 "aws_id, "
                 "country_id, "
                 "CASE WHEN elevation_hp IS NOT NULL THEN elevation_hp "
                 "WHEN elevation_ha IS NOT NULL THEN elevation_ha "
                 "ELSE NULL END AS elevation " 
                 "FROM "
                 "station "
                 "WHERE "
                 "indicatif_omm IS NOT NULL";
  
  /*
   * If aggressive_cache is not set, query only for the individual station.
   * Also, if aggressive_cache is set and map stationinfo is already populated
   * do not fetch again all stations. This condition will happen when a station
   * requested does not exist.
   */
  
  if (!aggressive_cache || (aggressive_cache && stationinfo.size() > 0))
    query += " AND indicatif_omm = " + boost::lexical_cast<string> (wmo_id);
  
  Query(query);
  
  while (true) {
    vector <string> values = FetchRow();
      
    map <string, string> station;
        
    if (values.empty())
      break;
      
    int wid = boost::lexical_cast<int> (values[0]);
      
    station["indicatif_omm"] = values[0];
    station["name"] = values[1];
    station["latitude"] = values[2];
    station["longitude"] = values[3];
    station["lpnn"] = values[4];
    station["aws_id"] = values[5];
    station["country_id"] = values[6];
    station["elevation"] = values[7];
      
    stationinfo[wid] = station;
  
  }

  map <string, string> ret;

  if (stationinfo.find(wmo_id) != stationinfo.end())
    ret = stationinfo[wmo_id];
  else
    // If station does not exist, place empty map as a placeholder
    stationinfo[wmo_id] = ret;
  
  return ret;
}

/*
 * GetStationListForArea(float,float,float,float,bool)
 * 
 * Retrieves a list of stations from Neons table station based on 
 * given max/min lat/lon coordinates. If last argument (bool temp) 
 * is false (default), retrieve all stations. If it is true, retrieve
 * only sounding stations.
 * 
 * Results are placed in an STL map, with currently three mandatory fields:
 * - station name field must be present and named "station_name"
 * - latitude must be present and named "latitude"
 * - longitude must be present and named "longitude"
 * 
 * Replaces functions
 * 
 * int GetDbStationList(DBStationList*)
 * 
 * defined at 
 * 
 * fmidbu/pcsource/getdbstationlist.pc
 * 
 * and
 * 
 * int GetDBStationListForTemp(DBStationList*)
 * 
 * defined at
 * 
 * fmidbu/pcsource/getdbstationlistfortemp.pc
 * 
 */

map<int, map<string, string> > NFmiNeonsDB::GetStationListForArea(double max_latitude, double min_latitude, double max_longitude, double min_longitude, bool temp ) {
  
  map<int, map<string, string> > stationlist;
  
  string query = "SELECT "
                 "indicatif_omm, "
                 "indicatif_oaci, "
                 "indicatif_ship, "
                 "indicatif_insee, "
                 "nom_station, " 
                 "lat/100000, "
                 "lon/100000, "
                 "lpnn, "
                 "aws_id, "
                 "country_id, "
                 "CASE WHEN elevation_hp IS NOT NULL THEN elevation_hp "
                 "WHEN elevation_ha IS NOT NULL THEN elevation_ha "
                 "ELSE NULL END AS elevation "
                 "FROM "
                 "station "
                 "WHERE "
                 "indicatif_omm IS NOT NULL "
                 "AND "
                 "lat/100000 BETWEEN " +
                 boost::lexical_cast<string> (min_latitude) +
                 " AND " +
                 boost::lexical_cast<string> (max_latitude) +
                 " AND "
                 "lon/100000 BETWEEN " +
                 boost::lexical_cast<string> (min_longitude) +
                 " AND " +
                 boost::lexical_cast<string> (max_longitude);
                 
                 
  if (temp)    
    query += " AND ( "
             "(OBSALTI00 LIKE '%W%' OR OBSALTI00  LIKE '%P%')"
             "OR (OBSALTI06  LIKE '%W%' OR OBSALTI06  LIKE '%P%')"
             "OR (OBSALTI12  LIKE '%W%' OR OBSALTI12  LIKE '%P%')"
             "OR (OBSALTI18  LIKE '%W%' OR OBSALTI18  LIKE '%P%'))";

  query += " ORDER BY lat DESC, lon";
  
  Query(query);
  
  while (true) {
    vector <string> values = FetchRow();
      
    map <string, string> station;
        
    if (values.empty())
      break;
      
    int wid = boost::lexical_cast<int> (values[0]);

    station["indicatif_omm"] = values[0];
    station["name"] = values[4];
    station["latitude"] = values[5];
    station["longitude"] = values[6];
    station["lpnn"] = values[7];
    station["aws_id"] = values[8];
    station["country_id"] = values[9];
    station["elevation"] = values[10];
      
    stationlist[wid] = station;
      
    /*
     * Fill stationinfo also. This implies that when later on 
     * GetStationInfo() is called, it will not fetch the station list 
     * but uses this information. 
     * 
     * This should not be a problem since when querying data for an area 
     * only include stations that are inside that area. This could be a 
     * problem if in one par we would have an area query and that query 
     * would contain stations outside the area, but AFAIK that is impossible 
     * since parfile can only contain EITHER station id OR coordinates, not both. 
     * 
     */
      
    stationinfo[wid] = station;

  }  

  return stationlist;
}

const int MAX_WORKERS = boost::thread::hardware_concurrency() == 0 ? 4 : boost::thread::hardware_concurrency(); // Should be same as MAX_THREADS

static std::vector<int> itsWorkingList(MAX_WORKERS, -1);
static std::vector<NFmiNeonsDB *> itsWorkerList(MAX_WORKERS);

NFmiNeonsDB * NFmiNeonsDBPool::GetConnection() {
 
  /*
   *  1 --> active
   *  0 --> inactive
   * -1 --> uninitialized
   *
   * Logic of returning connections:
   * 
   * 1. Check if worker is idle, if so return that worker.
   * 2. Check if worker is uninitialized, if so create worker and return that.
   * 3. Sleep and start over
   */ 

  while (true) {
  	
    for (unsigned int i = 0; i < itsWorkingList.size(); i++) {

	    if (itsWorkingList[i] == 0) {
	      itsWorkingList[i] = 1;
        return itsWorkerList[i];
      
	    } else if (itsWorkingList[i] == -1) {

	  	  itsWorkerList[i] = new NFmiNeonsDB(i);
	  	  itsWorkerList[i]->Connect();
	  	  itsWorkerList[i]->Execute("SET TRANSACTION READ ONLY"); 
	  	
	  	  itsWorkingList[i] = 1;
	  	  return itsWorkerList[i];
	  	}
	  }
	  
  	// All threads active
#ifdef DEBUG
  	cout << "Waiting for worker release" << endl;
#endif
 
  	sleep(3);  
  }
  	
	throw runtime_error("Impossible error at NFmiNeonsDBPool::GetConnection()");
	 
}

void NFmiNeonsDBPool::Release(NFmiNeonsDB *theWorker) {
	
	theWorker->Rollback();
  itsWorkingList[theWorker->Id()] = 0;
#ifdef DEBUG
	cout << "Worker released for id " << theWorker->Id() << endl;
#endif
	sleep(1); // Do we need this ?
}