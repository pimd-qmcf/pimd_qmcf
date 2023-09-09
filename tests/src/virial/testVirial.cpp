#include "testVirial.hpp"

#include "gtest/gtest.h"   // for Message, TestPartResult
#include <memory>          // for allocator

TEST_F(TestVirial, calculateVirial)
{
    const auto force_mol1_atom1 = _simulationBox->getMolecule(0).getAtomForce(0);
    const auto force_mol1_atom2 = _simulationBox->getMolecule(0).getAtomForce(1);
    const auto force_mol2_atom1 = _simulationBox->getMolecule(1).getAtomForce(0);

    const auto position_mol1_atom1 = _simulationBox->getMolecule(0).getAtomPosition(0);
    const auto position_mol1_atom2 = _simulationBox->getMolecule(0).getAtomPosition(1);
    const auto position_mol2_atom1 = _simulationBox->getMolecule(1).getAtomPosition(0);

    const auto shiftForce_mol1_atom1 = _simulationBox->getMolecule(0).getAtomShiftForce(0);
    const auto shiftForce_mol1_atom2 = _simulationBox->getMolecule(0).getAtomShiftForce(1);
    const auto shiftForce_mol2_atom1 = _simulationBox->getMolecule(1).getAtomShiftForce(0);

    const auto virial = force_mol1_atom1 * position_mol1_atom1 + force_mol1_atom2 * position_mol1_atom2 +
                        force_mol2_atom1 * position_mol2_atom1 + shiftForce_mol1_atom1 + shiftForce_mol1_atom2 +
                        shiftForce_mol2_atom1;

    _virial->calculateVirial(*_simulationBox, *_data);

    EXPECT_EQ(_data->getVirial(), virial);
    EXPECT_EQ(_simulationBox->getMolecule(0).getAtomShiftForce(0), linearAlgebra::Vec3D(0.0, 0.0, 0.0));
    EXPECT_EQ(_simulationBox->getMolecule(0).getAtomShiftForce(1), linearAlgebra::Vec3D(0.0, 0.0, 0.0));
    EXPECT_EQ(_simulationBox->getMolecule(1).getAtomShiftForce(0), linearAlgebra::Vec3D(0.0, 0.0, 0.0));
}

TEST_F(TestVirial, intramolecularCorrection)
{
    auto *virialClass = new virial::VirialMolecular();
    virialClass->setVirial(linearAlgebra::Vec3D(0.0, 0.0, 0.0));
    const auto force_mol1_atom1 = _simulationBox->getMolecule(0).getAtomForce(0);
    const auto force_mol1_atom2 = _simulationBox->getMolecule(0).getAtomForce(1);
    const auto force_mol2_atom1 = _simulationBox->getMolecule(1).getAtomForce(0);

    const auto position_mol1_atom1 = _simulationBox->getMolecule(0).getAtomPosition(0);
    const auto position_mol1_atom2 = _simulationBox->getMolecule(0).getAtomPosition(1);
    const auto position_mol2_atom1 = _simulationBox->getMolecule(1).getAtomPosition(0);

    const auto shiftForce_mol1_atom1 = _simulationBox->getMolecule(0).getAtomShiftForce(0);
    const auto shiftForce_mol1_atom2 = _simulationBox->getMolecule(0).getAtomShiftForce(1);
    const auto shiftForce_mol2_atom1 = _simulationBox->getMolecule(1).getAtomShiftForce(0);

    auto virial = force_mol1_atom1 * position_mol1_atom1 + force_mol1_atom2 * position_mol1_atom2 +
                  force_mol2_atom1 * position_mol2_atom1 + shiftForce_mol1_atom1 + shiftForce_mol1_atom2 + shiftForce_mol2_atom1;

    virialClass->calculateVirial(*_simulationBox, *_data);

    EXPECT_EQ(_data->getVirial(), virial);
}

TEST_F(TestVirial, calculateVirialMolecular)
{
    auto *virialClass = new virial::VirialMolecular();
    virialClass->setVirial(linearAlgebra::Vec3D(0.0, 0.0, 0.0));
    const auto force_mol1_atom1 = _simulationBox->getMolecule(0).getAtomForce(0);
    const auto force_mol1_atom2 = _simulationBox->getMolecule(0).getAtomForce(1);
    const auto force_mol2_atom1 = _simulationBox->getMolecule(1).getAtomForce(0);

    const auto position_mol1_atom1 = _simulationBox->getMolecule(0).getAtomPosition(0);
    const auto position_mol1_atom2 = _simulationBox->getMolecule(0).getAtomPosition(1);
    const auto position_mol2_atom1 = _simulationBox->getMolecule(1).getAtomPosition(0);

    const auto centerOfMass_mol1 = _simulationBox->getMolecule(0).getCenterOfMass();
    const auto centerOfMass_mol2 = _simulationBox->getMolecule(1).getCenterOfMass();

    const auto virial = -force_mol1_atom1 * (position_mol1_atom1 - centerOfMass_mol1) -
                        force_mol1_atom2 * (position_mol1_atom2 - centerOfMass_mol1) -
                        force_mol2_atom1 * (position_mol2_atom1 - centerOfMass_mol2);

    physicalData::PhysicalData physicalData;

    virialClass->intraMolecularVirialCorrection(*_simulationBox, physicalData);

    EXPECT_EQ(virialClass->getVirial(), virial);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}