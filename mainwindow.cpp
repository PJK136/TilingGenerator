#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "surface.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPolygonF>
#include <QPen>
#include <QMessageBox>
#include <cmath>
#include <QFileDialog>
#include <QTime>
#include <QDateTime>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751
#endif

#define PHI (1. + sqrt(5.))/2

#define BIT(x,n) QString::number(((x) >> (n)) & 1)
#define BITSTOQSTRING(x) BIT(x, 0), BIT(x, 1), BIT(x, 2), BIT(x, 3), BIT(x, 4), BIT(x, 5), BIT(x, 6), BIT(x, 7)

QString penrose[] = {"L\n"
                     "1 2\n"
                     "2 0\n"
                     "P\n"
                     "0.6180339887498948482045868343656381177203091798057628 0\n"
                     "0.6180339887498948482045868343656381177203091798057628 1\n"
                     "S\n"
                     "0 4 0 1\n"
                     "0 4 3 1\n"
                     "1 2 4 3\n"
                     "\n"
                     "L\n"
                     "0 1\n"
                     "P\n"
                     "0.6180339887498948482045868343656381177203091798057628 0\n"
                     "S\n"
                     "0 2 3 0\n"
                     "1 1 2 3\n"
                     "\n"
                     "D\n"
                     "0 0 1 0 0.5 1.5388417685876267012851452880184549120033510717688962\n" // h = sqrt(2.*sqrt(5.)+5.)/2.
                     "0 0 1.6180339887498948482045868343656381177203091798057628 0 0.8090169943749474241022934171828190588601545899028814 0.5877852522924731291687059546390727685976524376431459\n",
                     // h = sqrt(-sqrt(5.) * 2. + 10.)/4.

                     "L\n"
                     "2 0\n"
                     "P\n"
                     "0.6180339887498948482045868343656381177203091798057628 0\n"
                     "S\n"
                     "0 3 0 1\n"
                     "1 1 2 3\n"
                     "\n"
                     "L\n"
                     "0 1\n"
                     "0 2\n"
                     "P\n"
                     "0.6180339887498948482045868343656381177203091798057628 0\n"
                     "0.6180339887498948482045868343656381177203091798057628 1\n"
                     "S\n"
                     "0 4 2 3\n"
                     "1 3 0 4\n"
                     "1 1 2 3\n"
                     "\n"
                     "D\n"
                     "0 0 1 0 0.5 1.5388417685876267012851452880184549120033510717688962\n" // h = sqrt(2.*sqrt(5.)+5.)/2.
                     "0 0 1.6180339887498948482045868343656381177203091798057628 0 0.8090169943749474241022934171828190588601545899028814 0.5877852522924731291687059546390727685976524376431459\n"};
                     // h = sqrt(-sqrt(5.) * 2. + 10.)/4.

QString carre =  "L\n"
                 "0 1\n"
                 "1 2\n"
                 "2 3\n"
                 "3 0\n"
                 "0 2\n"
                 "P\n"
                 ".5 0\n"
                 ".5 1\n"
                 ".5 2\n"
                 ".5 3\n"
                 ".5 4\n"
                 "S\n"
                 "%1 0 4 8 7\n"
                 "%2 4 1 5 8\n"
                 "%3 8 5 2 6\n"
                 "%4 7 8 6 3\n"
                 "\n"
                 "L\n"
                 "0 1\n"
                 "1 2\n"
                 "2 3\n"
                 "3 0\n"
                 "0 2\n"
                 "P\n"
                 ".5 0\n"
                 ".5 1\n"
                 ".5 2\n"
                 ".5 3\n"
                 ".5 4\n"
                 "S\n"
                 "%5 0 4 8 7\n"
                 "%6 4 1 5 8\n"
                 "%7 8 5 2 6\n"
                 "%8 7 8 6 3\n"
                 "\n"
                 "D\n"
                 "0 0 1 0 1 1 0 1\n"
                 "0 0 1 0 1 1 0 1\n";

