#include "../headers/Analysis.h"

void HashTableSizeAnalysis() {
    std::vector<float> coefficient = { 1, 1.5, 1.7, 1.9, 2, 2.1, 2.3, 2.5, 3, 4 };
    std::vector<std::string> NS_METHODS = { "Spatial hashing" };
    SIMULATION = "Initial";
    int steps = 1000;
    int nbCubes = 16;

    std::vector<std::vector<double>> executionTimes;
    for (auto method : NS_METHODS) {
        NS_METHOD = method;
        std::vector<double> timesForCurrentMethod;

        for (auto c : coefficient) {
            std::cout << "Coefficient: " << c << std::endl;
            CELL_SIZE = c * SPACING;

            Initialization(nbCubes);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; i++) { NeighborSearch(); }
            auto end = std::chrono::high_resolution_clock::now();

            particles.clear();

            std::chrono::duration<double> duration = end - start;
            timesForCurrentMethod.push_back(duration.count() / steps);
        }
        executionTimes.push_back(timesForCurrentMethod);
    }

    // Plot execution times against cell size
    plt::figure();
    for (int i = 0; i < NS_METHODS.size(); ++i) {
        plt::named_plot(NS_METHODS[i], coefficient, executionTimes[i]);
    }
    plt::title("Cell size analysis: Average computation time per simulation step");
    plt::xlabel("Cell size coefficient");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();
}

void PerformanceAnalysis() {
    //std::vector<std::string> NS_METHODS = { "Quadratic search", "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting" };
    //std::vector<std::string> NS_METHODS = { "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting" };
    std::vector<std::string> NS_METHODS = { "Uniform grid", "Spatial hashing", "Index Sorting" };
    //std::vector<std::string> NS_METHODS = { "Uniform grid" };
    SIMULATION = "Initial";

    std::vector<std::vector<double>> executionTimes;
    std::vector<int> nbParticles;
    int steps = 1000;

    for (int i = 4; i <= 164; i += 10) {
        std::cout << "Number of fluid particles: " << PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION << std::endl;
        std::vector<double> timesForCurrentParticles;

        for (auto method : NS_METHODS) {
            int neighbors = 0;
            NS_METHOD = method;
            Initialization(i);
            int count = 0;

            auto start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < steps; j++) {
                if (NS_METHOD == "Verlet list") {
                    if (count == 0) { UpdateVerletList(); }
                    else if (count == 10) { count = -1; }
                    count++;
                }
                Simulation();
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            timesForCurrentParticles.push_back(duration.count() / steps);
            particles.clear();
        }

        executionTimes.push_back(timesForCurrentParticles);
        WINDOW_WIDTH += PARTICLES_PER_DIMENSION * SUPPORT;
        nbParticles.push_back(PARTICLES_PER_DIMENSION * i * PARTICLES_PER_DIMENSION);
    }

    // Plot execution times against the number of particles
    plt::figure();
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::vector<double> methodTimes;
        for (size_t j = 0; j < executionTimes.size(); ++j) {
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

void NeighborAnalysis() {
    std::vector<std::string> NS_METHODS = { "Quadratic search", "Verlet list", "Uniform grid", "Spatial hashing", "Index Sorting" };
    SIMULATION = "Initial";

    std::vector<int> nbParticles;
    std::vector<int> nbNeighbors;
    int steps = 1000;

    for (auto method : NS_METHODS) {
        int neighbors = 0;
        NS_METHOD = method;
        Initialization(4); // check for a big scene
        int count = 0;

        for (int j = 0; j < steps; j++) {
            if (NS_METHOD == "Verlet list") {
                if (count == 0) { UpdateVerletList(); }
                else if (count == 10) { count = -1; }
                count++;
            }
            NeighborSearch();
        }

        for (auto& p : particles) {
            neighbors += p.neighbors.size();
        }
        nbNeighbors.push_back(neighbors);
        particles.clear();
    }

    // Write statistics into console
    std::cout << "Total number of neighbors for " << NS_METHODS[0] << ": " << nbNeighbors[0] << std::endl << std::endl;
    for (size_t i = 0; i < NS_METHODS.size(); ++i) {
        std::cout << "Total number of neighbors for " << NS_METHODS[i] << ": " << nbNeighbors[i] << std::endl;
        std::cout << "Number of false positives for " << NS_METHODS[i] << ": " << nbNeighbors[i] - nbNeighbors[0] << std::endl << std::endl;
    }

}

void CellSizeAnalysis() {
    std::vector<float> coefficient = { 1, 1.5, 1.7, 1.9, 2, 2.1, 2.3, 2.5, 3, 4 };
    std::vector<std::string> NS_METHODS = { "Spatial hashing" };
    SIMULATION = "Initial";
    int steps = 1000;
    int nbCubes = 16;

    std::vector<std::vector<double>> executionTimes;
    for (auto method : NS_METHODS) {
        NS_METHOD = method;
        std::vector<double> timesForCurrentMethod;

        for (auto c : coefficient) {
            std::cout << "Coefficient: " << c << std::endl;
            CELL_SIZE = c * SPACING;

            Initialization(nbCubes);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; i++) { NeighborSearch(); }
            auto end = std::chrono::high_resolution_clock::now();

            particles.clear();

            std::chrono::duration<double> duration = end - start;
            timesForCurrentMethod.push_back(duration.count() / steps);
        }
        executionTimes.push_back(timesForCurrentMethod);
    }

    // Plot execution times against cell size
    plt::figure();
    for (int i = 0; i < NS_METHODS.size(); ++i) {
        plt::named_plot(NS_METHODS[i], coefficient, executionTimes[i]);
    }
    plt::title("Cell size analysis: Average computation time per simulation step");
    plt::xlabel("Cell size coefficient");
    plt::ylabel("Time [s]");
    plt::legend();
    plt::show();
    plt::close();
}

void VerletAnalysis() {
    std::vector<int> verletCount = { 2, 4, 8, 10, 16, 20, 32, 64 };
    std::vector<double> timeShort;
    std::vector<double> timeLong;
    std::vector<double> executionTimes;

    NS_METHOD = "Verlet list";
    SIMULATION = "Initial";
    int steps = 1000;
    int nbCubes = 16;

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
