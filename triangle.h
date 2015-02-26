#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <utility>
#include <list>
#include <memory>
#include <QPolygonF>
#include <QVector2D>

static inline QPointF getVector(QPointF &from, QPointF &to, double length)
{
    return (QVector2D(to - from).normalized()*length).toPointF();
}

class Triangle
{
    enum {SommetA = 0, SommetB = 1, SommetC = 2};

public:
    Triangle(bool acute, QPointF A, QPointF B, QPointF C, int acute_pattern = 0, int obtuse_pattern = 0, bool acute_double_cut = true, bool obtuse_double_cut = false);
    Triangle(bool acute, QPolygonF triangle, int acute_pattern = 0, int obtuse_pattern = 0, bool acute_double_cut = true, bool obtuse_double_cut = false);
    void invert();
    double getBaseLength();
    std::pair<Triangle, Triangle> cut();
    void generate();
    std::list<std::pair<QPolygonF, bool>> getTriangles(unsigned int level);

private:
    struct Properties
    {
        Properties(int acute_pattern = 0, int obtuse_pattern = 0, bool acute_double_cut = true, bool obtuse_double_cut = false) :
            acute_pattern_(acute_pattern), obtuse_pattern_(obtuse_pattern), acute_double_cut_(acute_double_cut), obtuse_double_cut_(obtuse_double_cut) {}
        int acute_pattern_;
        int obtuse_pattern_;
        bool acute_double_cut_;
        bool obtuse_double_cut_;
    };

    Triangle(bool acute, QPointF A, QPointF B, QPointF C, std::shared_ptr<Properties> properties);
    Triangle(bool acute, QPolygonF, std::shared_ptr<Properties> properties);

    bool acute_;
    QPolygonF triangle_;
    std::vector<Triangle> sub_triangles_;
    std::shared_ptr<Properties>  properties_;
};

#endif // TRIANGLE_H
