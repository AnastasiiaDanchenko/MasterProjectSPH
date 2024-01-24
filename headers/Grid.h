#pragma once
#include "Particle.h"
#include <list>
#include <numeric>

struct GridCell {
    Eigen::Vector2i cellNumber;
    Eigen::Vector2f minBounds;
    Eigen::Vector2f maxBounds;
    std::vector<Particle*> cellParticles;
};

// Particle container
extern std::vector<Particle> particles;
extern std::vector<GridCell> grid;
extern std::vector<std::list<Particle*>> linearGrid;
extern std::vector<size_t> particleIndices;

extern int GRID_WIDTH;
extern int GRID_HEIGHT;

void InitFluid(const int lenght);
void InitBoundaries();

void UniformGrid();
void GridUpdate();
void GridUpdateIncremental();

void LinearGrid();
int LinearGridCellNumber(const int x, const int y);
void Sorting();
