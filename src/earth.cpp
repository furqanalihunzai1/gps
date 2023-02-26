#include <cmath>
#include <cassert>

#include "geometry.h"
#include "earth.h"

namespace GPS
{
  namespace Earth
  {
      const Position NorthPole = Position(poleLatitude,0,0);
      const Position SouthPole = Position(-poleLatitude,0,0);
      const Position EquatorialMeridian = Position(0,0,0);
      const Position EquatorialAntiMeridian = Position(0,antiMeridianLongitude,0);
      const Position CliftonCampus = Position(52.91249953,-1.18402513,58);
      const Position CityCampus = Position(52.9581383,-1.1542364,53);
      const Position Pontianak = Position(0,109.322134,0);

      const metres meanRadius = 6371008.8;
      const metres equatorialCircumference = 40075160;
      const metres polarCircumference = 40008000;

      metres circumferenceAtLatitude(degrees lat)
      {
          assert (isValidLatitude(lat));

          return equatorialCircumference * std::cos(degToRad(lat));
      }

      degrees latitudeSubtendedBy(metres northSouthDistance)
      {
          assert (! (northSouthDistance > polarCircumference));

          return (northSouthDistance / polarCircumference) * fullRotation;
      }

      degrees longitudeSubtendedBy(metres eastWestDistance, degrees lat)
      {
          assert (isValidLatitude(lat));

          metres circumferenceAtThisLatitude = circumferenceAtLatitude(lat);

          assert (eastWestDistance <= circumferenceAtThisLatitude);

          if (circumferenceAtThisLatitude == 0)
          {
              return 0; // No longitude at poles.
          }
          else
          {
              return (eastWestDistance / circumferenceAtThisLatitude) * fullRotation;
          }
      }

      bool isValidElevation(metres elevation)
      {
          return elevation >= -meanRadius;
      }

  }
}
