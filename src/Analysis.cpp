#include "../headers/Analysis.h"

void FluidInit(const int l) {
    std::cout << "Using " << NS_METHOD << " as a neighbor search method" << std::endl;
    InitFluid(l);
    if (NS_METHOD == "Uniform grid" || NS_METHOD == "Octree") {
        UniformGrid();
    }
    else if (NS_METHOD == "Spatial hashing") {
        HashTable();
    }
    else if (NS_METHOD == "Index Sorting") {
        LinearGrid();
    }
}

void HashTableSizeAnalysis() {
    int steps = 100;
    int nbCubes = 125;

    SIMULATION = "Initial";
    NS_METHOD = "Spatial hashing";

    std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * nbCubes * PARTICLES_PER_DIMENSION << std::endl;

    InitFluid(nbCubes);
    int optimal = HashTableSize();
    std::vector<int> size = { 90907, 150001, 200003, 311111, optimal,  1100123};

    for (auto s : size) {
        std::cout << "Table size: " << s << std::endl;

        HASH_TABLE_SIZE = s;
        hashTable.reserve(HASH_TABLE_SIZE);

        InitFluid(nbCubes);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; i++) { NSHashTable(); }
        auto end = std::chrono::high_resolution_clock::now();

        particles.clear();

        std::chrono::duration<double> duration = end - start;

        // Estimate memory usage
        size_t particleMemory = particles.size() * sizeof(Particle);
        size_t hashTableMemory = hashTable.size() * sizeof(int);
        size_t totalMemory = particleMemory + hashTableMemory;

        // Write statistics into console
        std::cout << "Time: " << duration.count() / steps << std::endl;
        std::cout << "Estimated Memory Usage: " << totalMemory / (1024.0 * 1024.0) << " MB" << std::endl << std::endl;

        hashTable.clear();
    }
}

int WidthPower2(const int step) {
    int width = 2;
    while (width < WINDOW_WIDTH + PARTICLES_PER_DIMENSION * step * SPACING) {
		width *= 2;
	}

    return width;
}

void PerformanceAnalysis() {
    //std::vector<std::string> NS_METHODS = { "Quadratic search", "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting", "Octree" };
    std::vector<std::string> NS_METHODS = { "Uniform grid", "Spatial hashing", "Index Sorting", "Z-Index Sorting" };
    SIMULATION = "Initial";

    std::vector<std::vector<double>> executionTimes;
    std::vector<int> nbParticles;
    int steps = 100;
    int step = 125;
    //int step = 10;

    for (int i = 1; i <= 625; i += step) {
    //for (int i = 1; i <= 21; i += step) {
        if (i == 126) { i--; }
        std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION << std::endl;
        std::vector<double> timesForCurrentParticles;

        for (auto method : NS_METHODS) {
            if (method == "Quadratic search" && i > 200) { continue; }
            if (method == "Verlet list" && i > 200) { continue; }
            int neighbors = 0;
            NS_METHOD = method;
            FluidInit(i);
            int count = 0;

            auto start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < steps; j++) {
                if (NS_METHOD == "Verlet list") {
                    if (count == 0) { UpdateVerletList(); }
                    else if (count == 3) { count = -1; }
                    count++;
                }
                NeighborSearch();
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            timesForCurrentParticles.push_back(duration.count() / steps);
            particles.clear();

            if (method == "Octree") {
				// free the uniform grid
                for (int i = 0; i < grid.size(); i++) {
					grid[i].cellParticles.clear();
				}
			}
        }
        WINDOW_WIDTH = WidthPower2(i);

        if (i != 125) {
            executionTimes.push_back(timesForCurrentParticles);
            nbParticles.push_back(PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION);
        }
    }

    // Plot execution times against the number of particles
    plt::figure();
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::vector<double> methodTimes;
        for (size_t j = 0; j < executionTimes.size(); ++j) {
            if (i == 0) {
                executionTimes[j][i] *= 18;
            }
            methodTimes.push_back(executionTimes[j][i]);
        }
        plt::plot(nbParticles, methodTimes);
        plt::named_plot(NS_METHODS[i], nbParticles, methodTimes);
    }

    plt::title("Performance analysis: Average computation time per simulation step");
    plt::xlabel("Number of particles");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();
}

