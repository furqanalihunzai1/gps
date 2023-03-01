// #include <iostream> // For debugging
#include <stdexcept>
#include <cmath>

#include "nmea/nmea-parser.h"

namespace GPS::NMEA
{
  bool isSupportedFormat(std::string)
  {
      // Stub definition, needs implementing
      return false;
  }

  bool isSingleSentence(std::string s)
  {
      char c;
      unsigned int i;
      if (s.empty()) return false; // Need this or next line could crash
      if (s[0] != '$') return false;
      if (s.size() < 3) return false; // Need this or next line could crash
      if (s[1] != 'G') return false;
      // cout << "Debug A" << endl;
      if (s.size() < 6) return false; // Need this or next line could crash
      c = s[2];
      if (c < 65 || c > 90) return false;
      c = s[3];
      if (c < 65 || c > 90) return false;
      c = s[4];
      if (c < 65 || c > 90) return false;
      c = s[5];
      if (c < 'A' || c > 'Z') return false; // Hey Ken: we can do it like this instead of using the ASCII codes.  Shall I change the previous two like this as well?
      // cout << "Debug B" << endl;
      if (s.size() < 7) return false; // Need this or next line could crash.
      if (s[6] != ',') return false;
      // cout << "Debug C" << endl;
      /*
      i = 7;
      while (true)
      {
          if (i == s.size()) return false;
          if (s[i] == '$') return false; // Not allowed here.
          if (s[i] == '*') break;
          ++i;
      }
      */
      // Nadia: for-loop is better.
      for (i = 7; true; ++i)
      {
          if (i == s.size()) return false;
          if (s[i] == '$') return false; // Not allowed here.
          if (s[i] == '*') break;
      }
      // cout << "Debug D" << endl;
      if (s.size() < i+3) return false; // Note: i+3 NOT i+2.  This is confusing.
      c = s[i+1];
      if ((c < 48) || (c > 57 && c < 65) || (c > 70 && c < 97) || (c > 102)) return false;
      c = s[i+2];
      if ((c < 48) || (c > 57 && c < 65) || (c > 70 && c < 97) || (c > 102)) return false;
      // cout << "Debug E" << endl;
      // cout << s.size() << endl;
      // cout << i << endl;
      if (s.size() != i+3) return false; // Note: i+3 NOT i+2.  This is confusing.
      // Alice: It's because indexing starts at 0, so the index number is one less.
      // Nadia: No, it's because we're using '<' rather than '<='.  We need a char at i+2, so we say < i+3.
      // Ken: But indexing normally starts at 0, and we normally use <. So why is this different?
      return true;
  }

  bool hasCorrectChecksum(std::string)
  {
      // Stub definition, needs implementing
      return false;
  }

  SentenceData parseSingleSentence(std::string)
  {
      // Stub definition, needs implementing
      return {"", "", {}};
  }

  bool hasCorrectNumberOfFields(SentenceData)
  {
      // Stub definition, needs implementing
      return false;
  }

  Position interpretSentenceData(SentenceData d)
  {
      using namespace std; // Ken: Writing 'std::' everywhere is irritating.
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

  std::vector<Position> parseValidSentences(std::istream &)
  {
      // Stub definition, needs implementing
      return {};
  }

}