QString triangle = "L\n"
                   "0 1\n"
                   "1 2\n"
                   "2 0\n"
                   "P\n"
                   ".5 0\n"
                   ".5 1\n"
                   ".5 2\n"
                   "S\n"
                   "%1 0 3 5\n"
                   "%2 3 1 4\n"
                   "%3 4 5 2\n"
                   "%4 3 4 5\n"
                   "\n"
                   "L\n"
                   "0 1\n"
                   "1 2\n"
                   "2 0\n"
                   "P\n"
                   ".5 0\n"
                   ".5 1\n"
                   ".5 2\n"
                   "S\n"
                   "%5 0 3 5\n"
                   "%6 3 1 4\n"
                   "%7 5 4 2\n"
                   "%8 3 4 5\n"
                   "\n"
                   "D\n"
                   "0 0 1 0 0.5 0.8660254037844386467637231707529361834714026269051903\n"
                   "0 0 1 0 0.5 0.8660254037844386467637231707529361834714026269051903\n";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene_ = new QGraphicsScene(this);
    scene_->setItemIndexMethod(QGraphicsScene::NoIndex);
    circle_.setRect(0.25, 0.25, 0.5, 0.5);

    int grayBlue = qGray(0,0,255);
    int grayYellow = qGray(255,255,0);

    pen_ = QPen(Qt::black);
    pen_.setWidthF(0);
    pen_.setCosmetic(true);
    connect(ui->pen_width, (void (QDoubleSpinBox::*)(double))&QDoubleSpinBox::valueChanged, [this](double width){pen_.setWidthF(width); draw(); });

    brushes_[0][0].setColor(Qt::blue);
    brushes_[0][1].setColor(QColor(grayBlue, grayBlue, grayBlue));
    brushes_[1][0].setColor(Qt::yellow);
    brushes_[1][1].setColor(QColor(grayYellow, grayYellow, grayYellow));

    regenerate();
    ui->piece_1->scale(pow(1.15, 37.), -pow(1.15, 37.));
    ui->piece_2->scale(pow(1.15, 37.), -pow(1.15, 37.));
    ui->graphicsView->scale(pow(1.15, 40.), -pow(1.15, 40.));
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->circle->toggle(); ui->circle->toggle(); //Hide circle parameters
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePensAndBrushes()
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            brushes_[i][j].setStyle(static_cast<Qt::BrushStyle>(ui->filling_type->value()));
            brushes_[i][j].setMatrix(ui->graphicsView->matrix().inverted());
        }
    }
}

