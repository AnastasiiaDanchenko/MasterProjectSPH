#pragma once
#include "Grid.h"
#include <unordered_map>

extern std::unordered_map<int, std::vector<Particle*>> hashTable;
extern int HASH_TABLE_SIZE;
extern float CELL_SIZE;

int HashTableSize();
int HashFunction(const Eigen::Vector2f position);
void HashTable();
void HashTableUpdate();
