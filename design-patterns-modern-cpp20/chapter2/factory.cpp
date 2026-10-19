#include "factory.h"

// class Wall
Wall::Wall(Point2D start, Point2D end, int elevation, int height)
    : start{start}, end{end}, elevation{elevation}, height{height} {}

bool Wall::intersects(const Wall &other) const {
  return start.x < other.end.x && end.x > other.start.x &&
         start.y < other.end.y && end.y > other.start.y;
}

// friend function
ostream &operator<<(ostream &os, const Wall &wall) {
  return os << "Wall from (" << wall.start.x << "," << wall.start.y << ") to ("
            << wall.end.x << "," << wall.end.y << ") at elevation "
            << wall.elevation << " and height " << wall.height;
}

shared_ptr<Wall> Wall::BasicWallFactory::create_wall(Point2D start, Point2D end, int elevation, int height){
    return shared_ptr<Wall>(new Wall{start, end, elevation, height});
}

Wall::BasicWallFactory Wall::factory;

// class SolidWall
SolidWall::SolidWall(const Point2D start, const Point2D end,
                     const int elevation, const int height, const int width,
                     const Material material)
    : Wall(start, end, elevation, height), witdh{width}, material{material} {}

bool SolidWall::intersects(const Wall &other) const {

  return Wall::intersects(other) && witdh;
}

ostream &operator<<(ostream &os, const SolidWall &solid_wall) {
  return os << static_cast<const Wall &>(solid_wall) << " with width "
            << solid_wall.witdh << " and material "
            << static_cast<int>(solid_wall.material);
}

// class WallFactory
shared_ptr<Wall> WallFactory::create_wall(WallType type, Point2D start,
                                          Point2D end, int elevation,
                                          int height) {
  switch (type) {
  case WallType::main:
    return shared_ptr<SolidWall>(new SolidWall(
        start, end, elevation, height, 375, Material::aerated_concrete));
  case WallType::partition:
    return shared_ptr<SolidWall>(
        new SolidWall(start, end, elevation, height, 120, Material::brick));
  case WallType::basic:
    return shared_ptr<Wall>(new Wall{start, end, elevation, height});
  default:
    return nullptr;
  }
}