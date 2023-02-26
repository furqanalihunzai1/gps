#include <boost/test/unit_test.hpp>

#include "types.h"

#include "nmea/ddm.h"

using namespace GPS;

BOOST_AUTO_TEST_SUITE( DDMtoDD )

const double percentageAccuracy = 0.0001;

BOOST_AUTO_TEST_CASE( wholeDegrees )
{
    const std::string ddm = "3700";
    const degrees expectedDD = 37;

    metres actualDD = ddmTodd(ddm);

    BOOST_CHECK_CLOSE( actualDD, expectedDD, percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( degreesAndWholeMinutes )
{
    const std::string ddm = "6730";
    const degrees expectedDD = 67.5;

    metres actualDD = ddmTodd(ddm);

    BOOST_CHECK_CLOSE( actualDD, expectedDD, percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( degreesAndFractionalMinutes )
{
    const std::string ddm = "7730.90";
    const degrees expectedDD = 77.515;

    metres actualDD = ddmTodd(ddm);

    BOOST_CHECK_CLOSE( actualDD, expectedDD, percentageAccuracy );
}

BOOST_AUTO_TEST_SUITE_END()
