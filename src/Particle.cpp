#include "..\headers\Particle.h"

Particle::Particle() :
    position(Eigen::Vector2f(0.0f, 0.0f)),
    velocity(Eigen::Vector2f(0.0f, 0.0f)),
    acceleration(Eigen::Vector2f(0.0f, 0.0f)),

    mass(SPACING * SPACING * REST_DENSITY),
    density(REST_DENSITY),
    pressure(0.0f),
    isFluid(true) {}

Eigen::Vector2i Particle::getCellNumber() {
	int x = std::floor(position.x() / SUPPORT);
	int y = std::floor(position.y() / SUPPORT);
	return Eigen::Vector2i(x, y);
}