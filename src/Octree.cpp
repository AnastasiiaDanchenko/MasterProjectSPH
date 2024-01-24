#include "..\headers\Octree.h"

Octree octree;

std::vector<int> getChildIndex(OctreeNode* node, const Eigen::Vector2i& cell) {
	float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
	float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;
	auto g = grid[cell.x() + cell.y() * GRID_WIDTH];

	std::vector<int> indices;

	if (g.minBounds.x() + CELL_SIZE >= node->minBounds.x() && g.maxBounds.x() - CELL_SIZE <= xMid) {
		if (g.minBounds.y() + CELL_SIZE >= node->minBounds.y() && g.maxBounds.y() - CELL_SIZE <= yMid) {
			indices.push_back(0);
		}
		if (g.minBounds.y() + CELL_SIZE >= yMid && g.maxBounds.y() - CELL_SIZE <= node->maxBounds.y()) {
			indices.push_back(2);
		}
	}
	if (g.minBounds.x() + CELL_SIZE >= xMid && g.maxBounds.x() - CELL_SIZE <= node->maxBounds.x()) {
		if (g.minBounds.y() + CELL_SIZE >= node->minBounds.y() && g.maxBounds.y() - CELL_SIZE <= yMid) {
			indices.push_back(1);
		}
		if (g.minBounds.y() + CELL_SIZE >= yMid && g.maxBounds.y() - CELL_SIZE <= node->maxBounds.y()) {
			indices.push_back(3);
		}
	}

	return indices;
}

void SplitNodeRecursive(OctreeNode* node) {
	if (node->nbParticles > octree.maxParticlesPerNode && node->nodeSize >= 2 * CELL_SIZE) {
		float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
		float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;

		node->children[0] = new OctreeNode(node->minBounds, Eigen::Vector2f(xMid, yMid));
		node->children[1] = new OctreeNode(Eigen::Vector2f(xMid, node->minBounds.y()), Eigen::Vector2f(node->maxBounds.x(), yMid));
		node->children[2] = new OctreeNode(Eigen::Vector2f(node->minBounds.x(), yMid), Eigen::Vector2f(xMid, node->maxBounds.y()));
		node->children[3] = new OctreeNode(Eigen::Vector2f(xMid, yMid), node->maxBounds);

		// Move particles from the parent node to the children
		for (auto& c : node->gridCells) {
			std::vector<int> indices = getChildIndex(node, c);
			for (auto& index : indices) {
				node->children[index]->gridCells.push_back(c);
				node->children[index]->nbParticles += grid[c.x() + c.y() * GRID_WIDTH].cellParticles.size();
			}
		}
		node->gridCells.clear();
		node->isLeaf = false;

		// split children recursively
		for (int i = 0; i < 4; ++i) {
			SplitNodeRecursive(node->children[i]);
		}
	}
}

void SplitNode() {
	SplitNodeRecursive(octree.root);
}

void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxParticlesPerNode) {
	octree.root = new OctreeNode(minBounds, maxBounds);
	octree.maxParticlesPerNode = maxParticlesPerNode;

	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			// insert only non-empty grid cells into the octree
			if (grid[i + j * GRID_WIDTH].cellParticles.size() > 0) {
				octree.root->gridCells.push_back(Eigen::Vector2i(i, j));
				octree.root->nbParticles += grid[i + j * GRID_WIDTH].cellParticles.size();
			}
		}
	}
}

void outputBounds() {
    std::cout << "Octree bounds: " << octree.root->minBounds.x() << ", " << octree.root->minBounds.y() << " - " << octree.root->maxBounds.x() << ", " << octree.root->maxBounds.y() << std::endl;
}

void OctreeSearchRecursive(OctreeNode* node){
	if (node->isLeaf == false) {
		for (int i = 0; i < 4; ++i) {
			OctreeSearchRecursive(node->children[i]);
		}
	} else {
		std::vector<Particle*> innerParticles;
		for (int i = 0; i < node->gridCells.size(); ++i) {
			// gather all particles from the same leaf node
			for (auto& p : grid[node->gridCells[i].x() + node->gridCells[i].y() * GRID_WIDTH].cellParticles) {
				p->neighbors.clear();
				innerParticles.push_back(p);
			}
		}

		// use brute force search for pairs of particles in the same leaf node
		for (int i = 0; i < innerParticles.size(); ++i) {
			for (int j = i + 1; j < innerParticles.size(); ++j) {
				float dx = innerParticles[j]->position.x() - innerParticles[i]->position.x();
				float dy = innerParticles[j]->position.y() - innerParticles[i]->position.y();
				float distance = std::sqrt(dx * dx + dy * dy);
				if (distance < SUPPORT) {
					innerParticles[i]->neighbors.push_back(innerParticles[j]);
					innerParticles[j]->neighbors.push_back(innerParticles[i]);
				}
			}
			// add particle to itself as a neighbor
			innerParticles[i]->neighbors.push_back(innerParticles[i]);
		}
	}
}

void OctreeSearch() {
	OctreeSearchRecursive(octree.root);
}

