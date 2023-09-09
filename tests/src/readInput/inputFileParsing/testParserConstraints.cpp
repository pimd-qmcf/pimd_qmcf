#include "constraintSettings.hpp"           // for ConstraintSettings
#include "constraints.hpp"                  // for Constraints
#include "engine.hpp"                       // for Engine
#include "exceptions.hpp"                   // for InputFileException
#include "inputFileParser.hpp"              // for readInput
#include "inputFileParserConstraints.hpp"   // for InputFileParserConstraints
#include "testInputFileReader.hpp"          // for TestInputFileReader
#include "throwWithMessage.hpp"             // for EXPECT_THROW_MSG

#include "gtest/gtest.h"   // for Message, TestPartResult
#include <gtest/gtest.h>   // for TestInfo (ptr only), TEST_F
#include <string>          // for string, allocator, basic_string
#include <vector>          // for vector

using namespace readInput;

/**
 * @brief tests parsing the "shake" command
 *
 * @details possible options are on or off - otherwise throws inputFileException
 *
 */
TEST_F(TestInputFileReader, testParseShakeActivated)
{
    InputFileParserConstraints parser(_engine);
    std::vector<std::string>   lineElements = {"shake", "=", "off"};
    parser.parseShakeActivated(lineElements, 0);
    EXPECT_FALSE(_engine.getConstraints().isActive());

    lineElements = {"shake", "=", "on"};
    parser.parseShakeActivated(lineElements, 0);
    EXPECT_TRUE(_engine.getConstraints().isActive());

    lineElements = {"shake", "=", "1"};
    EXPECT_THROW_MSG(parser.parseShakeActivated(lineElements, 0),
                     customException::InputFileException,
                     R"(Invalid shake keyword "1" at line 0 in input file\n Possible keywords are "on" and "off")");
}

/**
 * @brief tests parsing the "shake-tolerance" command
 *
 * @details if the tolerance is negative, throws inputFileException
 *
 */
TEST_F(TestInputFileReader, testParseShakeTolerance)
{
    InputFileParserConstraints parser(_engine);
    std::vector<std::string>   lineElements = {"shake-tolerance", "=", "0.0001"};
    parser.parseShakeTolerance(lineElements, 0);
    EXPECT_EQ(settings::ConstraintSettings::getShakeTolerance(), 0.0001);

    lineElements = {"shake-tolerance", "=", "-0.0001"};
    EXPECT_THROW_MSG(
        parser.parseShakeTolerance(lineElements, 0), customException::InputFileException, "Shake tolerance must be positive");
}

/**
 * @brief tests parsing the "shake-iter" command
 *
 * @details if the number of iterations is negative, throws inputFileException
 *
 */
TEST_F(TestInputFileReader, testParseShakeIteration)
{
    InputFileParserConstraints parser(_engine);
    std::vector<std::string>   lineElements = {"shake-iter", "=", "100"};
    parser.parseShakeIteration(lineElements, 0);
    EXPECT_EQ(settings::ConstraintSettings::getShakeMaxIter(), 100);

    lineElements = {"shake-iter", "=", "-100"};
    EXPECT_THROW_MSG(parser.parseShakeIteration(lineElements, 0),
                     customException::InputFileException,
                     "Maximum shake iterations must be positive");
}

/**
 * @brief tests parsing the "rattle-tolerance" command
 *
 * @details if the tolerance is negative, throws inputFileException
 *
 */
TEST_F(TestInputFileReader, testParseRattleTolerance)
{
    InputFileParserConstraints parser(_engine);
    std::vector<std::string>   lineElements = {"rattle-tolerance", "=", "0.0001"};
    parser.parseRattleTolerance(lineElements, 0);
    EXPECT_EQ(settings::ConstraintSettings::getRattleTolerance(), 0.0001);

    lineElements = {"rattle-tolerance", "=", "-0.0001"};
    EXPECT_THROW_MSG(
        parser.parseRattleTolerance(lineElements, 0), customException::InputFileException, "Rattle tolerance must be positive");
}

/**
 * @brief tests parsing the "rattle-iter" command
 *
 * @details if the number of iterations is negative, throws inputFileException
 *
 */
TEST_F(TestInputFileReader, testParseRattleIteration)
{
    InputFileParserConstraints parser(_engine);
    std::vector<std::string>   lineElements = {"rattle-iter", "=", "100"};
    parser.parseRattleIteration(lineElements, 0);
    EXPECT_EQ(settings::ConstraintSettings::getRattleMaxIter(), 100);

    lineElements = {"rattle-iter", "=", "-100"};
    EXPECT_THROW_MSG(parser.parseRattleIteration(lineElements, 0),
                     customException::InputFileException,
                     "Maximum rattle iterations must be positive");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}