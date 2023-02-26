#ifndef GPS_GEOMETRY_H
#define GPS_GEOMETRY_H

#include "types.h"

namespace GPS
{
  extern const unsigned int minutesPerDegree;
  extern const unsigned int secondsPerMinute;
  extern const unsigned int degreesInACircle;
  extern const double pi;
  extern const degrees fullRotation;
  extern const degrees halfRotation;
  extern const degrees poleLatitude;
  extern const degrees antiMeridianLongitude;

  // Compute hypotenuse of right-angled triangle in two dimensions.
  double pythagoras(double,double);

  // Compute hypotenuse of right-angled triangle in three dimensions.
  double pythagoras(double,double,double);

  // Convert from degrees to radians.
  radians degToRad(degrees);

  // Convert from radians to degrees.
  radians radToDeg(degrees);

  // Sine squared function: sin^2(x)
  double sinSqr(radians);

  // Check if the angle is within the [-90,90] range.
  bool isValidLatitude(degrees);

  // Check if the angle is within the [-180,180] range.
  bool isValidLongitude(degrees);

  // Convert larger/smaller degrees into the (-180,180] range.
  degrees normaliseDegrees(degrees);
}

#endif
