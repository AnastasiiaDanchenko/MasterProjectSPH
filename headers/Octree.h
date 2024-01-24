#pragma once
#include "Grid.h"
#include <cmath>

// Define an Octree node structure
struct OctreeNode {
    Eigen::Vector2f minBounds;
    Eigen::Vector2f maxBounds;
    std::vector<Eigen::Vector2i> gridCells;
    OctreeNode* children[4];
    bool isLeaf;
    int nbParticles;
    float nodeSize;

    OctreeNode(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds)
        : minBounds(minBounds), maxBounds(maxBounds), isLeaf(true), nbParticles(0) {
        for (int i = 0; i < 4; ++i) {
            children[i] = nullptr;
        }
        nodeSize = maxBounds.x() - minBounds.x();
    }
};

struct Octree {
    int maxParticlesPerNode;
    OctreeNode* root;
};

extern Octree octree;

void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxParticlesPerNode);
void InsertCell(const Eigen::Vector2i& cell);
void SplitNode();
void OctreeSearch();
void outputBounds();


//#pragma once
//#include "Particle.h"
//#include <cmath>
//
//// Define an Octree node structure
//struct OctreeNode {
//    Eigen::Vector2f minBounds;
//    Eigen::Vector2f maxBounds;
//    std::vector<Particle*> particles;
//    OctreeNode* children[4];
//    bool isLeaf = true;
//
//    OctreeNode(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds)
//        : minBounds(minBounds), maxBounds(maxBounds), isLeaf(true) {
//        for (int i = 0; i < 4; ++i) {
//            children[i] = nullptr;
//        }
//    }
//};
//
//struct Octree {
//    int maxDepth;
//    int maxParticlesPerNode;
//    OctreeNode* root;
//};
//
//extern Octree octree;
//
//void OctreeInit();
//void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxDepth, int maxParticlesPerNode);
//void OctreeInit(const Octree& other);
//
//void InsertParticle(Particle* particle);
//void UpdateOctree();
//
//void outputDepth();
//void outputBounds();
//void outputParticlesPerNode();
