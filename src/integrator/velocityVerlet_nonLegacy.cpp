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

#include "constants.hpp"
#include "simulationBox.hpp"
#include "timingsSettings.hpp"
#include "velocityVerlet.hpp"

using namespace integrator;
using namespace simulationBox;
using namespace settings;
using namespace constants;

VelocityVerlet::VelocityVerlet() : Integrator("VelocityVerlet") {};

/**
 * @brief applies first half step of velocity verlet algorithm
 *
 * @param simBox
 */
void VelocityVerlet::firstStep(SimulationBox& simBox)
{
    startTimingsSection("Velocity Verlet - First Step");

    const auto dt = TimingsSettings::getTimeStep();

    simBox.flattenVelocities();
    simBox.flattenPositions();
    simBox.flattenForces();

    auto* const       velPtr    = simBox.getVelPtr();
    auto* const       posPtr    = simBox.getPosPtr();
    auto* const       forcesPtr = simBox.getForcesPtr();
    const auto* const massesPtr = simBox.getMassesPtr();

    // clang-format off
    #pragma omp target teams distribute parallel for                \
                collapse(2)                                         \
                is_device_ptr(velPtr, posPtr, massesPtr, forcesPtr)
    for(size_t i = 0; i < simBox.getNumberOfAtoms(); ++i)
    {
        const auto mass = massesPtr[i];

        for(size_t j = 0; j < 3; ++j)
        {
            velPtr[3 * i + j] += dt * forcesPtr[3 * i + j] / mass * _V_VERLET_VELOCITY_FACTOR_;
            posPtr[3 * i + j] += dt * velPtr[3 * i + j] * _FS_TO_S_;
            forcesPtr[3 * i + j] = 0.0;
        }
    }
    // clang-format on

    simBox.deFlattenVelocities();
    simBox.deFlattenPositions();
    simBox.deFlattenForces();

    const auto box = simBox.getBoxPtr();

    auto calculateCOM = [&box](auto& molecule)
    { molecule.calculateCenterOfMass(*box); };

    std::ranges::for_each(simBox.getMolecules(), calculateCOM);

    stopTimingsSection("Velocity Verlet - First Step");
}

/**
 * @brief applies second half step of velocity verlet algorithm
 *
 * @param simBox
 */
void VelocityVerlet::secondStep(SimulationBox& simBox)
{
    startTimingsSection("Velocity Verlet - Second Step");

    const auto dt = TimingsSettings::getTimeStep();

    simBox.flattenVelocities();
    simBox.flattenForces();

    auto* const       velPtr    = simBox.getVelPtr();
    const auto* const massesPtr = simBox.getMassesPtr();
    const auto* const forcesPtr = simBox.getForcesPtr();

    // clang-format off
    #pragma omp target teams distribute parallel for                \
                collapse(2)                                         \
                is_device_ptr(velPtr, massesPtr, forcesPtr)
    for(size_t i = 0; i < simBox.getNumberOfAtoms(); ++i)
    {
        const auto mass = massesPtr[i];

        for(size_t j = 0; j < 3; ++j)
            velPtr[3 * i + j] += dt * forcesPtr[3 * i + j] / mass * _V_VERLET_VELOCITY_FACTOR_;
    }
    // clang-format on

    simBox.deFlattenVelocities();

    stopTimingsSection("Velocity Verlet - Second Step");
}