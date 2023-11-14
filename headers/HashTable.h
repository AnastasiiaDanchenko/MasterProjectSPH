#pragma once
#include "Grid.h"
#include <map>

typedef std::multimap<int, Particle*> hashmap;

class HashTable {
public:
	HashTable();
	HashTable(const int cellSize, const int tableSize);
	int HashFunction(const Eigen::Vector2f position);
	int HashTableSize();
	void UpdateHashTable();
	void HashTableNeighborCells(const Eigen::Vector2f position, std::vector<Particle*>& neighbors);
	bool CheckNeighbors(const Particle& particle, std::vector<Particle*>& neighbors);
	std::vector<Particle*> Neighbors(const Eigen::Vector2f position);

private:
	hashmap hashTable;
	int cellSize;
	int hashTableSize;

	void ClearHashTable();
};
