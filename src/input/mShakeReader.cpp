/*****************************************************************************
<GPL_HEADER>

    PQ
    Copyright (C) 2023-now  Jakob Gamper

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

<GPL_HEADER>
******************************************************************************/

#include "mShakeReader.hpp"

#include <format>   // for format

#include "engine.hpp"            // for Engine
#include "exceptions.hpp"        // for MShakeFileException
#include "fileSettings.hpp"      // for FileSettings
#include "mShakeReference.hpp"   // for MShakeReference
#include "stringUtilities.hpp"   // for removeComments

using namespace input::mShake;

/**
 * @brief Wrapper to construct MShakeReader and read mShake file
 *
 * @param engine
 */
void input::mShake::readMShake(engine::Engine &engine)
{
    MShakeReader mShakeReader(engine);
    mShakeReader.read();
}

/**
 * @brief constructor
 *
 * @details opens mShake file pointer
 *
 * @param engine
 */
MShakeReader::MShakeReader(engine::Engine &engine) : _engine(engine)
{
    _fileName = settings::FileSettings::getMShakeFileName();
    _fp.open(_fileName);
}

/**
 * @brief reads mShake file
 *
 * @details reads mShake file
 */
void MShakeReader::read()
{
    std::string line;

    _lineNumber = 0;

    while (getline(_fp, line))
    {
        line              = utilities::removeComments(line, "#");
        auto lineElements = utilities::splitString(line);

        ++_lineNumber;

        if (lineElements.empty())
            customException::MShakeFileException(std::format(
                "Empty line in mShake file at line {}! The M-Shake file should "
                "be in the form a an extended xyz file. Therefore, this line "
                "should be the header line of the extended xyz file and "
                "contain the number of atoms.",
                _lineNumber
            ));

        const auto nAtoms = std::stoi(lineElements[0]);

        auto mShakeReference = constraints::MShakeReference();

        getline(_fp, line);
        processCommentLine(line, mShakeReference);

        std::vector<std::string> atomLines;

        for (int i = 0; i < nAtoms; ++i)
        {
            getline(_fp, line);
            atomLines.push_back(line);
        }

        processAtomLines(atomLines, mShakeReference);

        _engine.getConstraints().addMShakeReference(mShakeReference);
    }
}

/**
 * @brief processes comment line
 *
 * @details processes comment line
 *
 * @param line
 * @param mShakeReference (empty)
 */
void MShakeReader::processCommentLine(
    std::string                  &line,
    constraints::MShakeReference &mShakeReference
)
{
    line         = utilities::removeComments(line, "#");
    auto configs = utilities::getLineCommands(line, _lineNumber);

    auto foundMolType = false;

    for (auto &config : configs)
    {
        utilities::addSpaces(config, "=", _lineNumber);
        auto configElements = utilities::splitString(config);

        if (utilities::toLowerCopy(configElements[0]) == "moltypes")
        {
            const auto molType = std::stoi(configElements[1]);
            try
            {
                auto simBox = _engine.getSimulationBox();

                mShakeReference.setMoleculeType(
                    simBox.findMoleculeType(size_t(molType))
                );

                foundMolType = true;
            }
            catch (const customException::RstFileException &e)
            {
                throw customException::MShakeFileException(e.what());
            }

            if (foundMolType)
                break;
        }
    }

    if (!foundMolType)
        customException::MShakeFileException(std::format(
            "Unknown command in mShake file at line {}! The M-Shake file "
            "should be in the form a an extended xyz file. Here, the "
            "comment line should contain the molecule type from the "
            "moldescriptor file in the following form: 'MolType = 1;'. "
            "Please note that the syntax parsing works exactly like in the "
            "input file. Thus, it is case insensitive and the commands are "
            "separated by semicolons. Furthermore, the spaces around the "
            "'=' sign can be of arbitrary length (including also no spaces "
            "at all).",
            _lineNumber
        ));
}

/**
 * @brief processes atom line
 *
 * @details processes atom line
 *
 * @param lines
 */
void MShakeReader::processAtomLines(
    std::vector<std::string>     &lines,
    constraints::MShakeReference &mShakeReference
)
{
    std::vector<std::string>         atomNames;
    std::vector<simulationBox::Atom> atoms;

    for (auto &line : lines)
    {
        line              = utilities::removeComments(line, "#");
        auto lineElements = utilities::splitString(line);

        if (lineElements.size() != 4)
            customException::MShakeFileException(std::format(
                "Wrong number of elements in atom line in mShake file at line "
                "{}! "
                "The M-Shake file should be in the form a an extended xyz "
                "file. "
                "Therefore, this line should contain the atom type and the "
                "coordinates of the atom.",
                _lineNumber
            ));

        const auto atomName = lineElements[0];
        const auto x        = std::stod(lineElements[1]);
        const auto y        = std::stod(lineElements[2]);
        const auto z        = std::stod(lineElements[3]);

        auto atom = simulationBox::Atom();

        atom.setName(atomName);
        atom.addPosition({x, y, z});

        atomNames.push_back(atomName);
        atoms.push_back(atom);
    }
    // // _engine.addAt
    auto  molType      = mShakeReference.getMoleculeType();
    auto &refAtomNames = molType.getAtomNames();

    if (atomNames != refAtomNames)
        customException::MShakeFileException(std::format(
            "Atom names in mShake file at line {} do not match the atom "
            "names of the molecule type! The M-Shake file should be in the "
            "form a an extended xyz file. Therefore, the atom names in the "
            "atom lines should match the atom names of the molecule type "
            "from the restart file.",
            _lineNumber
        ));

    mShakeReference.setAtoms(atoms);
}