#include <cmath>

#include "geometry.h"

namespace GPS
{
  const unsigned int minutesPerDegree = 60;
  const unsigned int secondsPerMinute = 60;
  const unsigned int degreesInACircle = 360;
  const double pi = 3.141592653589793;
  const degrees fullRotation = degreesInACircle;
  const degrees halfRotation = fullRotation/2;
  const degrees poleLatitude = fullRotation/4;
  const degrees antiMeridianLongitude = fullRotation/2;

  double pythagoras(double x, double y)
  {
      return std::sqrt(x*x + y*y);
  }

  double pythagoras(double x, double y, double z)
  {
      return std::sqrt(x*x + y*y + z*z);
  }

  radians degToRad(degrees d)
  {
      return d * pi / halfRotation;
  }

  degrees radToDeg(radians r)
  {
      return r * halfRotation / pi;
  }

  double sinSqr(radians x)
  {
      const double sx = std::sin(x);
      return sx * sx;
  }

  bool isValidLatitude(degrees lat)
  {
      return std::abs(lat) <= poleLatitude;
  }

  bool isValidLongitude(degrees lon)
  {
      return std::abs(lon) <= antiMeridianLongitude;
  }

  degrees normaliseDegrees(degrees d)
  {
      d = fmod(d,fullRotation); // results in range (-360,360)
      if (d <= -halfRotation) d += fullRotation; // results in range (-180,360)
      if (d > halfRotation) d -= fullRotation; // results in range (-180,180]
      return d;
  }
}
