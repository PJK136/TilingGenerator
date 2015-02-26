#include "surface.h"
#include <cmath>
#include <QLineF>
#include <QString>
#include <QTextStream>

#define PHI (1. + sqrt(5.))/2

Surface::Surface(const QPolygonF &polygon, unsigned int type, std::shared_ptr<Properties> properties)
{
    polygon_ = polygon;
    type_ = type;
    properties_ = properties;
}

Surface::~Surface()
{

}

void Surface::generate()
{
    if (!sub_surfaces_.empty())
        sub_surfaces_.clear();

    const Properties::Rules &rules = properties_->rules[type_];

    std::vector<QLineF> lines;
    for (const std::pair<char, char> &line : rules.lines)
    {
        lines.push_back(QLineF(polygon_.at(line.first), polygon_.at(line.second)));
    }

    std::vector<QPointF> points = polygon_.toStdVector();

    for (const std::pair<double, char> &point : rules.points)
    {
        QLineF line(lines[point.second]);
        line.setLength(line.length()*point.first);
        points.push_back(line.p2());
    }

    for (const std::pair<std::list<char>, unsigned> &surface : rules.surfaces)
    {
        QPolygonF polygon;
        for (char point : surface.first)
            polygon.push_back(points[point]);
        sub_surfaces_.push_back(Surface(polygon, surface.second, properties_));
    }
}

std::list<std::pair<QPolygonF, int>> Surface::getPolygons(unsigned int level, int exclude)
{
    std::list<std::pair<QPolygonF, int>> surfaces;
    if (level)
    {
        if (sub_surfaces_.empty())
            generate();

        for (Surface &surface : sub_surfaces_)
            surfaces.splice(surfaces.begin(), surface.getPolygons(level-1, exclude));
    }
    else
    {
        if (type_ != exclude)
            surfaces.push_back(std::make_pair(polygon_, type_));
    }

    return surfaces;
}

void Surface::getLines(unsigned int level, lines_map &lines)
{
    if (level)
    {
        if (sub_surfaces_.empty())
            generate();

        for (Surface &surface : sub_surfaces_)
            surface.getLines(level-1, lines);
    }
    else
    {
        for (int i = 0; i < polygon_.size(); i++)
        {
            QLineF line;

            if (i == polygon_.size()-1)
                line.setPoints(polygon_[polygon_.size()-1], polygon_[0]);
            else
                line.setPoints(polygon_[i], polygon_[i+1]);

            if (lines.find(line) == lines.end())
                lines[line] = type_;
            else if (lines[line] == type_)
                lines[line] = -1;
        }
    }
}

int Surface::type() const
{
    return type_;
}

QPolygonF Surface::polygon() const
{
    return polygon_;
}


Properties::Properties(QString source)
{
    int i = -1, mode = 0;
    QTextStream stream(&source);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        if (line.isEmpty())
            continue;

        if (line.contains("L"))
        {
            rules.push_back(Properties::Rules());
            i++;
            mode = 0;
        }
        else if (line.contains("P"))
            mode = 1;
        else if (line.contains("S"))
            mode = 2;
        else
        {
            QStringList args = line.split(" ");
            if (args.isEmpty())
                continue;

            if (mode == 0)
                rules[i].lines.push_back({args[0].toUInt(), args[1].toUInt()});
            else if (mode == 1)
                rules[i].points.push_back({args[0].toDouble(), args[1].toUInt()});
            else
            {
                unsigned int type = args.takeFirst().toUInt();
                std::list<char> points;
                for (QString string : args)
                    points.push_back(string.toUInt());
                rules[i].surfaces.push_back({points, type});
            }
        }
    }
}
