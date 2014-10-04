#include "triangle.h"
#include <QVector2D>

Triangle::Triangle(bool acute, QPointF A, QPointF B, QPointF C, int acute_pattern, int obtuse_pattern, bool double_cut)
    : acute_(acute), acute_pattern_(acute_pattern), obtuse_pattern_(obtuse_pattern), double_cut_(double_cut)
{
    triangle_ << A << B << C << A;
}

void Triangle::generate()
{
    if (!sub_triangles_.empty())
        sub_triangles_.clear();

    if (acute_)
    {
        QPointF D, E;
        double length = QVector2D(triangle_[SommetC] - triangle_[SommetB]).length();
        if (double_cut_)
        {
            if (acute_pattern_ <= 1)
            {
                D = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetB], length);
                E = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetC], length);
                sub_triangles_.push_back(std::unique_ptr<Triangle>(new Triangle(true, triangle_[SommetA], D, E)));
                if (acute_pattern_ == 0)
                {
                    addSubTriangle(true, triangle_[SommetC], D, triangle_[SommetB]);
                    addSubTriangle(false, E, D, triangle_[SommetC]);
                }
                else
                {
                    addSubTriangle(true, triangle_[SommetB], triangle_[SommetC], E);
                    addSubTriangle(false, D, triangle_[SommetB], E);
                }
            }
            else if (acute_pattern_ == 2)
            {
                D = triangle_[SommetB] + getVector(triangle_[SommetB], triangle_[SommetA], length);
                E = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetC], length);
                addSubTriangle(true, triangle_[SommetB], E, D);
                addSubTriangle(true, triangle_[SommetB], triangle_[SommetC], E);
                addSubTriangle(false, D, E, triangle_[SommetA]);
            }
            else
            {
                D = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetB], length);
                E = triangle_[SommetC] + (QVector2D(triangle_[SommetA] - triangle_[SommetC]).normalized()*length).toPointF();
                E = triangle_[SommetC] + getVector(triangle_[SommetC], triangle_[SommetA], length);
                addSubTriangle(true, triangle_[SommetC], triangle_[SommetB], D);
                addSubTriangle(true, triangle_[SommetC], E, D);
                addSubTriangle(false, E, triangle_[SommetA], D);
            }
        }
        else
        {
            if (acute_pattern_)
            {
                D = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetB], length);
                addSubTriangle(true, triangle_[SommetC], D, triangle_[SommetB]);
                addSubTriangle(false, D, triangle_[SommetC], triangle_[SommetA]);
            }
            else
            {
                D = triangle_[SommetA] + getVector(triangle_[SommetA], triangle_[SommetC], length);
                addSubTriangle(true, triangle_[SommetB], triangle_[SommetC], D);
                addSubTriangle(false, D, triangle_[SommetA], triangle_[SommetB]);
            }
        }
    }
    else
    {
        QPointF D;
        double length = QVector2D(triangle_[SommetB] - triangle_[SommetA]).length();
        if (obtuse_pattern_)
        {
            D = triangle_[SommetB] + getVector(triangle_[SommetB], triangle_[SommetC], length);
            addSubTriangle(true, triangle_[SommetB], D, triangle_[SommetA]);
            addSubTriangle(false, D, triangle_[SommetC], triangle_[SommetA]);
        }
        else
        {
            D = triangle_[SommetC] + getVector(triangle_[SommetC], triangle_[SommetB], length);
            addSubTriangle(true, triangle_[SommetC], triangle_[SommetA], D);
            addSubTriangle(false, D, triangle_[SommetA], triangle_[SommetB]);
        }
    }
}

std::list<std::pair<QPolygonF, bool>> Triangle::getTriangles(unsigned int level)
{
    std::list<std::pair<QPolygonF, bool>> triangles;
    if (level)
    {
        if (sub_triangles_.empty())
            generate();

        for (std::unique_ptr<Triangle> &triangle : sub_triangles_)
            triangles.splice(triangles.begin(), triangle->getTriangles(level-1));
    }
    else
        triangles.push_back(std::pair<QPolygonF, bool>(triangle_, acute_));

    return triangles;
}

void Triangle::addSubTriangle(bool acute, QPointF &A, QPointF &B, QPointF &C)
{
    sub_triangles_.push_back(std::unique_ptr<Triangle>(new Triangle(acute, A, B, C, acute_pattern_, obtuse_pattern_, double_cut_)));
}

QPointF Triangle::getVector(QPointF &from, QPointF &to, double length)
{
    return (QVector2D(to - from).normalized()*length).toPointF();
}
