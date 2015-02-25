#include "triangle.h"
#include <QLineF>

Triangle::Triangle(bool acute, QPointF A, QPointF B, QPointF C, int acute_pattern, int obtuse_pattern, bool acute_double_cut, bool obtuse_double_cut)
    : Triangle(acute, QPolygonF({A, B, C}), std::make_shared<Properties>(acute_pattern, obtuse_pattern, acute_double_cut, obtuse_double_cut))
{
}

Triangle::Triangle(bool acute, QPolygonF triangle, int acute_pattern, int obtuse_pattern, bool acute_double_cut, bool obtuse_double_cut)
    : Triangle(acute, triangle, std::make_shared<Properties>(acute_pattern, obtuse_pattern, acute_double_cut, obtuse_double_cut))
{

}

Triangle::Triangle(bool acute, QPointF A, QPointF B, QPointF C, std::shared_ptr<Properties> properties)
    : Triangle(acute, QPolygonF({A, B, C}), properties)
{
}

Triangle::Triangle(bool acute, QPolygonF triangle, std::shared_ptr<Triangle::Properties> properties) : acute_(acute), triangle_(triangle), properties_(properties)
{
    if (acute_ && properties_->acute_pattern_ % 2)
        invert();
    else if (!acute_ && properties_->obtuse_pattern_ % 2)
        invert();
}


void Triangle::invert()
{
    std::swap(triangle_[SommetB], triangle_[SommetC]);
}

double Triangle::getBaseLength()
{
    if (acute_)
        return QLineF(triangle_[SommetB], triangle_[SommetC]).length();
    else
        return QLineF(triangle_[SommetA], triangle_[SommetB]).length();
}

std::pair<Triangle, Triangle> Triangle::cut()
{
    QPointF &A = triangle_[SommetA], &B = triangle_[SommetB], &C = triangle_[SommetC];
    if (acute_)
    {
        QPointF D = A + getVector(A, B, getBaseLength());
        return std::make_pair(Triangle(true, C, D, B, properties_), Triangle(false, D, C, A, properties_));
    }
    else
    {
        QPointF D = B + getVector(B, C, getBaseLength());
        return std::make_pair(Triangle(true, B, A, D, properties_), Triangle(false, D, C, A, properties_));
    }
}

void Triangle::generate()
{
    if (!sub_triangles_.empty())
        sub_triangles_.clear();

    std::pair<Triangle, Triangle> triangle = cut();
    if (acute_ && properties_->acute_double_cut_)
    {
        if (properties_->obtuse_pattern_ % 2)
            triangle.second.invert(); //Cancel invert by obtuse pattern

        if (properties_->acute_pattern_ > 1)
            triangle.second.invert();

        std::pair<Triangle, Triangle> triangle2 = triangle.second.cut();
        sub_triangles_.push_back(triangle.first);
        sub_triangles_.push_back(triangle2.first);
        sub_triangles_.push_back(triangle2.second);
    }
    else if (!acute_ && properties_->obtuse_double_cut_)
    {
        if (properties_->acute_pattern_ % 2)
            triangle.first.invert(); //Cancel invert by obtuse pattern

        if (properties_->obtuse_pattern_ > 1)
            triangle.first.invert();

        std::pair<Triangle, Triangle> triangle2 = triangle.first.cut();
        sub_triangles_.push_back(triangle.second);
        sub_triangles_.push_back(triangle2.first);
        sub_triangles_.push_back(triangle2.second);
    }
    else
    {
        sub_triangles_.push_back(triangle.first);
        sub_triangles_.push_back(triangle.second);
    }
}

std::list<std::pair<QPolygonF, bool>> Triangle::getTriangles(unsigned int level)
{
    std::list<std::pair<QPolygonF, bool>> triangles;
    if (level)
    {
        if (sub_triangles_.empty())
            generate();

        for (Triangle &triangle : sub_triangles_)
            triangles.splice(triangles.begin(), triangle.getTriangles(level-1));
    }
    else
        triangles.push_back(std::pair<QPolygonF, bool>(triangle_, acute_));

    return triangles;
}