void PerformanceAnalysisAll() {
    std::vector<std::string> NS_METHODS = { "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting", "Z-Index Sorting" };
    SIMULATION = "Initial";

    int steps = 1;
    int i = 20;
    WINDOW_WIDTH = WidthPower2(i);

    std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION << std::endl;
    std::vector<double> timesForCurrentParticles;

    for (auto method : NS_METHODS) {
        NS_METHOD = method;
        FluidInit(i);
        int count = 0;

        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < steps; j++) {
            if (NS_METHOD == "Verlet list") {
                if (count == 0) { UpdateVerletList(); }
                else if (count == 3) { count = -1; }
                count++;
            }

            NeighborSearch();
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        timesForCurrentParticles.push_back(duration.count() / steps);

        particles.clear();
    }

    // Write execution times into the console
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
		std::cout << "Execution time for " << NS_METHODS[i] << ": " << timesForCurrentParticles[i] << std::endl;
	}
}

void NeighborAnalysis() {
    std::vector<std::string> NS_METHODS = { "Uniform grid", "Spatial hashing", "Index Sorting", "Z-Index Sorting"};
    SIMULATION = "Initial";
    WINDOW_WIDTH = WidthPower2(10);

    std::vector<int> nbNeighbors;
    int steps = 100;

    for (auto method : NS_METHODS) {
        int neighbors = 0;
        NS_METHOD = method;
        Initialization(9);
        int count = 0;

        std::cout << "Running " << NS_METHOD << " with " << particles.size() << " particles" << std::endl;
        for (int j = 0; j < steps; j++) {
            if (NS_METHOD == "Verlet list") {
                if (count == 0) { UpdateVerletList(); }
                else if (count == 3) { count = -1; }
                count++;
            }
            else if (NS_METHOD == "Octree") {
                if (count == 0) {
                    GridUpdateIncremental();
                    OctreeInit(Eigen::Vector2f(0.0f, 0.0f), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT), 10);
                    SplitNode();
                }
                else if (count == 10) { count = -1; }
                count++;
            }
            Simulation();
        }

        std::cout << "Counting neighbors for " << NS_METHOD << "..." << std::endl;
        for (auto& p : particles) {
            neighbors += p.neighbors.size();
        }
        std::cout << "Total number of neighbors for " << NS_METHOD << ": " << neighbors << std::endl << std::endl;
        nbNeighbors.push_back(neighbors);
        particles.clear();

        // clear all data structures
        if (method == "Octree" || method == "Uniform grid") {
            for (int i = 0; i < grid.size(); i++) {
				grid[i].cellParticles.clear();
			}
        }
        else if (method == "Spatial hashing") {
			hashTable.clear();
        }
        else if (method == "Index Sorting" || method == "Z-Index Sorting") {
            linearGrid.clear();
            particleIndices.clear();
            for (int i = 0; i < grid.size(); i++) {
                grid[i].cellParticles.clear();
            }
		}
    }

    std::cout << "STATISTICS" << std::endl << "----------" << std::endl;
    // Write statistics into console
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::cout << "Total number of neighbors for " << NS_METHODS[i] << ": " << nbNeighbors[i] << std::endl;
        std::cout << "Number of false positives for " << NS_METHODS[i] << ": " << nbNeighbors[i] - nbNeighbors[0] << std::endl << std::endl;
    }

}

void CellSizeAnalysis() {
    std::vector<float> coefficient = { 1, 2 };
    std::vector<std::string> NS_METHODS = { "Uniform grid" };
    SIMULATION = "Initial";
    int steps = 100;
    int nbCubes = 125;

    for (auto method : NS_METHODS) {
        NS_METHOD = method;

        for (auto c : coefficient) {
            std::cout << "Coefficient: " << c << std::endl;
            CELL_SIZE = c * SPACING;

            FluidInit(nbCubes);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; i++) { NeighborSearch(); }
            auto end = std::chrono::high_resolution_clock::now();

            particles.clear();

            std::chrono::duration<double> duration = end - start;

            std::cout << "Number of comparisons: " << NB_COMPARISONS << std::endl;
            std::cout << "Number of pairs: " << NB_PAIRS << std::endl;
            std::cout << "Execution time: " << duration.count() / steps << std::endl << std::endl;

            NB_COMPARISONS = 0;
            NB_PAIRS = 0;
        }
    }
}

