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

#include <algorithm>    // for __for_each_fn
#include <cstddef>      // for size_t
#include <functional>   // for identity

#include "constants/conversionFactors.hpp"   // for _BOLTZMANN_CONSTANT_IN_KCAL_PER_MOL_, _FS_TO_S_
#include "constants/internalConversionFactors.hpp"   // for _MOMENTUM_TO_FORCE_
#include "noseHooverThermostat.hpp"
#include "physicalData.hpp"         // for PhysicalData
#include "simulationBox.hpp"        // for SimulationBox
#include "thermostatSettings.hpp"   // for ThermostatType
#include "timingsSettings.hpp"      // for TimingsSettings
#include "vector3d.hpp"             // for operator*

using thermostat::NoseHooverThermostat;
using namespace constants;
using namespace settings;
using namespace simulationBox;
using namespace physicalData;

/**
 * @brief applies the Nose-Hoover thermostat on the forces
 *
 * @details the Nose-Hoover thermostat is applied on the forces of the atoms
 * after force calculation
 *
 * @param simBox simulation box
 */
void NoseHooverThermostat::applyThermostatOnForces(SimulationBox &simBox)
{
    startTimingsSection("Nose-Hoover - Forces");

    const auto kB        = _BOLTZMANN_CONSTANT_IN_KCAL_PER_MOL_;
    const auto kT_target = kB * _targetTemperature;

    const double degreesOfFreedom    = simBox.getDegreesOfFreedom();
    const auto   couplingFreqSquared = _couplingFrequency * _couplingFrequency;

    auto factor  = _chi[0] * couplingFreqSquared;
    factor      /= (kT_target * degreesOfFreedom);
    factor      *= _MOMENTUM_TO_FORCE_;

    auto applyNoseHoover = [factor](auto &atom)
    { atom->addForce(-factor * atom->getVelocity() * atom->getMass()); };

    std::ranges::for_each(simBox.getAtoms(), applyNoseHoover);

    stopTimingsSection("Nose-Hoover - Forces");
}