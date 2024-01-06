#pragma once
#include "Grid.h"

// Octree node structure
struct OctreeNode {
    float minX, minY, maxX, maxY;  // Bounding box
    std::vector<Particle*> particles;  // Particles in this node
    OctreeNode* children[4];  // 4 children for 2D space
};

// Octree class
class Octree {
public:
    Octree(float minX, float minY, float maxX, float maxY, int maxParticlesPerNode)
        : root(nullptr), maxParticlesPerNode(maxParticlesPerNode) {
        root = new OctreeNode{ minX, minY, maxX, maxY, {}, {nullptr, nullptr, nullptr, nullptr} };
    }

    ~Octree() {
        clear(root);
    }

    void insert(Particle* particle) {
        insert(root, particle);
    }

    void queryNeighbors(Particle* particle, float radius, std::vector<Particle*>& neighbors) {
        queryNeighbors(root, particle, radius, neighbors);
    }

private:
    OctreeNode* root;
    int maxParticlesPerNode;

    void clear(OctreeNode* node) {
        if (!node) return;
        for (int i = 0; i < 4; ++i) {
            clear(node->children[i]);
        }
        delete node;
    }

    void insert(OctreeNode* node, Particle* particle) {
        if (!node) return;

        // Check if the particle is within the node's bounding box
        if (particle->position.x() >= node->minX && particle->position.x() <= node->maxX &&
            particle->position.y() >= node->minY && particle->position.y() <= node->maxY) {
            // Insert the particle into this node
            node->particles.push_back(particle);

            // Check if the node needs to be subdivided
            if (node->particles.size() > maxParticlesPerNode) {
                subdivide(node);
            }

            // Recursively insert into child nodes
            for (int i = 0; i < 4; ++i) {
                insert(node->children[i], particle);
            }
        }
    }

    void subdivide(OctreeNode* node) {
        float midX = (node->minX + node->maxX) / 2.0f;
        float midY = (node->minY + node->maxY) / 2.0f;

        node->children[0] = new OctreeNode{ node->minX, midY, midX, node->maxY, {}, {nullptr, nullptr, nullptr, nullptr} };
        node->children[1] = new OctreeNode{ midX, midY, node->maxX, node->maxY, {}, {nullptr, nullptr, nullptr, nullptr} };
        node->children[2] = new OctreeNode{ midX, node->minY, node->maxX, midY, {}, {nullptr, nullptr, nullptr, nullptr} };
        node->children[3] = new OctreeNode{ node->minX, node->minY, midX, midY, {}, {nullptr, nullptr, nullptr, nullptr} };

        // Move particles to child nodes
        for (auto& p : node->particles) {
            for (int i = 0; i < 4; ++i) {
                insert(node->children[i], p);
            }
        }
        node->particles.clear();
    }

    void queryNeighbors(OctreeNode* node, Particle* particle, float radius, std::vector<Particle*>& neighbors) {
        if (!node) return;

        // Check if the node's bounding box intersects with the search radius
        if (node->minX - radius > particle->position.x() || node->maxX + radius < particle->position.x() ||
            node->minY - radius > particle->position.y() || node->maxY + radius < particle->position.y()) {
            return;
        }

        // Check particles in this node
        for (auto& p : node->particles) {
            float distanceSquared = (p->position.x() - particle->position.x()) * (p->position.x() - particle->position.x()) +
                (p->position.y() - particle->position.y()) * (p->position.y() - particle->position.y());
            if (distanceSquared < radius * radius) {
                neighbors.push_back(p);
            }
        }

        // Recursively query child nodes
        for (int i = 0; i < 4; ++i) {
            queryNeighbors(node->children[i], particle, radius, neighbors);
        }
    }
};
