#include "../headers/HashTable.h"

std::unordered_map<int, std::vector<Particle*>> hashTable;
int HASH_TABLE_SIZE;
float CELL_SIZE;

int HashTableSize() {
    int tableSize = 2 * particles.size();
    // Find next prime number
    bool isPrime = false;
    while (isPrime == false) {
        for (int i = 2; i < sqrt(tableSize); i++) {
            if (tableSize % i == 0) {
                isPrime = false;
                break;
            }
            isPrime = true;
        }
        tableSize++;
    }
    tableSize--;
    std::cout << "Using Hash Table with size: " << tableSize << std::endl;

    return tableSize;
}

void HashTable() {
	HASH_TABLE_SIZE = HashTableSize();
    CELL_SIZE = 2 * SPACING;

    hashTable.reserve(HASH_TABLE_SIZE);
}

int HashFunction(const Eigen::Vector2f position) {
	const int p1 = 73856093;
	const int p2 = 19349663;
	const int p3 = 83492791;

	int c = ((int)(position.x() / CELL_SIZE) * p1) ^ ((int)(position.y() / CELL_SIZE) * p2);
	c = abs(c) % HASH_TABLE_SIZE;
	return c;
}

void HashTableUpdate() {
	hashTable.clear();
    for (auto& p : particles) {
        int c = HashFunction(p.position);
        hashTable[c].push_back(&p);
    }
}