void MainWindow::regenerate()
{
    surfaces_.clear();

    double x = 0, y = 0, height = 1, width = 1;
    if (ui->tiling->currentIndex() == 0)
    {
        ui->start->setMaximum(1);
        ui->model->setMaximum(255);
        surfaces_.push_back(Surface(ui->start->value(),
                                    std::make_shared<Properties>(carre.arg(BITSTOQSTRING(ui->model->value())))));
    }
    if (ui->tiling->currentIndex() == 1)
    {
        ui->start->setMaximum(1);
        ui->model->setMaximum(255);
        surfaces_.push_back(Surface(ui->start->value(),
                                    std::make_shared<Properties>(triangle.arg(BITSTOQSTRING(ui->model->value())))));
    }
    if (ui->tiling->currentIndex() == 2)
    {
        ui->start->setMaximum(5);
        ui->model->setMaximum(1);
        auto properties = std::make_shared<Properties>(penrose[ui->model->value()]);
        switch (ui->start->value())
        {
        case 0:
        case 1:
            if (!ui->start->value())
            {
                height = sqrt(2.*sqrt(5.)+5.)/2.;
                width = 1.;
            }
            else
            {
                height = sqrt(-sqrt(5.) * 2. + 10.)/4.;
                width = (1.+sqrt(5.))/2.;
            }

            surfaces_.push_back(Surface(ui->start->value(), properties));
            break;
        case 2:
            height = PHI;
            width = cos(M_PI/10.)*2.;
            x = -width/2;
            surfaces_.push_back(Surface(QPolygonF({QPointF(0, 0), QPointF(cos(M_PI/10.), sin(M_PI/10.)), QPointF(0, PHI)}),
                                        0,
                                        properties));
            surfaces_.push_back(Surface(QPolygonF({QPointF(0, 0), QPointF(cos(9.*M_PI/10.), sin(9.*M_PI/10.)), QPointF(0, PHI)}),
                                        0,
                                        properties));
            break;
        case 3:
            width = cos(M_PI/10.)*2.;
            height = 1 + sin(M_PI/10.);
            x = -width/2;
            y = 1 - height;
            surfaces_.push_back(Surface(QPolygonF({QPointF(0, 1), QPointF(cos(-M_PI/10.), sin(-M_PI/10.)), QPointF(0, 0)}),
                                        1,
                                        properties));
            surfaces_.push_back(Surface(QPolygonF({QPointF(0, 1), QPointF(cos(-9.*M_PI/10.), sin(-9.*M_PI/10.)), QPointF(0, 0)}),
                                        1,
                                        properties));
            break;
        case 4:
            {
                height = 2*PHI;
                width = sqrt(2.*sqrt(5.)+5.);
                x = -width/2;
                y = -height/2;

                QPointF A(0,0);
                QPointF B(0, PHI);

                for (unsigned int i = 0; i < 10; i++)
                {
                    QPointF C = QTransform().rotate(36*(i+1)).map(QPointF(0, PHI));
                    if (i % 2 == 0)
                        surfaces_.push_back(Surface(QPolygonF({B, C, A}), 0, properties));
                    else
                        surfaces_.push_back(Surface(QPolygonF({C, B, A}), 0, properties));
                    B = C;
                }
            }
            break;
        case 5:
            {
                height = 2*PHI;
                width = 2*PHI;
                x = -width/2;
                y = -height/2;

                QPointF B(0,0);
                QPointF C(0, PHI);
                QTransform transform;
                transform.rotate(36.);
                for (unsigned int i = 0; i < 5; i++)
                {
                    QPointF A = B + transform.map(getVector(B, C));
                    surfaces_.push_back(Surface(QPolygonF({B, C, A}), 1, properties));
                    C = B + transform.map(getVector(B, A, PHI));
                    surfaces_.push_back(Surface(QPolygonF({B, C, A}), 1, properties));
                }
            }
            break;
        }
    }

    scene_->setSceneRect(x - 0.1, y - 0.1, width + 0.2, height + 0.2);
    ui->graphicsView->setSceneRect(scene_->sceneRect());
    ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);

    draw();

    if(ui->piece_1->scene())
        delete ui->piece_1->scene();
    if(ui->piece_2->scene())
        delete ui->piece_2->scene();

    ui->piece_1->setScene(new QGraphicsScene(this));
    ui->piece_2->setScene(new QGraphicsScene(this));
    for (std::pair<QPolygonF, int> &polygon : Surface(0, surfaces_.front().properties()).getPolygons(1))
        ui->piece_1->scene()->addPolygon(polygon.first, pen_, QBrush(!polygon.second ? Qt::blue : Qt::yellow));
    for (std::pair<QPolygonF, int> &polygon : Surface(1, surfaces_.front().properties()).getPolygons(1))
        ui->piece_2->scene()->addPolygon(polygon.first, pen_, QBrush(!polygon.second ? Qt::blue : Qt::yellow));
}

void MainWindow::draw()
{
    updatePensAndBrushes();
    if (!ui->all_surfaces->isChecked() && ui->circle->isChecked())
    {
        ui->circle->toggle();
        return;
    }

    ui->graphicsView->setUpdatesEnabled(false);
    scene_->clear();

    QPainterPath circle;
    circle.arcMoveTo(circle_, 360);
    circle.arcTo(circle_, 0, 360);

    unsigned long count[2] = {0};
    unsigned long circle_count[2] = {0};

    for (Surface &surface : surfaces_)
    {
        if (!ui->all_surfaces->isChecked())
            scene_->addPolygon(surface.polygon(), pen_, brushes_[surface.type()][0]);

        for (std::pair<QPolygonF, int> &polygon : surface.getPolygons(ui->level->value(), ui->all_surfaces->isChecked() ? -1 : surface.type()))
        {
            bool color = false;
            if (ui->circle->isChecked())
            {
                if (ui->entire_triangles->isChecked() &&
                    circle.contains(polygon.first[0]) && circle.contains(polygon.first[1]) && circle.contains(polygon.first[2]))
                    color = true;
                else if (!ui->entire_triangles->isChecked() &&
                         (circle.contains(polygon.first[0]) || circle.contains(polygon.first[1]) || circle.contains(polygon.first[2])))
                    color = true;
            }
            else
                color = true;

            count[polygon.second]++;
            if (color)
                circle_count[polygon.second]++;

            scene_->addPolygon(polygon.first, pen_, brushes_[polygon.second][!color]);
        }
    }

    if (!ui->all_surfaces->isChecked())
        for (Surface &surface : surfaces_)
            count[surface.type()] += surface.getPolygons(ui->level->value(), !surface.type()).size();

    ui->first_count->setText(QString::number(count[0]));
    ui->second_count->setText(QString::number(count[1]));
    ui->total->setText(QString::number(count[0] + count[1]));
    ui->ratio->setText(QString::number((double)qMax(count[0], count[1])/qMin(count[0], count[1]), 'g', 10));
    if (ui->circle->isChecked())
    {
        scene_->addPath(circle, pen_);
        ui->circle_first_count->setText(QString::number(circle_count[0]));
        ui->circle_second_count->setText(QString::number(circle_count[1]));
        ui->circle_total->setText(QString::number(circle_count[0] + circle_count[1]));
        ui->circle_ratio->setText(QString::number((double)qMax(circle_count[0], circle_count[1])/qMin(circle_count[0], circle_count[1]), 'g', 10));
    }
    ui->graphicsView->setUpdatesEnabled(true);
}

