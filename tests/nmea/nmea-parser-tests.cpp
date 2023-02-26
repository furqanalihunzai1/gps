#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE NMEA_Parser_Tests
#include <boost/test/unit_test.hpp>

#include <string>
#include <stdexcept>
#include <vector>
#include <utility>
#include <ostream>
#include <fstream>
#include <sstream>

#include "dataFiles.h"
#include "nmea/ddm.h"

#include "nmea/nmea-parser.h"

using namespace GPS;
using namespace NMEA;

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( IsSupportedFormat )

BOOST_AUTO_TEST_CASE( SupportedFormats )
{
    BOOST_CHECK( isSupportedFormat("GLL") );
    BOOST_CHECK( isSupportedFormat("WPL") );
    BOOST_CHECK( isSupportedFormat("RMC") );
    BOOST_CHECK( isSupportedFormat("GGA") );
}

BOOST_AUTO_TEST_CASE( UnupportedFormatsThatExist )
{
    BOOST_CHECK( ! isSupportedFormat("BOD") );
    BOOST_CHECK( ! isSupportedFormat("ZDA") );
}

BOOST_AUTO_TEST_CASE( UnupportedFormatsHypothetical )
{
    BOOST_CHECK( ! isSupportedFormat("XXX") );
}

BOOST_AUTO_TEST_CASE( UnupportedFormatsWrongLength )
{
    BOOST_CHECK( ! isSupportedFormat("") );
    BOOST_CHECK( ! isSupportedFormat("G") );
    BOOST_CHECK( ! isSupportedFormat("GL") );
    BOOST_CHECK( ! isSupportedFormat("GLLL") ); // prefix is valid format
    BOOST_CHECK( ! isSupportedFormat("GGGA") ); // suffix is valid format
}

BOOST_AUTO_TEST_CASE( UnupportedFormatsVeryLong )
{
    const std::string veryLongString(10000,'X');
    BOOST_CHECK( ! isSupportedFormat(veryLongString) );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( IsSingleSentence )

BOOST_AUTO_TEST_CASE( ValidOneField )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,1*23") );
}

BOOST_AUTO_TEST_CASE( ValidTwoFields )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,1,testing*69") );
}

BOOST_AUTO_TEST_CASE( ValidEmptyField )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,*47") );
}

BOOST_AUTO_TEST_CASE( ValidManyFields )
{
    const std::string commas(1000,','); // 1000 fields
    BOOST_CHECK( isSingleSentence("$GPXXX" + commas + "*28") );
}

BOOST_AUTO_TEST_CASE( ValidEdgeLetters )
{
    BOOST_CHECK( isSingleSentence("$GPAAA,1*23") );
    BOOST_CHECK( isSingleSentence("$GPZZZ,1*23") );
}

BOOST_AUTO_TEST_CASE( ValidEdgeHexDecimalDigits )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,1*09") );
    BOOST_CHECK( isSingleSentence("$GPXXX,1*90") );
}

BOOST_AUTO_TEST_CASE( ValidLowercaseHexCharacters )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,*ce") );
}

BOOST_AUTO_TEST_CASE( ValidUppercaseHexCharacters )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,*DB") );
}

BOOST_AUTO_TEST_CASE( ValidHexCharactersMix )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,*b6") );
    BOOST_CHECK( isSingleSentence("$GPXXX,*3E") );
    BOOST_CHECK( isSingleSentence("$GPXXX,*cD") );
}

BOOST_AUTO_TEST_CASE( ValidEdgeHexDigits )
{
    BOOST_CHECK( isSingleSentence("$GPXXX,*af") );
    BOOST_CHECK( isSingleSentence("$GPXXX,*FA") );
}

BOOST_AUTO_TEST_CASE( ValidTypicalSentences )
{
    BOOST_CHECK( isSingleSentence("$GPGLL,5425.31,N,107.03,W,82610*69") );
    BOOST_CHECK( isSingleSentence("$GPWPL,5128.62,N,00027.58,W,EGLL*59") );
    BOOST_CHECK( isSingleSentence("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62") );
    BOOST_CHECK( isSingleSentence("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*40") );
    BOOST_CHECK( isSingleSentence("$GPMSS,55,27,318.0,100,*66") );
}

