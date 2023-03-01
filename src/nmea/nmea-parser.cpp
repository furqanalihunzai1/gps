// #include <iostream> // For debugging
#include <stdexcept>
#include <cmath>
#include <string>

#include "nmea/nmea-parser.h"
using namespace std;

namespace GPS
{
  bool isSupportedFormat(string s)
  {
	  //function to check if the format is supported
      

      if (s == "GLL" || s == "WPL" || s == "RMC" || s == "GGA")
          return true;
      else
          return false;

  }

  bool isSingleSentence(string s)
  {
	 
      if (s[0] != '$')
          return false;
      if (s[1] != 'G')
          return false;
      if (s[2] < 'A' || s[2]>'Z')
          return false;
      if (s[3] < 'A' || s[3]>'Z')
          return false;
      if (s[4] < 'A' || s[4]>'Z')
          return false;
      for (int i = 5; i < s.length() - 3; i++) {
          if (s[i] == ',')
              continue;
          if (s[i] == '*')
              break;
          if (s[i] == '$' || s[i] == '*')
              return false;
      }
      if (s[s.length() - 3] != '*')
          return false;
      return true;





      
  }

  bool hasCorrectChecksum(string s)
  {
    


      int sum = 0;
      for (int i = 1; i < s.length() - 3; i++) {
          sum = sum ^ s[i];
      }
      int a, b;
      if (s[s.length() - 2] >= '0' && s[s.length() - 2] <= '9')
          a = s[s.length() - 2] - '0';
      else
          a = s[s.length() - 2] - 'A' + 10;
      if (s[s.length() - 1] >= '0' && s[s.length() - 1] <= '9')
          b = s[s.length() - 1] - '0';
      else
          b = s[s.length() - 1] - 'A' + 10;
      if (sum == (a * 16 + b))
          return true;
      else
          return false;




      
  }

  SentenceData parseSingleSentence(string s)
  {
      SentenceData sd;
      sd.transmitter = s.substr(1, 2);
      sd.format = s.substr(3, 3);
      int i = 6;
      while (i < s.length() - 3) {
          string temp;
          while (s[i] != ',') {
              temp = temp + s[i];
              i++;
          }
          sd.dataFields.push_back(temp);
          i++;
      }
      return sd;



      
  }

  bool hasCorrectNumberOfFields(SentenceData sd)
  {
      if (sd.format == "GLL" && sd.dataFields.size() == 7)
          return true;
      if (sd.format == "WPL" && sd.dataFields.size() == 4)
          return true;
      if (sd.format == "RMC" && sd.dataFields.size() == 12)
          return true;
      if (sd.format == "GGA" && sd.dataFields.size() == 15)
          return true;
      return false;





  }

