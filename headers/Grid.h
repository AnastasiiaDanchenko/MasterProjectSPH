#pragma once
#include "Particle.h"
#include <list>

// Particle container
extern std::vector<Particle> particles;
extern std::vector<std::vector<std::list<Particle*>>> grid;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;

void InitFluid();
void InitBoundaries();

void UniformGrid();
void GridUpdate();
