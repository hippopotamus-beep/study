#ifndef FACTORY_H
#define FACTORY_H

#include <vector>
#include <memory>

using namespace std;

class Point2D{
public:
    int x, y;
};

class Wall{
friend ostream &operator << (ostream &os, const Wall &wall);
friend class SolidWall;
public:
    Wall(Point2D start, Point2D end, int elevation, int height);

protected:
    Point2D start, end;
private:
    int elevation, height;
};

enum class Material{
    brick,
    aerated_concrete,
    drywall
};

class SolidWall : public Wall{
friend class WallFactory;
private:
    int witdh;
    Material material;
protected:
    SolidWall(const Point2D start, const Point2D end, const int evelation, const int height,
              const int width, const Material material);

public:
    static shared_ptr<SolidWall> create_main(Point2D start, Point2D end, int elevation, int height);
    static unique_ptr<SolidWall> create_partition(Point2D start, Point2D end, int elevation, 
        int height);

    bool intersects(const Wall &other) const;
};

class WallFactory{
private:
    static vector<weak_ptr<Wall>> walls;
public:
    static shared_ptr<SolidWall> create_main(Point2D start, Point2D end, int elevation, int height);
    static shared_ptr<SolidWall> create_partition(Point2D start, Point2D end, int elevation, int height);
};

#endif