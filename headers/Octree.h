#pragma once
#include "Particle.h"
#include <cmath>

// Define an Octree node structure
struct OctreeNode {
    Eigen::Vector2f minBounds;
    Eigen::Vector2f maxBounds;
    std::vector<Particle*> particles;
    OctreeNode* children[4];
    bool isLeaf = true;

    OctreeNode(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds)
        : minBounds(minBounds), maxBounds(maxBounds), isLeaf(true) {
        for (int i = 0; i < 4; ++i) {
            children[i] = nullptr;
        }
    }
};

struct Octree {
    int maxDepth;
    int maxParticlesPerNode;
    OctreeNode* root;
};

extern Octree octree;

void OctreeInit();
void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxDepth, int maxParticlesPerNode);
void OctreeInit(const Octree& other);

void InsertParticle(Particle* particle);
void InsertParticleRecursive(OctreeNode* node, Particle* particle, int depth);
void QueryParticles(const Eigen::Vector2f& queryPoint, std::vector<Particle*>& result);
void QueryParticlesRecursive(OctreeNode* node, const Eigen::Vector2f& queryPoint, std::vector<Particle*>& result);

void outputDepth();
void outputBounds();
void outputParticlesPerNode();