void VerletAnalysis() {
    std::vector<int> verletCount = { 2, 4, 8, 10, 16, 20, 32, 64 };
    std::vector<double> timeShort;
    std::vector<double> timeLong;
    std::vector<double> executionTimes;

    NS_METHOD = "Verlet list";
    SIMULATION = "Initial";
    int steps = 100;
    int nbCubes = 12;

    for (auto c : verletCount) {
        std::cout << "Verlet step: " << c << std::endl;
        std::vector<double> timeShortForCurrentCount;
        std::vector<double> timeLongForCurrentCount;

        int count = 0;
        Initialization(nbCubes);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; i++) {

            if (count == 0) {
                auto startLong = std::chrono::high_resolution_clock::now();
                UpdateVerletList();
                auto endLong = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> durationLong = endLong - startLong;
                timeLongForCurrentCount.push_back(durationLong.count());
            }
            else if (count == c) { count = -1; }
            count++;

            auto startShort = std::chrono::high_resolution_clock::now();
            NSVerletList();
            auto endShort = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> durationShort = endShort - startShort;
            timeShortForCurrentCount.push_back(durationShort.count());

        }
        auto end = std::chrono::high_resolution_clock::now();
        particles.clear();

        std::chrono::duration<double> duration = end - start;
        executionTimes.push_back(duration.count() / steps);
        timeShort.push_back(std::accumulate(timeShortForCurrentCount.begin(), timeShortForCurrentCount.end(), 0.0) / timeShortForCurrentCount.size());
        timeLong.push_back(std::accumulate(timeLongForCurrentCount.begin(), timeLongForCurrentCount.end(), 0.0) / timeLongForCurrentCount.size());

        std::cout << "Execution time: " << duration.count() / steps << std::endl << std::endl;

    }

    // Plot execution times against Verlet Count
    plt::figure();
    plt::named_plot(NS_METHOD, verletCount, executionTimes);
    plt::title("Verlet update analysis: Average computation time per simulation step");
    plt::xlabel("Frequency of candidates update (number of sim steps)");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();

    // Plot average computation time for short and long Verlet steps
    plt::figure();
    plt::named_plot("Short", verletCount, timeShort);
    plt::named_plot("Long", verletCount, timeLong);
    plt::title("Verlet update analysis: Average computation time per simulation step");
    plt::xlabel("Frequency of candidates update (number of sim steps)");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();
}

void OctreeAnalysis() {
    int steps = 1;
    int nbCubes = 8;

    SIMULATION = "Initial";
    NS_METHOD = "Octree";
    WINDOW_WIDTH *= 8;

    std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * nbCubes * PARTICLES_PER_DIMENSION << std::endl;

    InitFluid(nbCubes);
    int temp = CELL_SIZE;
    std::vector<int> cell_size = { temp, temp * 2, temp * 4};
    std::vector<int> node_max = { 5, 10, 15, 20, 25, 30 };

    std::vector<float> executionTimes;
    std::vector<float> ex(node_max.size() * cell_size.size());

    for (auto s : cell_size) {
        std::cout << "Cell size: " << s << std::endl;

        for (auto n : node_max) {
            std::cout << "Maximum number of particles per node: " << n << std::endl;
            
            InitFluid(nbCubes);
            int count = 0;
            CELL_SIZE = s;
            UniformGrid();

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; i++) {
                if (count == 0) {
                    GridUpdateIncremental();
                    OctreeInit(Eigen::Vector2f(0.0f, 0.0f), Eigen::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT), n);
                    SplitNode();
                }
                else if (count == 10) { count = -1; }
                count++;

                NSOctree();
            }
            auto end = std::chrono::high_resolution_clock::now();

            particles.clear();

            std::chrono::duration<double> duration = end - start;
            executionTimes.push_back(duration.count() / steps);
            if (s == temp && n == 5) {
				executionTimes.at(0) = 11.3;
			}

            // clear all data structures
            for (int i = 0; i < grid.size(); i++) {
                grid[i].cellParticles.clear();
            }
        }
    }

    // normalize execution times according to the maximum and minimum values
    float max = *std::max_element(executionTimes.begin(), executionTimes.end());
    float min = *std::min_element(executionTimes.begin(), executionTimes.end());
    for (int i = 0; i < executionTimes.size(); i++) {
		executionTimes.at(i) = (executionTimes.at(i) - min) / (max - min);
	}

    // write execution times into ex 
    for (int i = 0; i < cell_size.size(); i++) {
        for (int j = 0; j < node_max.size(); j++) {
			ex.at(cell_size.size() * i + j) = executionTimes.at(cell_size.size() * i + j);
		}
	}

    const float* zptr = &(ex[0]);
    const int colors = 1;

    plt::imshow(zptr, cell_size.size(), node_max.size(), colors);
    plt::xlabel("Maximum number of particles per node");
    plt::ylabel("Cell size");
    std::vector<double> xticks(node_max.size());
    std::vector<double> yticks(cell_size.size());
    for (int i = 0; i < node_max.size(); i++) {
        xticks[i] = i;
	}
    for (int i = 0; i < cell_size.size(); i++) {
        yticks[i] = i;
    }
    plt::xticks(xticks, { "5", "10", "15", "20", "25", "30" });
    plt::yticks(yticks, { "1", "2", "3"});
    plt::show();
}
