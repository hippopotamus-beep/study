#ifndef FACTORY_H
#define FACTORY_H

// #include <vector>
#include <ostream>
#include <memory>

using namespace std;

class Point2D{
public:
    int x, y;
};

enum class WallType{
    basic,
    main,
    partition
};

class Wall{
friend ostream &operator << (ostream &os, const Wall &wall);
public:
    Wall(Point2D start, Point2D end, int elevation, int height);
    virtual ~Wall() = default;

protected:
    virtual bool intersects(const Wall &other) const;

protected:
    Point2D start, end;
    int elevation, height;

private:
    class BasicWallFactory{
        friend class Wall;
        BasicWallFactory() = default;
    public:
        shared_ptr<Wall> create_wall(Point2D start, Point2D end, int elevation, int height);
    };
public:
    static BasicWallFactory factory;
};

enum class Material{
    brick,
    aerated_concrete,
    drywall
};

class SolidWall : public Wall{
friend ostream &operator << (ostream &os, const SolidWall &solid_wall);
friend class WallFactory;
private:
    int witdh;
    Material material;
protected:
    SolidWall(const Point2D start, const Point2D end, const int evelation, const int height,
              const int width, const Material material);

    bool intersects(const Wall &other) const;
};

class WallFactory{
// private:
//     static vector<weak_ptr<Wall>> walls;
public:
    static shared_ptr<Wall> create_wall(WallType type, Point2D start, Point2D end, int elevation, int height);
};

#endif