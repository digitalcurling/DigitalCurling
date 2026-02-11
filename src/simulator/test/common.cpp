#include <iostream>
#include "common.hpp"

namespace digitalcurling::test {

bool EqualsSimulatorStones(dcs::ISimulator::AllStones const& stones0, dcs::ISimulator::AllStones const& stones1)
{
    bool equal = true;
    for (int i = 0; i < stones0.size(); ++i) {
        if (stones0[i].has_value() != stones1[i].has_value()) {
            equal = false;
            break;
        }
        if (stones0[i].has_value()) {
            if (stones0[i]->position.x != stones1[i]->position.x ||
                stones0[i]->position.y != stones1[i]->position.y ||
                stones0[i]->angle != stones1[i]->angle ||
                stones0[i]->translational_velocity.x != stones1[i]->translational_velocity.x ||
                stones0[i]->translational_velocity.y != stones1[i]->translational_velocity.y ||
                stones0[i]->angular_velocity != stones1[i]->angular_velocity) {
                equal = false;
                break;
            }
        }
    }

    return equal;
}

void PrintSimulatorStones(dcs::ISimulator::AllStones const& stones)
{
    for (int i = 0; i < stones.size(); ++i) {
        std::cout << i << ": ";
        if (stones[i].has_value()) {
            std::cout << "pos.x: " << stones[i]->position.x << ", "
                << "pos.y: " << stones[i]->position.y << ", "
                << "angle: " << stones[i]->angle << ", "
                << "lv.x: " << stones[i]->translational_velocity.x << ", "
                << "lv.y: " << stones[i]->translational_velocity.y << ", "
                << "av: " << stones[i]->angular_velocity;
        } else {
            std::cout << "none";
        }

        std::cout << std::endl;
    }
}

} // unnamed namespace