BOOST_AUTO_TEST_CASE( ValidTransmitterOtherThanGP )
{                                                   // GP: GPS
    BOOST_CHECK( isSingleSentence("$GLXXX,1*23") ); // GL: GLONASS
    BOOST_CHECK( isSingleSentence("$GAXXX,1*23") ); // GA: Galileo
    BOOST_CHECK( isSingleSentence("$GBXXX,1*23") ); // GB: BeiDou
    BOOST_CHECK( isSingleSentence("$GIXXX,1*23") ); // GA: NavIC
    BOOST_CHECK( isSingleSentence("$GQXXX,1*23") ); // GB: QZSS
    BOOST_CHECK( isSingleSentence("$GNXXX,1*23") ); // GN: combined
}

BOOST_AUTO_TEST_CASE( InvalidMissingSuffix )
{
    BOOST_CHECK( ! isSingleSentence("") );
    BOOST_CHECK( ! isSingleSentence("$") );
    BOOST_CHECK( ! isSingleSentence("$G") );
    BOOST_CHECK( ! isSingleSentence("$GP") );
    BOOST_CHECK( ! isSingleSentence("$GPG") );
    BOOST_CHECK( ! isSingleSentence("$GPGL") );
    BOOST_CHECK( ! isSingleSentence("$GPGLL") );
    BOOST_CHECK( ! isSingleSentence("$GPGLL,") );
    BOOST_CHECK( ! isSingleSentence("$GPGLL,*") );
    BOOST_CHECK( ! isSingleSentence("$GPGLL,*1") );
}

BOOST_AUTO_TEST_CASE( InvalidStartSymbol )
{
    BOOST_CHECK( ! isSingleSentence("SGPXXX,*01") );
    BOOST_CHECK( ! isSingleSentence( "GPXXX,*01") );
}

BOOST_AUTO_TEST_CASE( InvalidLettersPrefix )
{
    BOOST_CHECK( ! isSingleSentence("$HPXXX,*01") );
    BOOST_CHECK( ! isSingleSentence("$PXXX,*01") );
    BOOST_CHECK( ! isSingleSentence("$GXXX,*01") );
    BOOST_CHECK( ! isSingleSentence("$KLXXX,*01") );
    BOOST_CHECK( ! isSingleSentence("$XXX,*01") );
}

BOOST_AUTO_TEST_CASE( InvalidFormatCode )
{
    BOOST_CHECK( ! isSingleSentence("$GP,*01") );
    BOOST_CHECK( ! isSingleSentence("$GPX,*01") );
    BOOST_CHECK( ! isSingleSentence("$GPXX,*01") );
    BOOST_CHECK( ! isSingleSentence("$GP1XX,*01") );
    BOOST_CHECK( ! isSingleSentence("$GPX%X,*01") );
    BOOST_CHECK( ! isSingleSentence("$GPXX ,*01") );
    BOOST_CHECK( ! isSingleSentence("$GPabc,*01") );
}

BOOST_AUTO_TEST_CASE( InvalidNoFields )
{
    BOOST_CHECK( ! isSingleSentence("$GPXXX*01") );
}

BOOST_AUTO_TEST_CASE( InvalidReservedCharInField )
{
    BOOST_CHECK( ! isSingleSentence("$GPXXX,$77*01") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,2*3,1*77") );
}

BOOST_AUTO_TEST_CASE( InvalidChecksumPrefix )
{
    BOOST_CHECK( ! isSingleSentence("$GPXX,X77") );
    BOOST_CHECK( ! isSingleSentence("$GPXX,X%77") );
}

BOOST_AUTO_TEST_CASE( InvalidChecksumTooLong )
{
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*012") );
}

// There are some other ASCII symbols between the 0-9 characters and the A-Z range,
// and then some more between A-Z and the a-z range.
BOOST_AUTO_TEST_CASE( InvalidHexSymbolsFromMiddleOfASCIIRange )
{
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*=3") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*4?") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*^5") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*8-") );
}

