#include "factory.h"
#include <memory>


// class Wall
Wall::Wall(Point2D start, Point2D end, int elevation, int height)
        : start{start}, end{end}, elevation{elevation}, height{height} {}

// friend function
ostream &operator << (ostream &os, const Wall &wall){
    return os << "Wall from (" << wall.start.x << "," << wall.start.y << ") to ("
              << wall.end.x << "," << wall.end.y << ") at elevation " << wall.elevation
              << " and height " << wall.height;
}

// class SolidWall
SolidWall::SolidWall(const Point2D start, const Point2D end, const int elevation, const int height,
                      const int width, const Material material)
        : Wall(start, end, elevation, height), witdh{width}, material{material} {}

shared_ptr<SolidWall> SolidWall::create_main(Point2D start, Point2D end, int elevation, int height){
    if(elevation < 0) return nullptr;
    return make_shared<SolidWall>(SolidWall{start, end, elevation, height, 375, Material::aerated_concrete});
}

unique_ptr<SolidWall> SolidWall::create_partition(Point2D start, Point2D end, int elevation,
                                                  int height)
{
    return make_unique<SolidWall>(SolidWall{start, end, elevation, height, 120, Material::brick});
}

bool SolidWall::intersects(const Wall &other) const{
    return start.x < other.end.x && end.x > other.start.x &&
           start.y < other.end.y && end.y > other.start.y;
}


// class WallFactory
vector<weak_ptr<Wall>> WallFactory::walls;

shared_ptr<SolidWall> WallFactory::create_main(Point2D start, Point2D end, int elevation, int height){
    if(elevation < 0) return nullptr;
    return make_shared<SolidWall>(SolidWall{start, end, elevation, height, 375, Material::brick});
}

shared_ptr<SolidWall> WallFactory::create_partition(Point2D start, Point2D end, int elevation, int height){
    const auto this_wall = new SolidWall(start, end, elevation, height, 120, Material::brick);

    for(const auto &wall : walls){
        if(auto p = wall.lock()){
            if(this_wall->intersects(*p)){
                delete this_wall;
                return nullptr;
            }
        }
    }
    shared_ptr<SolidWall> ptr(this_wall);
    walls.push_back(ptr);
    return ptr;
}