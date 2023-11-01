#pragma once

#include <string>
#include <fstream>

// Read parameters for the SPH simulation from a .csv file
const std::string fileName = "parameters.csv";

// Window parameters
int WINDOW_WIDTH;
int WINDOW_HEIGHT;

// Initial grid parameters
int PARTICLES_PER_DIMENSION;
float SPACING;

// SPH parameters
float SUPPORT;
float REST_DENSITY;

std::ifstream file(fileName);

void readParameters() {
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
	}
}
