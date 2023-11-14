#include "../headers/HashTable.h"

HashTable::HashTable() {
	cellSize = 0.0f;
	hashTableSize = 0;
}

HashTable::HashTable(const int cell, const int tableSize) {
	cellSize = cell;
	hashTableSize = tableSize;
}

int HashTable::HashFunction(const Eigen::Vector2f position) {
	const int p1 = 73856093;
	const int p2 = 19349663;
	const int p3 = 83492791; 

	int c = ((int)(position.x() / cellSize) * p1) ^ ((int)(position.y() / cellSize) * p2);
	c = abs(c) % hashTableSize;
	return c;
}

int HashTable::HashTableSize() {
	int number = 2 * particles.size();
	// Find next prime number
	bool isPrime = false;
	while (!isPrime) {
		for (int i = 2; i < sqrt(number); i++) {
			if (number % i == 0) {
				isPrime = false;
				break;
			}
			isPrime = true;
		}
		number++;
	}
	return number;
}

void HashTable::ClearHashTable() {
	hashTable.clear();
}

void HashTable::UpdateHashTable() {
	hashTable.clear();
	const int p1 = 73856093;
	const int p2 = 19349663;
	const int p3 = 83492791;
	for (int i = 0; i < particles.size(); i++) {
		int c = (int)(particles[i].position.x() / cellSize * p1) ^ (int)(particles[i].position.y() / cellSize * p2);
		//int c = (int)((int)(particles[i].position.x() / cellSize) * p1) ^ (int)((int)(particles[i].position.y() / cellSize) * p2);
		c = abs(c) % this->hashTableSize;
		//int c = HashFunction(particles[i].position);
		hashTable.insert(std::pair<int, Particle*>(c, &particles[i]));
	}

	for (int i = 0; i < hashTable.size(); i++) {
		if (hashTable.count(i) > 1) {
			std::cout << "Cell " << i << " has " << hashTable.count(i) << " particles" << std::endl;
		}
	}
	exit(0);
}

bool HashTable::CheckNeighbors(const Particle& particle, std::vector<Particle*>& neighbors) {
	bool onTheList = false;
	for (auto& p : neighbors) {
		if (p == &particle) {
			onTheList = true;
			break;
		}
	}
	return onTheList;
}

void HashTable::HashTableNeighborCells(const Eigen::Vector2f position, std::vector<Particle*>& neighbors) {
	const int c = HashFunction(position);
	std::pair<hashmap::iterator, hashmap::iterator> range = hashTable.equal_range(c);
	for (hashmap::iterator it = range.first; it != range.second; ++it) {
		Particle *p = it->second;

		// Check if particle is already on the neighbor list
		if (!CheckNeighbors(*p, neighbors)) {
			const Eigen::Vector2f r = position - p->position;
			if (r.squaredNorm() < pow(SUPPORT, 2)) {
				neighbors.push_back(p);
			}
		}
	}
}

std::vector<Particle*> HashTable::Neighbors(const Eigen::Vector2f position) {
	std::vector<Particle*> neighbors;
	//HashTableNeighborCells(position, neighbors);

	const int c = HashFunction(position);
	std::pair<hashmap::iterator, hashmap::iterator> range = hashTable.equal_range(c);
	for (hashmap::iterator it = range.first; it != range.second; ++it) {
		Particle* p = it->second;

		// Check if particle is already on the neighbor list
		if (!CheckNeighbors(*p, neighbors)) {
			const Eigen::Vector2f r = position - p->position;
			if (r.squaredNorm() < pow(SUPPORT, 2)) {
				neighbors.push_back(p);
			}
		}
	}

	const int min_x = (int)((position.x() - cellSize) / cellSize);
	const int max_x = (int)((position.x() + cellSize) / cellSize);
	const int min_y = (int)((position.y() - cellSize) / cellSize);
	const int max_y = (int)((position.y() + cellSize) / cellSize);

	const float step = 0.85;
	for (float i = min_x; i <= max_x; i += step) {
		for (float j = min_y; j <= max_y; j += step) {
			//HashTableNeighborCells(Eigen::Vector2f(i, j), neighbors);
			const int c = HashFunction(position);
			std::pair<hashmap::iterator, hashmap::iterator> range = hashTable.equal_range(c);
			for (hashmap::iterator it = range.first; it != range.second; ++it) {
				Particle* p = it->second;

				// Check if particle is already on the neighbor list
				if (!CheckNeighbors(*p, neighbors)) {
					const Eigen::Vector2f r = position - p->position;
					if (r.squaredNorm() < pow(SUPPORT, 2)) {
						neighbors.push_back(p);
					}
				}
			}
		}
	}

	return neighbors;
}
