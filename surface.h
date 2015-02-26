#ifndef SURFACE_H
#define SURFACE_H

#include <utility>
#include <list>
#include <unordered_map>
#include <memory>
#include <QPolygonF>
#include <QLineF>

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

    friend class Surface;
};

typedef std::hash<double>::result_type hash_type;

class QLineFHasher
{
public:
    hash_type operator ()(const QLineF& line) const
    {
        std::hash<float> hasher;
        hash_type hash_p1 = 0;
        hash_combine(hash_p1, hasher((float)line.p1().x()));
        hash_combine(hash_p1, hasher((float)line.p1().y()));
        hash_type hash_p2 = 0;
        hash_combine(hash_p2, hasher((float)line.p2().x()));
        hash_combine(hash_p2, hasher((float)line.p2().y()));
        return hash_p1 ^ hash_p2;
    }
private:
    inline void hash_combine(hash_type& seed, hash_type value) const
    {
        static hash_type magic_number = static_cast<hash_type>((std::sqrt(5.)-1) * ((hash_type)1 << (sizeof(hash_type)*8-1)));
        seed ^= value + magic_number + (seed<<6) + (seed>>2);
    }
};

class QLineFCompare
{
public:
    bool operator ()(const QLineF& a, const QLineF &b) const
    {
        return (a.p1() == b.p1() && a.p2() == b.p2()) || (a.p1() == b.p2() && b.p2() == a.p1());
    }
};

typedef std::unordered_map<QLineF, int, QLineFHasher, QLineFCompare> lines_map;

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
    Surface(const QPolygonF &surface, unsigned int type, std::shared_ptr<Properties> properties);
    ~Surface();
    void generate();
    std::list<std::pair<QPolygonF, int>> getPolygons(unsigned int level, int exclude = -1);
    void getLines(unsigned int level, lines_map &lines);
    int type() const;
    QPolygonF polygon() const;

private:
    QPolygonF polygon_;
    int type_;
    std::list<Surface> sub_surfaces_;
    std::shared_ptr<Properties> properties_;
};

#endif // SURFACE_H
