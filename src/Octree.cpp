//#include "../headers/Octree.h"
//
//OctreeNode* root;
//const int MAX_PARTICLES_PER_NODE = 10;
//
//void Octree() {
//	root = new OctreeNode();
//	root->minX = 0.0f;
//	root->maxX = WINDOW_WIDTH;
//	root->minY = 0.0f;
//	root->maxY = WINDOW_HEIGHT;
//}
//
//void Octree(float minX, float maxX, float minY, float maxY) {
//	root = new OctreeNode();
//	root->minX = minX;
//	root->maxX = maxX;
//	root->minY = minY;
//	root->maxY = maxY;
//}
//
//void clear(OctreeNode* node) {
//	if (node == nullptr) return;
//	for (int i = 0; i < 4; i++) {
//		clear(node->children[i]);
//	}
//	delete node;
//}
//
//void subdivide(OctreeNode* node) {
//	float midX = (node->minX + node->maxX) / 2.0f;
//	float midY = (node->minY + node->maxY) / 2.0f;
//
//	node->children[0] = new OctreeNode;
//}
//
//void insert(OctreeNode* node, Particle* p) {
//	if (node == nullptr) return;
//	if (p->position.x() >= node->minX && p->position.x() <= node->maxX && p->position.y() >= node->minY && p->position.y() <= node->maxY) {
//		node->particles.push_back(p);
//
//		if (node->particles.size() >= MAX_PARTICLES_PER_NODE) {
//			subdivide(node);
//		}
//
//		// Insert particle into children
//		for (int i = 0; i < 4; i++) {
//			insert(node->children[i], p);
//		}
//	}
//}
