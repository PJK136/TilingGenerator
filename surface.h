#ifndef SURFACE_H
#define SURFACE_H

#include <utility>
#include <list>
#include <unordered_map>
#include <memory>
#include <QPolygonF>
#include <QLineF>
#include <cmath>

class Properties
{
public:
    Properties(QString source);

private:
    struct Rules
    {
        std::list<std::pair<char, char>> lines;
        std::list<std::pair<double, char>> points;
        std::list<std::pair<std::list<char>, unsigned int>> surfaces;
    };

    std::vector<Rules> rules;
    std::vector<QPolygonF> defaults;

    friend class Surface;
};

inline QPointF getVector(QPointF &from, QPointF &to, double length=1.)
{
    double dx = to.x()-from.x();
    double dy = to.y()-from.y();
    length /= sqrt(dx*dx + dy*dy);
    return QPointF(dx*length, dy*length);
}

class Surface
{
public:
    Surface(unsigned int type, std::shared_ptr<const Properties> properties);
    Surface(const QPolygonF &surface, unsigned int type, std::shared_ptr<const Properties> properties);
    ~Surface();
    void generate();
    std::list<std::pair<QPolygonF, int>> getPolygons(unsigned int level, int exclude = -1);
    int type() const;
    QPolygonF polygon() const;
    std::shared_ptr<const Properties> properties() const;

private:
    QPolygonF polygon_;
    int type_;
    std::list<Surface> sub_surfaces_;
    std::shared_ptr<const Properties> properties_;
};

#endif // SURFACE_H