void MainWindow::save()
{
    QString filename(QFileDialog::getSaveFileName(this, "Enregistrer le pavage sous…", QString(), "Images (*.png *.svg)"));
    if (filename.isEmpty())
        return;

    QImage image((scene_->sceneRect().size()*std::abs(ui->graphicsView->transform().m11())).toSize(), QImage::Format_RGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    scene_->render(&painter);
    image.transformed(QTransform().scale(1, -1)).save(filename);
}

bool MainWindow::eventFilter(QObject *object, QEvent *e)
{
    if (object == ui->graphicsView->viewport())
    {
        if (e->type() == QEvent::Wheel)
        {
            zoom(reinterpret_cast<QWheelEvent*>(e));
            e->accept();
            return true;
        }
        else if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseMove)
        {
            QMouseEvent *event = reinterpret_cast<QMouseEvent*>(e);
            if (event->buttons() & Qt::LeftButton && event->modifiers().testFlag(Qt::ControlModifier))
            {
                moveCircle(event);
                e->accept();
                return true;
            }
            else
                return false;
        }
    }
    return false;
}

void MainWindow::zoom(QWheelEvent* event)
{
    double scaleFactor = 1.15;
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        QPointF center = circle_.center();
        if(event->delta() > 0)
            circle_.setSize(circle_.size()*scaleFactor);
        else
            circle_.setSize(circle_.size()/scaleFactor);
        circle_.moveCenter(center);
        draw();
    }
    else
    {
        if(event->delta() > 0)
            ui->graphicsView->scale(scaleFactor, scaleFactor);
        else
            ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);

        ui->zoom->setText(QString::number(std::abs(ui->graphicsView->matrix().m11())));
        //draw();
    }
}

void MainWindow::moveCircle(QMouseEvent *event)
{
    static QPoint lastMousePos = event->pos();
    static unsigned long lastTimestamp = 0;
    static unsigned int renderDuration = 0;

    if (event->timestamp() - lastTimestamp < renderDuration)
        return;

    QTime drawingTime;
    drawingTime.restart();

    if (event->type() == QEvent::MouseButtonPress)
    {
        circle_.moveCenter(ui->graphicsView->mapToScene(event->pos()));
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QPointF move = ui->graphicsView->mapToScene(event->pos()) - ui->graphicsView->mapToScene(lastMousePos);
        circle_.moveTopLeft(circle_.topLeft() + move);
    }
    lastMousePos = event->pos();

    draw();
    qDebug(QString("%1 %2").arg(ui->circle_first_count->text(), ui->circle_second_count->text()).toLatin1());
    renderDuration = drawingTime.elapsed();
    lastTimestamp = event->timestamp();
}

void MainWindow::clockwiseRotation()
{
    ui->graphicsView->rotate(72);
}

void MainWindow::counterclockwiseRotation()
{
    ui->graphicsView->rotate(-72);
}

void MainWindow::about()
{
    QMessageBox::about(this, "À propos de Tiling Generator", "<strong>Tiling Generator</strong><br/>Générateur de pavages.<br/>Copyright © 2014-2015 Paul DU");
}
