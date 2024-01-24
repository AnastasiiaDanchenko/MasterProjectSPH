#pragma once
#include <vector>

#include "Parameters.h"

class Particle {
public:
    Eigen::Vector2f position;
    Eigen::Vector2f velocity;
    Eigen::Vector2f acceleration;

    float mass;
    float density;
    float pressure;

    bool isFluid;

    int ID;
    Eigen::Vector2i currentCell;

    std::vector<Particle*> neighbors;
    std::vector<Particle*> candidates;

    Particle();
    Eigen::Vector2i getCellNumber();
};
