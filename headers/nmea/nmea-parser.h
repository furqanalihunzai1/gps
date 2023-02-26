#ifndef GPS_NMEA_PARSER_H
#define GPS_NMEA_PARSER_H

#include <string>
#include <vector>
#include <istream>

#include "position.h"

namespace GPS::NMEA
{
  /* Determine whether the parameter is the three-character code for a sentence format
   * that is currently supported.
   *
   * Currently the only supported sentence formats are "GLL", "WPL", "RMC", and "GGA".
   */
  bool isSupportedFormat(std::string);


  /* Determine whether a string conforms to the structure of a single NMEA sentence.
   * A NMEA sentence contains the following contents:
   *   - The first character is always the '$' symbol;
   *   - followed by two characters indicating the type of transmitting device,
   *     which consist of an uppercase letter 'G' followed by any uppercase English letter;
   *   - followed by a sequence of three uppercase (English) alphabet characters identifying
   *     the sentence format;
   *   - followed by a sequence of one or more comma-prefixed data fields;
   *   - followed by a '*' character;
   *   - followed by a two-character hexadecimal checksum.
   *
   * Data fields may contain any characters except '$' or '*', and may be empty.
   *
   * For sentences that do not match this structure, this function returns false.
   *
   * Note that this function does NOT check whether the type of transmitting device is
   * recognised, or whether the sentence format is supported.
   */
  bool isSingleSentence(std::string);


  /* A NMEA sentence stores a two-digit hexadecimal checksum at the end of the sentence
   * (after the '*'). The checksum value should be the ASCII character code (in hexadecimal)
   * of the bitwise XOR reduction of the character codes of all characters between the '$'
   * and the '*' (exclusive).
   *
   * This function verifies whether the checksum is correct for the sentence contents.
   *
   * Pre-condition: the argument string must conform to the structure of a single NMEA sentence.
   * Non-conforming arguments cause undefined behaviour.
   */
  bool hasCorrectChecksum(std::string);


  /* Stores the transmitter type, sentence format, and fields of a NMEA sentence (the checksum
   * is not stored).
   */
  struct SentenceData
  {
      /* Stores the type of transmitting device.
       * E.g. "GP"
       */
      std::string transmitter;

      /* Stores the NMEA sentence format.
       * E.g. "GLL".
       */
      std::string format;

      /* Stores the data fields.
       * E.g.
       *  - the first element of the vector could be "5425.32",
       *  - the second element could be "N".
       */
      std::vector<std::string> dataFields;
  };


  /* Extracts the transmitter type, sentence format, and field contents from a NMEA sentence
   * string.  The '$', '*' and the checksum are ignored.
   *
   * Pre-condition: the argument string must conform to the structure of a single NMEA sentence.
   * Non-conforming arguments cause undefined behaviour.
   */
  SentenceData parseSingleSentence(std::string);


  /* Check whether the sentence data contains the correct number of fields for the sentence format.
   *
   * Pre-condition: the sentence data contains a supported format.
   * Unsupported formats cause undefined behaviour.
   */
  bool hasCorrectNumberOfFields(SentenceData);


  /* Interprets the data from a NMEA sentence and constructs a Position object.
   * Currently only supports the GLL, GGA and RMC sentence formats.
   * If the format does not include elevation data, then the elevation is set to zero.
   *
   * Throws a std::domain_error exception if the necessary data fields contain invalid data.
   *
   * Pre-conditions:
   *   - the sentence data contains a supported format;
   *   - the sentence data contains the correct number of fields for that format.
   * Unsupported formats or incorrect numbers of fields cause undefined behaviour.
   */
  Position interpretSentenceData(SentenceData);


  /* Parses a stream of NMEA sentences (one sentence per line), and constructs a vector of
   * Position objects, ignoring any lines that do not contain valid sentences.
   *
   * A line is a valid sentence if all of the following are true:
   *  - the line conforms to the structure of a single NMEA sentence;
   *  - the sentence stores the correct checksum value;
   *  - the sentence format is supported (currently GLL, GGA and RMC);
   *  - the sentence has the correct number of fields;
   *  - the necessary fields contain valid data.
   */
  std::vector<Position> parseValidSentences(std::istream &);

}

#endif
