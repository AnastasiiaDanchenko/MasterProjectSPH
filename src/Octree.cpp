#include "..\headers\Octree.h"

Octree octree;

void OctreeInit() {
	octree.root = new OctreeNode(Eigen::Vector2f(0, 0), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	octree.maxDepth = 0;
	octree.maxParticlesPerNode = 0;
}

void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxDepth, int maxParticlesPerNode) {
	octree.root = new OctreeNode(minBounds, maxBounds);
	octree.maxDepth = maxDepth;
	octree.maxParticlesPerNode = maxParticlesPerNode;
}

// Recursive function to copy the octree
OctreeNode* CopyOctreeRecursive(const OctreeNode* node) {
    if (node == nullptr) {
        return nullptr;
    }

    OctreeNode* newNode = new OctreeNode(node->minBounds, node->maxBounds);
    newNode->particles = node->particles;
    newNode->isLeaf = node->isLeaf;

    for (int i = 0; i < 4; ++i) {
        newNode->children[i] = CopyOctreeRecursive(node->children[i]);
    }

    return newNode;
}

void OctreeInit(const Octree& other) {
    octree.root = CopyOctreeRecursive(other.root);
    octree.maxDepth = other.maxDepth;
    octree.maxParticlesPerNode = other.maxParticlesPerNode;
}

void InsertParticle(Particle* particle) {
	InsertParticleRecursive(octree.root, particle, 0);
}

int getChildIndex(OctreeNode* node, Particle* particle) {
    float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
    float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;

    if (particle->position.x() < xMid) {
        if (particle->position.y() < yMid) {
            return 0;
        }
        else {
            return 2;
        }
    }
    else {
        if (particle->position.y() < yMid) {
            return 1;
        }
        else {
            return 3;
        }
    }
}

void InsertParticleRecursive(OctreeNode* node, Particle* particle, int depth) {
	if (particle->position.x() >= node->minBounds.x() && particle->position.x() < node->maxBounds.x() &&
        particle->position.y() >= node->minBounds.y() && particle->position.y() < node->maxBounds.y()) {

        if (node->isLeaf == false) {
            int index = getChildIndex(node, particle);
            InsertParticleRecursive(node->children[index], particle, depth + 1);
        }
        else if (node->particles.size() >= octree.maxParticlesPerNode) {
            float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
            float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;

            node->children[0] = new OctreeNode(node->minBounds, Eigen::Vector2f(xMid, yMid));
            node->children[1] = new OctreeNode(Eigen::Vector2f(xMid, node->minBounds.y()), Eigen::Vector2f(node->maxBounds.x(), yMid));
            node->children[2] = new OctreeNode(Eigen::Vector2f(node->minBounds.x(), yMid), Eigen::Vector2f(xMid, node->maxBounds.y()));
            node->children[3] = new OctreeNode(Eigen::Vector2f(xMid, yMid), node->maxBounds);

            // Move particles from the parent node to the children
            for (auto& p : node->particles) {
                int index = getChildIndex(node, p);
                InsertParticleRecursive(node->children[index], p, depth + 1);
            }
            node->particles.clear();
            node->isLeaf = false;

            // insert current particle
            int index = getChildIndex(node, particle);
            InsertParticleRecursive(node->children[index], particle, depth + 1);
        }
        else {
            node->particles.push_back(particle);
        }
    }
    else {
		std::cout << "Particle " << particle->ID << " is out of bounds" << std::endl;
        std::cout << "Particle position: " << particle->position.x() << ", " << particle->position.y() << std::endl;
        std::cout << "Node bounds: " << node->minBounds.x() << ", " << node->minBounds.y() << " - " << node->maxBounds.x() << ", " << node->maxBounds.y() << std::endl;
        std::cout << "Depth: " << depth << std::endl;
        std::cout << "Root bounds: " << octree.root->minBounds.x() << ", " << octree.root->minBounds.y() << " - " << octree.root->maxBounds.x() << ", " << octree.root->maxBounds.y() << std::endl;
        std::cout << std::endl;
	}
}

void QueryParticles(const Eigen::Vector2f& queryPoint, std::vector<Particle*>& result) {
	QueryParticlesRecursive(octree.root, queryPoint, result);
}

void QueryParticlesRecursive(OctreeNode* node, const Eigen::Vector2f& queryPoint, std::vector<Particle*>& result) {
	float closestX = std::max(node->minBounds.x(), std::min(queryPoint.x(), node->maxBounds.x()));
	float closestY = std::max(node->minBounds.y(), std::min(queryPoint.y(), node->maxBounds.y()));

	float distance2 = std::pow(queryPoint.x() - closestX, 2) + std::pow(queryPoint.y() - closestY, 2);

	if (distance2 > std::pow(SUPPORT, 2)) {
		return;
	}
	for (auto& p : node->particles) {
		float distance2 = std::pow(queryPoint.x() - p->position.x(), 2) + std::pow(queryPoint.y() - p->position.y(), 2);
		if (distance2 < std::pow(SUPPORT, 2)) {
			result.push_back(p);
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (node->children[i] != nullptr) {
			QueryParticlesRecursive(node->children[i], queryPoint, result);
		}
	}
}

void outputDepth() {
    std::cout << "Octree depth: " << octree.maxDepth << std::endl;
}

void outputBounds() {
    std::cout << "Octree bounds: " << octree.root->minBounds.x() << ", " << octree.root->minBounds.y() << " - " << octree.root->maxBounds.x() << ", " << octree.root->maxBounds.y() << std::endl;
}

void outputParticlesPerNodeRecursive(OctreeNode* node) {
    std::cout << "Node bounds: " << node->minBounds.x() << ", " << node->minBounds.y() << " - " << node->maxBounds.x() << ", " << node->maxBounds.y() << std::endl;
    std::cout << "Is leaf: " << node->isLeaf << std::endl;
    std::cout << "Number of particles: " << node->particles.size() << std::endl << std::endl;
    for (int i = 0; i < 4; ++i) {
        if (node->children[i] != nullptr) {
            outputParticlesPerNodeRecursive(node->children[i]);
        }
    }
}

void outputParticlesPerNode() {
    // output recursively the number of particles in each node of the octree
    outputParticlesPerNodeRecursive(octree.root);
}
