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
	std::vector<Particle*> innerParticles;
	innerParticles.reserve(node->nbParticles);

	if (!node->isLeaf) {
		for (int i = 0; i < 4; ++i) {
			OctreeSearchRecursive(node->children[i]);
		}
	} 
	else {
		innerParticles.clear();
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
