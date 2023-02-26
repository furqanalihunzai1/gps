#include <cmath>

#include "geometry.h"

#include "nmea/ddm.h"

namespace GPS
{

  degrees ddmTodd(std::string ddmStr)
  {
      double ddm  = std::stod(ddmStr);
      degrees wholeDegrees = std::floor(ddm / 100); // 100 for two decimal places
      double mins = ddm - 100 * wholeDegrees;
      degrees fractionsOfADegree = mins / minutesPerDegree; // converts minutes to decimal fractions of a degree
      return wholeDegrees + fractionsOfADegree;
  }

}
