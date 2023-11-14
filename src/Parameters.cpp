#include "..\headers\Parameters.h"

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int PARTICLE_NEIGHBORS;

int PARTICLES_PER_DIMENSION;
float SPACING;

float SUPPORT;
float REST_DENSITY;
float TIME_STEP;
float STIFFNESS;
float VISCOSITY;

Eigen::Vector2f GRAVITY = Eigen::Vector2f(0.0f, -9.8f);

std::string NS_METHOD;
std::string SIMULATION;

void readParameters() {
	std::ifstream file(fileName);

	if (!file.is_open()) {
		std::cout << "Error opening file " << fileName << std::endl;
		std::exit(1);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string parameterName;
		std::string parameterValue;
		std::getline(iss, parameterName, ',');
		std::getline(iss, parameterValue, ',');
		if (parameterName == "window_width") {
			WINDOW_WIDTH = std::stoi(parameterValue);
		}
		else if (parameterName == "window_hight") {
			WINDOW_HEIGHT = std::stoi(parameterValue);
		}
		else if (parameterName == "spacing") {
			SPACING = std::stod(parameterValue);
		}
		else if (parameterName == "support") {
			SUPPORT = SPACING * std::stod(parameterValue);
		}
		else if (parameterName == "density") {
			REST_DENSITY = std::stod(parameterValue);
		}
		else if (parameterName == "particles") {
			PARTICLES_PER_DIMENSION = std::stoi(parameterValue);
		}
		else if (parameterName == "timestep") {
			TIME_STEP = std::stod(parameterValue);
		}
		else if (parameterName == "stiffness") {
			STIFFNESS = std::stod(parameterValue);
		}
		else if (parameterName == "viscosity") {
			VISCOSITY = std::stod(parameterValue);
		}
		else if (parameterName == "neighbors") {
			PARTICLE_NEIGHBORS = std::stoi(parameterValue);
		} else if (parameterName == "method") {
			const int method = std::stoi(parameterValue);
			switch (method) {
			case 0: NS_METHOD = "Quadratic search"; break;
			case 1: NS_METHOD = "Verlet list"; break;
			case 2: NS_METHOD = "Uniform grid"; break;
			case 3: NS_METHOD = "Spatial hashing"; break;
			}
			std::cout << "Using " << NS_METHOD << " for neighbor search" << std::endl;
		}
		else if (parameterName == "simulation") {
			const int simulation = std::stoi(parameterValue);
			switch (simulation) {
			case 0: SIMULATION = "Initial"; WINDOW_HEIGHT /= 2; WINDOW_WIDTH /= 2.5; break;
			case 1: SIMULATION = "Dam break"; PARTICLES_PER_DIMENSION *= 2; break;
			case 2: SIMULATION = "Complex boundaries"; break;
			}
			std::cout << "Using " << SIMULATION << " simulation" << std::endl;
		}
	}
}
