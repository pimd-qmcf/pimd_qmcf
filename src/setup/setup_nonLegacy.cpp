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

#include <cassert>

#include "coulombPotential.hpp"
#include "engine.hpp"
#include "forceFieldNonCoulomb.hpp"
#include "forceFieldSettings.hpp"
#include "lennardJones.hpp"
#include "lennardJonesPair.hpp"
#include "nonCoulombPotential.hpp"
#include "potential.hpp"
#include "potentialSettings.hpp"
#include "setup.hpp"

using namespace engine;
using namespace settings;
using namespace potential;

/**
 * @brief setup the engine with the flattened data
 *
 * @param inputFileName
 * @param engine
 */
void setup::setupFlattenedData(Engine &engine)
{
    // TODO: clean the following lines up!

    auto &simBox = engine.getSimulationBox();
    simBox.flattenPositions();
    simBox.flattenVelocities();
    simBox.flattenForces();
    simBox.flattenShiftForces();
    simBox.flattenCharges();

    simBox.initAtomsPerMolecule();
    simBox.initMoleculeIndices();

    simBox.flattenAtomTypes();
    simBox.flattenMolTypes();
    simBox.flattenInternalGlobalVDWTypes();

#ifdef __PQ_GPU__
    if (engine.getDevice().isDeviceUsed())
        initDeviceMemory(engine);
#endif

    auto *const pot = engine.getPotentialPtr();

    using enum NonCoulombType;

    if (ForceFieldSettings::isActive())
    {
        if (PotentialSettings::getNonCoulombType() == LJ)
            setupFlattenedNonCoulPot<LennardJones>(pot);
        else if (PotentialSettings::getNonCoulombType() == BUCKINGHAM)
            setupFlattenedNonCoulPot<Buckingham>(pot);
        else if (PotentialSettings::getNonCoulombType() == MORSE)
            setupFlattenedNonCoulPot<Morse>(pot);
        else
            customException::UserInputException(
                "NonCoulombType not implemented yet"
            );
    }
    else
        customException::UserInputException(
            "NonCoulombType only implemented for ForceField at the moment"
        );

    pot->setCoulombParamVectors(pot->getCoulombPotential().copyParamsVector());

    const auto box            = simBox.getBoxPtr();
    const auto isOrthoRhombic = box->isOrthoRhombic();

    pot->setFunctionPointers(isOrthoRhombic);
}