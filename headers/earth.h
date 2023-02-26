#ifndef GPS_EARTH_H
#define GPS_EARTH_H

#include "position.h"

namespace GPS
{
  namespace Earth
  {
      extern const Position NorthPole;
      extern const Position SouthPole;
      extern const Position EquatorialMeridian;
      extern const Position EquatorialAntiMeridian;
      extern const Position CliftonCampus;
      extern const Position CityCampus;
      extern const Position Pontianak;

      extern const metres meanRadius;
      extern const metres equatorialCircumference;
      extern const metres polarCircumference;


      /* Determine the east/west circumference of the Earth at a specified latitude.
       *
       * Pre-condition: The latitude is a valid latitude angle.
       */
      metres circumferenceAtLatitude(degrees lat);


      /* Determine the change in degrees latitude from a North/South distance (in metres).
       *
       * Pre-condition: The distance is no greater than the Earth's polar circumference.
       */
      degrees latitudeSubtendedBy(metres northSouthDistance);


      /* Determine the change in degrees longitude from an East/West distance (in metres).
       * Unlike North/South changes, the change in degrees longitude also depends on the
       * latitude where the East/West line occurs.
       *
       * Pre-condition: The latitude is a valid latitude angle.
       *
       * Pre-condition: The distance is no greater than the Earth's circumference at the
       * specified latitude.
       */
      degrees longitudeSubtendedBy(metres eastWestDistance, degrees lat);


      /* Check that the elevation value is no lower than the centre of the Earth.
       * No upper limit is imposed.
       */
      bool isValidElevation(metres);

  }
}

#endif

