#include <cassert>
#include <cmath>
#include <stdexcept>

#include "geometry.h"
#include "earth.h"
#include "position.h"

namespace GPS
{
  Position::Position(degrees lat, degrees lon, metres ele)
  {
      if (! isValidLatitude(lat))
          throw std::invalid_argument("Latitude values must not exceed " + std::to_string(poleLatitude) + " degrees.");

      if (! isValidLongitude(lon))
          throw std::invalid_argument("Longitude values must not exceed " + std::to_string(antiMeridianLongitude) + " degrees.");

      if (! Earth::isValidElevation(ele))
          throw std::invalid_argument("Negative elevation values must not exceed the Earth's mean radius (i.e. must not be below the centre of the Earth).");

      this->lat = lat;
      this->lon = lon;
      this->ele = ele;
  }

  degrees Position::latitude() const
  {
      return lat;
  }

  degrees Position::longitude() const
  {
      return lon;
  }

  metres Position::elevation() const
  {
      return ele;
  }

  metres Position::horizontalDistanceBetween(Position p1, Position p2)
  /* The Haversine Formula
   * See: https://en.wikipedia.org/wiki/Haversine_formula
   */
  {
      const radians lat1 = degToRad(p1.latitude());
      const radians lat2 = degToRad(p2.latitude());
      const radians lon1 = degToRad(p1.longitude());
      const radians lon2 = degToRad(p2.longitude());

      const double h = sinSqr((lat2-lat1)/2) + std::cos(lat1)*std::cos(lat2)*sinSqr((lon2-lon1)/2);
      return 2 * Earth::meanRadius * std::asin(std::sqrt(h));
  }

  metres Position::verticalDistanceBetween(Position p1, Position p2)
  {
      return std::abs(p2.elevation() - p1.elevation());
  }
}
