#include "molecule.hpp"       // for Molecule
#include "physicalData.hpp"   // for PhysicalData
#include "potential.hpp"
#include "simulationBox.hpp"   // for SimulationBox

#include <cstddef>   // for size_t

namespace simulationBox
{
    class CellList;
}   // namespace simulationBox

using namespace potential;

/**
 * @brief calculates forces, coulombic and non-coulombic energy for brute force routine
 *
 * @param simBox
 * @param physicalData
 */
inline void PotentialBruteForce::calculateForces(simulationBox::SimulationBox &simBox,
                                                 physicalData::PhysicalData   &physicalData,
                                                 simulationBox::CellList &)
{
    const auto box = simBox.getBoxDimensions();

    double totalCoulombEnergy    = 0.0;
    double totalNonCoulombEnergy = 0.0;

    // inter molecular forces
    const size_t numberOfMolecules = simBox.getNumberOfMolecules();

    for (size_t mol1 = 0; mol1 < numberOfMolecules; ++mol1)
    {
        auto        &molecule1                 = simBox.getMolecule(mol1);
        const size_t numberOfAtomsInMolecule_i = molecule1.getNumberOfAtoms();

        for (size_t mol2 = 0; mol2 < mol1; ++mol2)
        {
            auto        &molecule2                 = simBox.getMolecule(mol2);
            const size_t numberOfAtomsInMolecule_j = molecule2.getNumberOfAtoms();

            for (size_t atom1 = 0; atom1 < numberOfAtomsInMolecule_i; ++atom1)
            {
                for (size_t atom2 = 0; atom2 < numberOfAtomsInMolecule_j; ++atom2)
                {
                    const auto [coulombEnergy, nonCoulombEnergy] =
                        calculateSingleInteraction(box, molecule1, molecule2, atom1, atom2);

                    totalCoulombEnergy    += coulombEnergy;
                    totalNonCoulombEnergy += nonCoulombEnergy;
                }
            }
        }
    }

    physicalData.setCoulombEnergy(totalCoulombEnergy);
    physicalData.setNonCoulombEnergy(totalNonCoulombEnergy);
}