#ifndef GPS_POSITION_H
#define GPS_POSITION_H

#include <string>

#include "types.h"

namespace GPS
{
  /* A Position object represents a location on, within, or above, the Earth's surface.
   * The location is identifed by degrees latitude, degrees longitude, and elevation.
   * Elevation is relative to the Earth's mean radius, i.e. roughly, but not exactly, the Earth's surface.
   */
  class Position
  {
    public:
      /* Construct a Position from degrees latitude, degrees longitude, and elevation in metres.
       *
       * Throws a std::invalid_argument exception for:
       *   - geometrically invalid latitude or longitude values,
       *   - negative elevation values that exceed the Earth's mean radius,
       *     i.e. those that would be below the centre of the Earth.
       */
      Position(degrees lat, degrees lon, metres ele);

      degrees latitude() const;
      degrees longitude() const;
      metres  elevation() const;

      /* Computes an approximation of the horizontal distance between two Positions on the Earth's surface.
       * Note that this does NOT take into account elevation.
       */
      static metres horizontalDistanceBetween(Position, Position);

      /* Computes an approximation of the vertical distance between two Positions,
       * i.e. the difference in elevations.  This always returns a positive value.
       */
      static metres verticalDistanceBetween(Position, Position);

    private:
      degrees lat;
      degrees lon;
      metres  ele;
  };
}

#endif
