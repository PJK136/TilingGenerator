#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <utility>
#include <list>
#include <array>
#include <memory>
#include <QPolygonF>

class Triangle
{
    enum {SommetA = 0, SommetB = 1, SommetC = 2};

public:
    Triangle(bool acute, QPointF A, QPointF B, QPointF C, int acute_pattern = 0, int obtuse_pattern = 0, bool double_cut = true);
    void generate();
    std::list<std::pair<QPolygonF, bool>> getTriangles(unsigned int level);

private:
    inline void addSubTriangle(bool acute, QPointF &A, QPointF &B, QPointF &C);
    static inline QPointF getVector(QPointF &from, QPointF &to, double length);
    bool acute_;
    int acute_pattern_;
    int obtuse_pattern_;
    bool double_cut_;
    QPolygonF triangle_;
    std::list<std::unique_ptr<Triangle>> sub_triangles_;
};

#endif // TRIANGLE_H