//#include "..\headers\Octree.h"
//
//Octree octree;
//
//void OctreeInit() {
//	octree.root = new OctreeNode(Eigen::Vector2f(0, 0), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
//	octree.maxDepth = 0;
//	octree.maxParticlesPerNode = 0;
//}
//
//void OctreeInit(const Eigen::Vector2f& minBounds, const Eigen::Vector2f& maxBounds, int maxDepth, int maxParticlesPerNode) {
//	octree.root = new OctreeNode(minBounds, maxBounds);
//	octree.maxDepth = maxDepth;
//	octree.maxParticlesPerNode = maxParticlesPerNode;
//}
//
//// Recursive function to copy the octree
//OctreeNode* CopyOctreeRecursive(const OctreeNode* node) {
//    if (node == nullptr) {
//        return nullptr;
//    }
//
//    OctreeNode* newNode = new OctreeNode(node->minBounds, node->maxBounds);
//    newNode->particles = node->particles;
//    newNode->isLeaf = node->isLeaf;
//
//    for (int i = 0; i < 4; ++i) {
//        newNode->children[i] = CopyOctreeRecursive(node->children[i]);
//    }
//
//    return newNode;
//}
//
//void OctreeInit(const Octree& other) {
//    octree.root = CopyOctreeRecursive(other.root);
//    octree.maxDepth = other.maxDepth;
//    octree.maxParticlesPerNode = other.maxParticlesPerNode;
//}
//
//void InsertParticle(Particle* particle) {
//	//InsertParticleRecursive(octree.root, particle, 0);
//}
//
//int getChildIndex(OctreeNode* node, Particle* particle) {
//    float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
//    float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;
//
//    if (particle->position.x() < xMid) {
//        if (particle->position.y() < yMid) { return 0; }
//        else { return 2; }
//    } else {
//        if (particle->position.y() < yMid) { return 1; }
//        else { return 3; }
//    }
//}
//
//void InsertParticleRecursive(OctreeNode* node, Particle* particle, int depth) {
//	if (particle->position.x() >= node->minBounds.x() && particle->position.x() < node->maxBounds.x() &&
//        particle->position.y() >= node->minBounds.y() && particle->position.y() < node->maxBounds.y()) {
//
//        if (node->isLeaf == false) {
//            int index = getChildIndex(node, particle);
//            InsertParticleRecursive(node->children[index], particle, depth + 1);
//        } else if (node->particles.size() >= octree.maxParticlesPerNode) {
//            float xMid = (node->minBounds.x() + node->maxBounds.x()) / 2;
//            float yMid = (node->minBounds.y() + node->maxBounds.y()) / 2;
//
//            node->children[0] = new OctreeNode(node->minBounds, Eigen::Vector2f(xMid, yMid));
//            node->children[1] = new OctreeNode(Eigen::Vector2f(xMid, node->minBounds.y()), Eigen::Vector2f(node->maxBounds.x(), yMid));
//            node->children[2] = new OctreeNode(Eigen::Vector2f(node->minBounds.x(), yMid), Eigen::Vector2f(xMid, node->maxBounds.y()));
//            node->children[3] = new OctreeNode(Eigen::Vector2f(xMid, yMid), node->maxBounds);
//
//            // Move particles from the parent node to the children
//            for (auto& p : node->particles) {
//                int index = getChildIndex(node, p);
//                InsertParticleRecursive(node->children[index], p, depth + 1);
//            }
//            node->particles.clear();
//            node->isLeaf = false;
//
//            // insert current particle
//            int index = getChildIndex(node, particle);
//            InsertParticleRecursive(node->children[index], particle, depth + 1);
//        } else {
//            node->particles.push_back(particle);
//        }
//    } else {
//		std::cout << "Particle " << particle->ID << " is out of bounds" << std::endl;
//        std::cout << "Particle position: " << particle->position.x() << ", " << particle->position.y() << std::endl;
//        std::cout << "Node bounds: " << node->minBounds.x() << ", " << node->minBounds.y() << " - " << node->maxBounds.x() << ", " << node->maxBounds.y() << std::endl;
//        std::cout << "Depth: " << depth << std::endl;
//        std::cout << "Root bounds: " << octree.root->minBounds.x() << ", " << octree.root->minBounds.y() << " - " << octree.root->maxBounds.x() << ", " << octree.root->maxBounds.y() << std::endl;
//        std::cout << std::endl;
//	}
//}
//
//void outputDepth() {
//    std::cout << "Octree depth: " << octree.maxDepth << std::endl;
//}
//
//void outputBounds() {
//    std::cout << "Octree bounds: " << octree.root->minBounds.x() << ", " << octree.root->minBounds.y() << " - " << octree.root->maxBounds.x() << ", " << octree.root->maxBounds.y() << std::endl;
//}
//
//void outputParticlesPerNodeRecursive(OctreeNode* node) {
//    std::cout << "Node bounds: " << node->minBounds.x() << ", " << node->minBounds.y() << " - " << node->maxBounds.x() << ", " << node->maxBounds.y() << std::endl;
//    std::cout << "Is leaf: " << node->isLeaf << std::endl;
//    std::cout << "Number of particles: " << node->particles.size() << std::endl << std::endl;
//    for (int i = 0; i < 4; ++i) {
//        if (node->children[i] != nullptr) {
//            outputParticlesPerNodeRecursive(node->children[i]);
//        }
//    }
//}
//
//void outputParticlesPerNode() {
//    // output recursively the number of particles in each node of the octree
//    outputParticlesPerNodeRecursive(octree.root);
//}
//
//void UpdateOctreeRecursive(OctreeNode* node) {
//    if (node->isLeaf == false) {
//        for (int i = 0; i < 4; ++i) {
//            UpdateOctreeRecursive(node->children[i]);
//        }
//    }
//    else {
//        for (int i = 0; i < node->particles.size(); i++) {
//            if (node->particles[i]->position.x() < node->minBounds.x() || node->particles[i]->position.x() >= node->maxBounds.x() ||
//                node->particles[i]->position.y() < node->minBounds.y() || node->particles[i]->position.y() >= node->maxBounds.y()) {
//                node->particles.erase(node->particles.begin() + i);
//                InsertParticle(node->particles[i]);
//                i--;
//            }
//        }
//        // merge neighboring leaf nodes if they are empty
//
//    }
//}
//
//void UpdateOctree() {
//    UpdateOctreeRecursive(octree.root);
//}