// ASCII symbols taken from the edges just outside the ASCII ranges of valid hex characters.
BOOST_AUTO_TEST_CASE( InvalidHexSymbolsASCIIBoundaries )
{
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*2:") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*@9") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*7G") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*`8") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*1/") );
    BOOST_CHECK( ! isSingleSentence("$GPXXX,*g3") );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( HasCorrectChecksum )

BOOST_AUTO_TEST_CASE( MatchMinimalSentence )
{
    BOOST_CHECK( hasCorrectChecksum("$GPXXX,*63") );
}

BOOST_AUTO_TEST_CASE( MismatchMinimalSentence )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPAAA,*55") );
}

BOOST_AUTO_TEST_CASE( MatchTypicalGLL )
{
    BOOST_CHECK( hasCorrectChecksum("$GPGLL,5425.31,N,107.03,W,82610*69") );
}

BOOST_AUTO_TEST_CASE( MatchTypicalWPL )
{
    BOOST_CHECK( hasCorrectChecksum("$GPWPL,5128.62,N,00027.58,W,EGLL*59") );
}

BOOST_AUTO_TEST_CASE( MatchTypicalRMC )
{
    BOOST_CHECK( hasCorrectChecksum("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62") );
}

BOOST_AUTO_TEST_CASE( MatchTypicalGGA )
{
    BOOST_CHECK( hasCorrectChecksum("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*40") );
}

BOOST_AUTO_TEST_CASE( MismatchTypicalGLL )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPGLL,5425.31,N,107.03,W,82610*24") );
}

BOOST_AUTO_TEST_CASE( MismatchTypicalWPL )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPWPL,5128.62,N,00027.58,W,EGLL*48") );
}

BOOST_AUTO_TEST_CASE( MismatchTypicalRMC )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*97") );
}

BOOST_AUTO_TEST_CASE( MismatchTypicalGGA )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*41") );
}

BOOST_AUTO_TEST_CASE( MatchWithUppercaseHexDigit )
{
    BOOST_CHECK( hasCorrectChecksum("$GPAAA,*7A") );
}

BOOST_AUTO_TEST_CASE( MatchWithLowercaseHexDigit )
{
    BOOST_CHECK( hasCorrectChecksum("$GPAAA,*7a") );
}

BOOST_AUTO_TEST_CASE( MismatchWithUppercaseHexDigit )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPAAE,*5F") );
}

BOOST_AUTO_TEST_CASE( MismatchWithLowercaseHexDigit )
{
    BOOST_CHECK( ! hasCorrectChecksum("$GPAAE,*5f") );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( ParseSingleSentence )

/////////////////////////////////////////////////////////////////////////

/* Displaying vectors directly using the built-in BOOST macros is tricky.
 * For reasons I don't fully understand, just overloading operator<< here doesn't work.
 * See: https://stackoverflow.com/questions/17572583/boost-check-fails-to-compile-operator-for-custom-types
 *
 * So instead we introduce an auxilliary function for formatting the error message for mismatched vectors.
 */

std::ostream& operator<<(std::ostream& outputStream, const std::vector<std::string> & vec)
{
    outputStream << '{';
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) outputStream << ',';
        outputStream << '"' << *it << '"';
    }
    outputStream << '}';
    return outputStream;
}

std::string formatMismatchedFormatString(std::string actualString, std::string expectedString)
{
    std::stringstream outputMessage;
    outputMessage << "parseSentenceData() has failed, mismatched format string [ \"" << actualString << "\" != \"" << expectedString << "\" ]";
    return outputMessage.str();
}

std::string formatMismatchedFieldData(const std::vector<std::string> & actualFields,
                                      const std::vector<std::string> & expectedFields)
{
    std::stringstream outputMessage;
    outputMessage << "parseSentenceData() has failed, mismatched fields [ " << actualFields << " != " << expectedFields << " ]";
    return outputMessage.str();
}

void checkSentenceDataEqual(const SentenceData & actual,
                            const SentenceData & expected)
{
    BOOST_CHECK_MESSAGE( actual.format == expected.format,
                         formatMismatchedFormatString(actual.format, expected.format) );

    BOOST_CHECK_MESSAGE( actual.dataFields == expected.dataFields,
                         formatMismatchedFieldData(actual.dataFields,expected.dataFields) );
}