  Position interpretSentenceData(SentenceData d)
  {
       // Ken: Writing 'std::' everywhere is irritating.
      string t, la, lo, ns, ew, e;
      double temp, degs, mins, lat, lon, el;
      Position p = Position(0,0,0); // Dummy object becasue there's no default constructor available for Postion
      t = d.transmitter;
      if (d.format == "GLL")
      {
          la = d.dataFields[0];
          lo = d.dataFields[2];
          ns = d.dataFields[1];
          ew = d.dataFields[3];
          try
          {
              // DDM2DD
              temp  = stod(la);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lat = degs + mins / 60;
              if (lat < 0)
              {
                  throw std::invalid_argument("latitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              temp = stod(lo);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lon = degs + mins / 60;
              if (lon < 0)
              {
                  throw std::invalid_argument("longitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
          }
          catch (const invalid_argument& e)
          {
              throw domain_error(string("Ill-formed GLL sentence field: ") + e.what());
          }
          if (ns.size() == 1) {
              switch (ns[0])
              {
                  case 'N': break;
                  case 'S': lat = -lat; break;
                  default: throw domain_error("Ill-formed GLL sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          } else {
              throw domain_error("Ill-formed GLL sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          }
          if (ew.size() == 1) {
              switch (ew[0])
              {
                  case 'E': break;
                  case 'W': lon = -lon; break;
                  default: throw domain_error("Ill-formed GLL sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
              }
          } else {
              throw domain_error("Ill-formed GLL sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          }
          try
          {
                p = Position(lat,lon,0); // no el
          }
          catch (const invalid_argument& e)
          {
                throw domain_error(string("Ill-formed GLL sentence field: ") + e.what());
          }
      }
      else if (d.format == "WPL")
      {
          la = d.dataFields[0];
          lo = d.dataFields[2];
          ns = d.dataFields[1];
          ew = d.dataFields[3];
          try
          {
              // DDM2DD
              temp  = stod(la);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lat = degs + mins / 60;
              if (lat < 0)
              {
                  throw std::invalid_argument("latitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              temp = stod(lo);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lon = degs + mins / 60;
              if (lon < 0)
              {
                  throw std::invalid_argument("longitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
          }
          catch (const invalid_argument& e)
          {
              throw domain_error(string("Ill-formed WPL sentence field: ") + e.what());
          }
          if (ns.size() == 1) {
              switch (ns[0])
              {
                  case 'N': break;
                  case 'S': lat = -lat; break;
                  default: throw domain_error("Ill-formed WPL sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          } else {
              throw domain_error("Ill-formed WPL sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          }
          if (ew.size() == 1) {
              switch (ew[0])
              {
                  case 'E': break;
                  case 'W': lon = -lon; break;
                  default: throw domain_error("Ill-formed WPL sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
              }
          } else {
              throw domain_error("Ill-formed WPL sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          }
          try
          {
                p = Position(lat,lon,0); // no el
          }
          catch (const invalid_argument& e)
          {
                throw domain_error(string("Ill-formed WPL sentence field: ") + e.what());
          }
      }
      else if (d.format == "RMC")
      {
          la = d.dataFields[2];
          lo = d.dataFields[4];
          ns = d.dataFields[3];
          ew = d.dataFields[5];
          try
          {
              // DDM2DD
              temp  = stod(la);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lat = degs + mins / 60;
              if (lat < 0)
              {
                  throw std::invalid_argument("latitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              temp = stod(lo);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lon = degs + mins / 60;
              if (lon < 0)
              {
                  throw std::invalid_argument("longitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
          }
          catch (const invalid_argument& e)
          {
              throw domain_error(string("Ill-formed RMC sentence field: ") + e.what());
          }
          if (ns.size() != 1) {
              throw domain_error("Ill-formed RMC sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          } else {
              switch (ns[0])
              {
                  case 'N': break;
                  case 'S': lat = -lat; break;
                  default: throw domain_error("Ill-formed RMC sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          }
          if (ew.size() != 1) {
              throw domain_error("Ill-formed RMC sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          } else {
              switch (ew[0])
              {
                  case 'E': break;
                  case 'W': lon = -lon; break;
                  default: throw domain_error("Ill-formed RMC sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
              }
          }
          try {
            p = Position(lat,lon,0); // no el
          }
          catch (const invalid_argument& e) {
            throw domain_error(string("Ill-formed RMC sentence field: ") + e.what());
          }
      } else if (d.format == "GGA")
      {
          la = d.dataFields[1];
          lo = d.dataFields[3];
          ns = d.dataFields[2];
          ew = d.dataFields[4];
          e = d.dataFields[8];
          try
          {
              // DDM2DD
              temp  = stod(la);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lat = degs + mins / 60;
              if (lat < 0)
              {
                  throw std::invalid_argument("latitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              temp = stod(lo);
              degs = floor(temp / 100);
              mins = temp - 100 * degs;
              lon = degs + mins / 60;
              if (lon < 0)
              {
                  throw std::invalid_argument("longitude values in DDM format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              el = stod(e);
          }
          catch (const invalid_argument& e)
          {
              throw domain_error(string("Ill-formed GGA sentence field: ") + e.what());
          }
          if (ns.size() != 1) {
             throw domain_error("Ill-formed GGA sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          } else {
              switch (ns[0])
              {
                  case 'N': break;
                  case 'S': lat = -lat; break;
                  default: throw domain_error("Ill-formed GGA sentence field: " + ns + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          }
          if (ew.size() != 1) {
              throw domain_error("Ill-formed GGA sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          }
          else {
                        switch (ew[0])
                        {
                            case 'E': break;
                            case 'W': lon = -lon; break;
                            default: throw domain_error("Ill-formed GGA sentence field: " + ew + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
                        }
          }
          try {
            p = Position(lat,lon,el);
          } catch (const invalid_argument& e)
          {
              throw domain_error(string("Ill-formed GGA sentence field: ") + e.what());
          }
      }
      return p;
  }

  vector<Position> parseValidSentences(istream &is)
  {
	  //take istream object and return in vector<Position> positions;
	//without using iostream 


          /* Parses a stream of NMEA sentences and returns a vector of Position objects.
    * The stream may contain multiple sentences, and may contain invalid sentences.
    * Only valid sentences are parsed.
    */
      vector<Position> vp;
      string s;
      while (getline(is, s)) {
          if (isSingleSentence(s) && hasCorrectChecksum(s)) {
              SentenceData sd = parseSingleSentence(s);
              if (hasCorrectNumberOfFields(sd)) {
                  try {
                      Position p = interpretSentenceData(sd);
                      vp.push_back(p);
                  }
                  catch (domain_error e) {
                      continue;
                  }
              }
          }
      }
      
      return vp;


      
  }

}
