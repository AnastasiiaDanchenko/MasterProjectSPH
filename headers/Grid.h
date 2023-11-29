#pragma once
#include "Particle.h"
#include <list>

// Particle container
extern std::vector<Particle> particles;
extern std::vector<std::vector<std::list<Particle*>>> grid;
extern std::vector<Particle*> linearGrid;

extern int GRID_WIDTH;
extern int GRID_HEIGHT;

void InitFluid(const int lenght);
void InitBoundaries();

void UniformGrid();
void GridUpdate();

void LinearGrid();
int LinearGridCellNumber(const int x, const int y);
void Sorting();