////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( OneField )
{
    const std::string sentence = "$GPAAA,1*4b";
    const SentenceData expectedSentenceData = { "GP", "AAA", {"1"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( TwoFields )
{
    const std::string sentence = "$GPAAA,1,testing*11";
    const SentenceData expectedSentenceData = { "GP", "AAA", {"1","testing"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( GLL )
{
    const std::string sentence = "$GPGLL,5425.31,N,107.03,W,82610*69";
    const SentenceData expectedSentenceData = { "GP", "GLL", {"5425.31","N","107.03","W","82610"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( WPL )
{
    const std::string sentence = "$GPWPL,5128.62,N,00027.58,W,EGLL*59";
    const SentenceData expectedSentenceData = { "GP", "WPL", {"5128.62","N","00027.58","W","EGLL"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( RMC )
{
    const std::string sentence = "$GPRMC,115856.000,A,3722.6710,N,00559.3014,W,0.000,0.00,150914,,A*6d";
    const SentenceData expectedSentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","W","0.000","0.00","150914","","A"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( GGA )
{
    const std::string sentence = "$GPGGA,114530.000,3722.6279,N,00559.1566,W,1,0,,1.0,M,,M,,*4E";
    const SentenceData expectedSentenceData = { "GP", "GGA", {"114530.000","3722.6279","N","00559.1566","W","1","0","","1.0","M","","M","",""} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( MSS )
{
    const std::string sentence = "$GPMSS,55,27,318.0,100,*66";
    const SentenceData expectedSentenceData = { "GP", "MSS", {"55","27","318.0","100",""} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( TransmitterOtherThanGP )
{
    const std::string sentence = "$GLGLL,5425.31,N,107.03,W,82610*FF";
    const SentenceData expectedSentenceData = { "GL", "GLL", {"5425.31","N","107.03","W","82610"} };

    SentenceData actualSentenceData = parseSingleSentence(sentence);

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( HasCorrectNumberOfFields )

BOOST_AUTO_TEST_CASE( CorrectGLL )
{
    // GLL should have 5 fields
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","N","107.03","E","82610"} };

    BOOST_CHECK( hasCorrectNumberOfFields(sentenceData) );
}

BOOST_AUTO_TEST_CASE( CorrectWPL )
{
    // WPL should have 5 fields
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","N","00027.58","W","EGLL"} };

    BOOST_CHECK( hasCorrectNumberOfFields(sentenceData) );
}

BOOST_AUTO_TEST_CASE( CorrectRMC )
{
    // RMC should have 11 fields
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","S","00559.3014","E","0.000","0.00","150914","","A"} };

    BOOST_CHECK( hasCorrectNumberOfFields(sentenceData) );
}

BOOST_AUTO_TEST_CASE( CorrectGGA )
{
    // GGA should have 14 fields
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };

    BOOST_CHECK( hasCorrectNumberOfFields(sentenceData) );
}

BOOST_AUTO_TEST_CASE( MultipleMissingFields )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","N","00559.3014","0.000","0.00","","A"} };

    BOOST_CHECK( ! hasCorrectNumberOfFields(sentenceData));
}

BOOST_AUTO_TEST_CASE( ZeroFields )
{
    const SentenceData sentenceData = { "GP", "GLL", {} };

    BOOST_CHECK( ! hasCorrectNumberOfFields(sentenceData));
}

BOOST_AUTO_TEST_CASE( OneMissingField )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","280.2","M","-34.0","M",""} };

    BOOST_CHECK( ! hasCorrectNumberOfFields(sentenceData));
}

BOOST_AUTO_TEST_CASE( OneTooManyFields )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","N","107.03","E","82610","0"} };

    BOOST_CHECK( ! hasCorrectNumberOfFields(sentenceData) );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( InterpretSentenceData )

const double epsilon = 0.0001;
const double percentageAccuracy = 0.0001;
const double absoluteAccuracy = 0.0001;

BOOST_AUTO_TEST_CASE( GLL_NW )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","N","107.03","W","82610"} };
    const degrees expectedLatitude = ddmTodd("5425.31");
    const degrees expectedLongitude = -ddmTodd("107.03");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GLL_NE )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","N","107.03","E","82610"} };
    const degrees expectedLatitude = ddmTodd("5425.31");
    const degrees expectedLongitude = ddmTodd("107.03");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GLL_SE )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","S","107.03","E","82610"} };
    const degrees expectedLatitude = -ddmTodd("5425.31");
    const degrees expectedLongitude = ddmTodd("107.03");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GLL_SW )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","S","107.03","W","82610"} };
    const degrees expectedLatitude = -ddmTodd("5425.31");
    const degrees expectedLongitude = -ddmTodd("107.03");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GLL_ElevationDefault )
{
    const SentenceData sentenceData = { "GP", "GLL", {"5425.31","S","107.03","W","82610"} };

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_SMALL( actualPos.elevation() , epsilon ); // Elevation should default to 0
}

BOOST_AUTO_TEST_CASE( WPL_NW )
{
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","N","00027.58","W","EGLL"} };
    const degrees expectedLatitude = ddmTodd("5128.62");
    const degrees expectedLongitude = -ddmTodd("00027.58");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( WPL_NE )
{
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","N","00027.58","E","EGLL"} };
    const degrees expectedLatitude = ddmTodd("5128.62");
    const degrees expectedLongitude = ddmTodd("00027.58");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( WPL_SE )
{
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","S","00027.58","E","EGLL"} };
    const degrees expectedLatitude = -ddmTodd("5128.62");
    const degrees expectedLongitude = ddmTodd("00027.58");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( WPL_SW )
{
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","S","00027.58","W","EGLL"} };
    const degrees expectedLatitude = -ddmTodd("5128.62");
    const degrees expectedLongitude = -ddmTodd("00027.58");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( WPL_ElevationDefault )
{
    const SentenceData sentenceData = { "GP", "WPL", {"5128.62","S","00027.58","E","EGLL"} };

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_SMALL( actualPos.elevation() , epsilon ); // Elevation should default to 0
}

BOOST_AUTO_TEST_CASE( RMC_NW )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","W","0.000","0.00","150914","","A"} };
    const degrees expectedLatitude = ddmTodd("3722.6710");
    const degrees expectedLongitude = -ddmTodd("00559.3014");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( RMC_NE )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","E","0.000","0.00","150914","","A"} };
    const degrees expectedLatitude = ddmTodd("3722.6710");
    const degrees expectedLongitude = ddmTodd("00559.3014");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( RMC_SE )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","S","00559.3014","E","0.000","0.00","150914","","A"} };
    const degrees expectedLatitude = -ddmTodd("3722.6710");
    const degrees expectedLongitude = ddmTodd("00559.3014");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( RMC_SW )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","S","00559.3014","W","0.000","0.00","150914","","A"} };
    const degrees expectedLatitude = -ddmTodd("3722.6710");
    const degrees expectedLongitude = -ddmTodd("00559.3014");

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( RMC_ElevationDefault )
{
    const SentenceData sentenceData = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","E","0.000","0.00","150914","","A"} };

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_SMALL( actualPos.elevation() , epsilon ); // Elevation should default to 0
}

BOOST_AUTO_TEST_CASE( GGA_NW )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };
    const degrees expectedLatitude = ddmTodd("4124.8963");
    const degrees expectedLongitude = -ddmTodd("08151.6838");
    const metres expectedElevation = 280.2;

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.elevation() , expectedElevation , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_NE )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","N","08151.6838","E","1","05","1.5","280.2","M","-34.0","M","",""} };
    const degrees expectedLatitude = ddmTodd("4124.8963");
    const degrees expectedLongitude = ddmTodd("08151.6838");
    const metres expectedElevation = 280.2;

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.elevation() , expectedElevation , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_SE )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","S","08151.6838","E","1","05","1.5","280.2","M","-34.0","M","",""} };
    const degrees expectedLatitude = -ddmTodd("4124.8963");
    const degrees expectedLongitude = ddmTodd("08151.6838");
    const metres expectedElevation = 280.2;

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.elevation() , expectedElevation , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_SW )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","S","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };
    const degrees expectedLatitude = -ddmTodd("4124.8963");
    const degrees expectedLongitude = -ddmTodd("08151.6838");
    const metres expectedElevation = 280.2;

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.elevation() , expectedElevation , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( NegativeElevation )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","4124.8963","S","08151.6838","E","1","05","1.5","-280.2","M","-34.0","M","",""} };
    const degrees expectedLatitude = -ddmTodd("4124.8963");
    const degrees expectedLongitude = ddmTodd("08151.6838");
    const metres expectedElevation = -280.2;

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_CLOSE( actualPos.latitude() , expectedLatitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude() , expectedLongitude , percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.elevation() , expectedElevation , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( ZeroValues )
{
    const SentenceData sentenceData = { "GP", "GGA", {"170834","0000.0000","S","00000.0000","E","1","05","1.5","0.0","M","-34.0","M","",""} };

    Position actualPos = interpretSentenceData(sentenceData);

    BOOST_CHECK_SMALL( actualPos.latitude() , absoluteAccuracy );
    BOOST_CHECK_SMALL( actualPos.longitude() , absoluteAccuracy );
    BOOST_CHECK_SMALL( actualPos.elevation() , absoluteAccuracy );
}

BOOST_AUTO_TEST_CASE( InvalidLatitude_Text )
{
    const SentenceData invalidGLL_latitude = { "GP", "GLL", {"three","N","107.03","W","82610"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGLL_latitude) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLatitude_Negative )
{
    const SentenceData invalidGLL_latitude = { "GP", "GLL", {"-4124.8963","N","107.03","W","82610"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGLL_latitude) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLongitude_Symbols )
{
    const SentenceData invalidRMC_longitude = { "GP", "RMC", {"115856.000","A","3722.6710","N","?&*","W","0.000","0.00","150914","","A"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidRMC_longitude) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLongitude_Negative )
{
    const SentenceData invalidRMC_longitude = { "GP", "RMC", {"115856.000","A","3722.6710","N","-00559.3014","W","0.000","0.00","150914","","A"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidRMC_longitude) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidElevation )
{
    const SentenceData invalidGGA_elevation = { "GP", "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","zero","M","-34.0","M","",""} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGGA_elevation) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLatBearingSymbol )
{
    const SentenceData invalidGLL_northing = { "GP", "GLL", {"5425.31","X","107.03","W","82610"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGLL_northing) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLatBearingCase )
{
    const SentenceData invalidGLL_northing = { "GP", "GLL", {"5425.31","n","107.03","W","82610"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGLL_northing) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLatBearingWithValidPrefix )
{
    const SentenceData invalidGLL_northing = { "GP", "GLL", {"5425.31","NO","107.03","W","82610"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidGLL_northing) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLonBearingSymbol )
{
    const SentenceData invalidRMC_easting = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","-","0.000","0.00","150914","","A"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidRMC_easting) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLonBearingCase )
{
    const SentenceData invalidRMC_easting = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","e","0.000","0.00","150914","","A"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidRMC_easting) , std::domain_error );
}

BOOST_AUTO_TEST_CASE( InvalidLonBearingWithValidPrefix )
{
    const SentenceData invalidRMC_easting = { "GP", "RMC", {"115856.000","A","3722.6710","N","00559.3014","West","0.000","0.00","150914","","A"} };

    BOOST_CHECK_THROW( interpretSentenceData(invalidRMC_easting) , std::domain_error );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( ParseValidSentences )

const double percentageAccuracy = 0.0001;

const std::string validGLLSentence = "$GPGLL,5425.31,N,107.03,W,82610*69";
const std::string validRMCSentence = "$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62";
const std::string validGGASentence = "$GPGGA,114530.000,3722.6279,N,00559.1566,W,1,0,,1.0,M,,M,,*4E";
const std::string validMSSSentence = "$GPMSS,55,27,318.0,100,*66";

const GPS::Position gllPos = GPS::Position(ddmTodd("5425.31"),-ddmTodd("107.03"),0);
const GPS::Position rmcPos = GPS::Position(ddmTodd("3722.5993"),-ddmTodd("00559.2458"),0);
const GPS::Position ggaPos = GPS::Position(ddmTodd("3722.6279"),-ddmTodd("00559.1566"),1);

BOOST_AUTO_TEST_CASE( EmptyStream )
{
    std::stringstream sentences("");
    const unsigned int expectedSize = 0;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( OneValidSentence )
{
    std::stringstream sentences;
    sentences << validGLLSentence << std::endl;
    const unsigned int expectedSize = 1;
    const Position expectedPos = gllPos;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_REQUIRE_EQUAL( positions.size() , expectedSize );
    const Position actualPos = positions.front();
    BOOST_CHECK_CLOSE( actualPos.latitude(), expectedPos.latitude(), percentageAccuracy );
    BOOST_CHECK_CLOSE( actualPos.longitude(), expectedPos.longitude(), percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( TwoValidSentences )
{
    std::stringstream sentences;
    sentences << validGGASentence << std::endl;
    sentences << validRMCSentence << std::endl;
    const unsigned int expectedSize = 2;
    const Position expectedGGAPos = ggaPos;
    const Position expectedRMCPos = rmcPos;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_REQUIRE_EQUAL( positions.size() , expectedSize );
    const Position actualGGAPos = positions.front();
    const Position actualRMCPos = positions.back();
    BOOST_CHECK_CLOSE( actualGGAPos.latitude(), expectedGGAPos.latitude(), percentageAccuracy );
    BOOST_CHECK_CLOSE( actualGGAPos.longitude(), expectedGGAPos.longitude(), percentageAccuracy );
    BOOST_CHECK_CLOSE( actualGGAPos.elevation(), expectedGGAPos.elevation(), percentageAccuracy );
    BOOST_CHECK_CLOSE( actualRMCPos.latitude(), expectedRMCPos.latitude(), percentageAccuracy );
    BOOST_CHECK_CLOSE( actualRMCPos.longitude(), expectedRMCPos.longitude(), percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( StreamWithBlankLines )
{
    std::stringstream sentences;
    sentences << std::endl << std::endl;
    sentences << validGLLSentence << std::endl;
    sentences << validGGASentence << std::endl;
    sentences << std::endl;
    sentences << validRMCSentence << std::endl;
    sentences << std::endl << std::endl << std::endl;
    const unsigned int expectedSize = 3;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( StreamWithoutLineBreaks )
{
    std::stringstream sentences;
    sentences << validGLLSentence << validRMCSentence << std::endl;
    const unsigned int expectedSize = 0;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( InvalidSentenceStructures )
{
    std::stringstream sentences;
    sentences << "Arbitrary meta-data" << std::endl;
    sentences << validGLLSentence << std::endl;
    sentences << "$XXX*01" << std::endl;
    sentences << validGGASentence << std::endl;
    sentences << "$GPGLL" << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( InvalidChecksums )
{
    std::stringstream sentences;
    sentences << "$GPGLL,5425.31,N,107.03,W,82610*24" << std::endl;
    sentences << validGLLSentence << std::endl;
    sentences << "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*41" << std::endl;
    sentences << validRMCSentence << std::endl;
    sentences << "$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*97" << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( InvalidStructureButMatchingChecksum )
{
    std::stringstream sentences;
    sentences << validGLLSentence << std::endl;
    sentences << "$GX*1F" << std::endl;
    sentences << validGGASentence << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( UnsupportedSentenceFormats )
{
    std::stringstream sentences;
    sentences << validGLLSentence << std::endl;
    sentences << validMSSSentence << std::endl; // unsupported format
    sentences << validRMCSentence << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( MissingFields )
{
    const std::string gllSentenceWithMissingFields = "$GPGLL,5425.31,107.03,W,82610*0B";
    const std::string rmcSentenceWithMissingFields = "$GPRMC,113922.000,A,3722.5993,N,00559.2458*4C";
    const std::string ggaSentenceWithMissingFields = "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0*46";

    std::stringstream sentences;
    sentences << gllSentenceWithMissingFields << std::endl;
    sentences << validGLLSentence << std::endl;
    sentences << rmcSentenceWithMissingFields << std::endl;
    sentences << validRMCSentence << std::endl;
    sentences << ggaSentenceWithMissingFields << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

BOOST_AUTO_TEST_CASE( InvalidFields )
{
    std::stringstream sentences;
    sentences << "$GPGLL,5425.31,X,107.03,W,82610*7F" << std::endl;
    sentences << "$GPGLL,fivethousand,N,107.03,W,82610*41" << std::endl;
    sentences << validGLLSentence << std::endl;
    sentences << "$GPRMC,113922.000,A,3722.5993,N,00559.2458,7,0.000,0.00,150914,,A*02" << std::endl;
    sentences << "$GPRMC,113922.000,A,3722.5993,N,???,W,0.000,0.00,150914,,A*41" << std::endl;
    sentences << validRMCSentence << std::endl;
    sentences << "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,high,M,,M,,*64" << std::endl;
    const unsigned int expectedSize = 2;

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_CHECK_EQUAL( positions.size() , expectedSize );
}

std::fstream openNMEAfile(std::string filename)
{
    std::string dataFilepath = DataFiles::NMEADir + filename;
    std::fstream sentences{dataFilepath};
    BOOST_REQUIRE_MESSAGE( sentences.good() ,
      ("Could not open NMEA data file: " + dataFilepath +
       "\n(If you're running at the command-line, you need to 'cd' into the 'bin/' directory first.)") );
    return sentences;
}

BOOST_AUTO_TEST_CASE( LargeFileGLL )
{
    std::fstream sentences = openNMEAfile("gll.log");
    const unsigned int expectedSize = 1090;

    // Pos 0: $GPGLL,5425.32,N,107.11,W,82319*65
    const degrees expectedLatitudePos0 = ddmTodd("5425.32");
    const degrees expectedLongitudePos0 = -ddmTodd("107.11");

    // Pos 1000: $GPGLL,5430.32,N,106.39,W,154912*51
    const degrees expectedLatitudePos1000 = ddmTodd("5430.32");
    const degrees expectedLongitudePos1000 = -ddmTodd("106.39");

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_REQUIRE_EQUAL( positions.size() , expectedSize );

    BOOST_CHECK_CLOSE( positions[0].latitude() , expectedLatitudePos0 , percentageAccuracy );
    BOOST_CHECK_CLOSE( positions[0].longitude() , expectedLongitudePos0 , percentageAccuracy );

    BOOST_CHECK_CLOSE( positions[1000].latitude() , expectedLatitudePos1000 , percentageAccuracy );
    BOOST_CHECK_CLOSE( positions[1000].longitude() , expectedLongitudePos1000 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( LargeFileGGA_RMC )
{   
    std::fstream sentences = openNMEAfile("gga_rmc-1.log");
    const unsigned int expectedSize = 632; // The header and blank line should be discarded

    // Pos 0: $GPGGA,094627.000,3723.1622,N,00559.5788,W,1,0,,30.0,M,,M,,*7A
    const degrees expectedLatitudePos0 = ddmTodd("3723.1622");
    const degrees expectedLongitudePos0 = -ddmTodd("00559.5788");
    const metres expectedElevationPos0 = 30.0;

    // Pos 501: $GPRMC,113720.000,A,3722.5563,N,00559.2403,W,0.000,0.00,150914,,A*63
    const degrees expectedLatitudePos501 = ddmTodd("3722.5563");
    const degrees expectedLongitudePos501 = -ddmTodd("00559.2403");

    std::vector<Position> positions = parseValidSentences(sentences);

    BOOST_REQUIRE_EQUAL( positions.size() , expectedSize );

    BOOST_CHECK_CLOSE( positions[0].latitude(),  expectedLatitudePos0,  percentageAccuracy );
    BOOST_CHECK_CLOSE( positions[0].longitude(), expectedLongitudePos0, percentageAccuracy );
    BOOST_CHECK_CLOSE( positions[0].elevation(), expectedElevationPos0,  percentageAccuracy );

    BOOST_CHECK_CLOSE( positions[501].latitude(),  expectedLatitudePos501,  percentageAccuracy );
    BOOST_CHECK_CLOSE( positions[501].longitude(), expectedLongitudePos501, percentageAccuracy );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////
