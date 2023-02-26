#include <string>

#include "types.h"

namespace GPS
{

  /* Convert a DDM (degrees and decimal minutes) string representation of an angle
   * to a numeric DD (decimal degrees) value.
   *
   * Throws an exception if the string is not a well-formed number.
   */
  degrees ddmTodd(std::string);

}
