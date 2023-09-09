#include "inputFileParserManostat.hpp"

#include "exceptions.hpp"         // for InputFileException, customException
#include "manostatSettings.hpp"   // for ManostatSettings

#include <cstddef>       // for size_t
#include <format>        // for format
#include <functional>    // for _Bind_front_t, bind_front
#include <string_view>   // for string_view

using namespace readInput;

/**
 * @brief Construct a new Input File Parser Manostat:: Input File Parser Manostat object
 *
 * @details following keywords are added to the _keywordFuncMap, _keywordRequiredMap and _keywordCountMap:
 * 1) manostat <string>
 * 2) pressure <double> (only required if manostat is not none)
 * 3) p_relaxation <double>
 * 4) compressibility <double>
 *
 * @param engine
 */
InputFileParserManostat::InputFileParserManostat(engine::Engine &engine) : InputFileParser(engine)
{
    addKeyword(std::string("manostat"), bind_front(&InputFileParserManostat::parseManostat, this), false);
    addKeyword(std::string("pressure"), bind_front(&InputFileParserManostat::parsePressure, this), false);
    addKeyword(std::string("p_relaxation"), bind_front(&InputFileParserManostat::parseManostatRelaxationTime, this), false);
    addKeyword(std::string("compressibility"), bind_front(&InputFileParserManostat::parseCompressibility, this), false);
}

/**
 * @brief Parse the manostat used in the simulation
 *
 * @details Possible options are:
 * 1) "none" - no manostat is used (default)
 * 2) "berendsen" - berendsen manostat is used
 *
 * @param lineElements
 *
 * @throws customException::InputFileException if manostat is not berendsen or none
 */
void InputFileParserManostat::parseManostat(const std::vector<std::string> &lineElements, const size_t lineNumber)
{
    checkCommand(lineElements, lineNumber);
    if (lineElements[2] == "none")
        settings::ManostatSettings::setManostatType("none");
    else if (lineElements[2] == "berendsen")
        settings::ManostatSettings::setManostatType("berendsen");
    else
        throw customException::InputFileException(
            format("Invalid manostat \"{}\" at line {} in input file. Possible options are: berendsen and none",
                   lineElements[2],
                   lineNumber));
}

/**
 * @brief Parse the pressure used in the simulation
 *
 * @details no default value - if needed it has to be set in the input file
 *
 * @param lineElements
 */
void InputFileParserManostat::parsePressure(const std::vector<std::string> &lineElements, const size_t lineNumber)
{
    checkCommand(lineElements, lineNumber);

    settings::ManostatSettings::setTargetPressure(stod(lineElements[2]));
    settings::ManostatSettings::setPressureSet(true);
}

/**
 * @brief parses the relaxation time of the manostat
 *
 * @details default value is 1.0
 *
 * @param lineElements
 *
 * @throw customException::InputFileException if relaxation time is negative
 */
void InputFileParserManostat::parseManostatRelaxationTime(const std::vector<std::string> &lineElements, const size_t lineNumber)
{
    checkCommand(lineElements, lineNumber);
    const auto relaxationTime = stod(lineElements[2]);

    if (relaxationTime < 0)
        throw customException::InputFileException("Relaxation time of manostat cannot be negative");

    settings::ManostatSettings::setTauManostat(relaxationTime);
}

/**
 * @brief Parse the compressibility used in the simulation (isothermal compressibility)
 *
 * @details default value is 4.5e-5
 *
 * @param lineElements
 *
 * @throw customException::InputFileException if compressibility is negative
 */
void InputFileParserManostat::parseCompressibility(const std::vector<std::string> &lineElements, const size_t lineNumber)
{
    checkCommand(lineElements, lineNumber);
    const auto compressibility = stod(lineElements[2]);

    if (compressibility < 0.0)
        throw customException::InputFileException("Compressibility cannot be negative");

    settings::ManostatSettings::setCompressibility(compressibility);
}