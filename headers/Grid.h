#pragma once
#include "Octree.h"
#include <list>
#include <numeric>

// Particle container
extern std::vector<Particle> particles;
extern std::vector<std::vector<std::list<Particle*>>> grid;
extern std::vector<std::list<Particle*>> linearGrid;
extern std::vector<size_t> particleIndices;

extern int GRID_WIDTH;
extern int GRID_HEIGHT;

void InitFluid(const int lenght);
void InitBoundaries();

void UniformGrid();
void GridUpdate();

void LinearGrid();
int LinearGridCellNumber(const int x, const int y);
void Sorting();
void OctreeInit();
