#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "triangle.h"
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

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++)
            pens_[i][j].setCosmetic(true);

    pens_[1][0].setColor(Qt::blue);
    pens_[1][1].setColor(QColor(grayBlue, grayBlue, grayBlue));
    pens_[2][0].setColor(Qt::yellow);
    pens_[2][1].setColor(QColor(grayYellow, grayYellow, grayYellow));

    brushes_[0][0].setColor(Qt::blue);
    brushes_[0][1].setColor(QColor(grayBlue, grayBlue, grayBlue));
    brushes_[1][0].setColor(Qt::yellow);
    brushes_[1][1].setColor(QColor(grayYellow, grayYellow, grayYellow));

    regenerate();
    ui->graphicsView->rotate(180);
    ui->graphicsView->scale(pow(1.15, 40.), pow(1.15, 40.));
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->viewport()->installEventFilter(this);
    ui->circle->toggle(); ui->circle->toggle(); //Hide circle parameters
}

MainWindow::~MainWindow()
{
    delete scene_;
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
    triangles_.clear();

    if (ui->acute_double_cut->isChecked())
        ui->acute_pattern->setMaximum(4);
    else
        ui->acute_pattern->setMaximum(2);

    if (ui->obtuse_double_cut->isChecked())
        ui->obtuse_pattern->setMaximum(4);
    else
        ui->obtuse_pattern->setMaximum(2);

    QRectF scene;

    if (ui->polygon->currentIndex() <= 1)
    {
        QPolygonF triangle;
        double height = 0, width = 0;
        if (ui->polygon->currentIndex() == 0)
        {
            height = sqrt(2.*sqrt(5.)+5.)/2.;
            width = 1.;
        }
        else
        {
            height = sqrt(-sqrt(5.) * 2. + 10.)/4.;
            width = (1.+sqrt(5.))/2.;
        }

        triangle << QPointF(width/2, height) << QPointF(0,0) << QPointF(width, 0);
        triangles_.push_back(Triangle(!ui->polygon->currentIndex(), triangle,
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));
        scene.setCoords(-0.1, -0.1, width + 0.1, height + 0.1);
    }
    else if (ui->polygon->currentIndex() == 2)
    {
        triangles_.push_back(Triangle(true, QPointF(0,PHI), QPointF(0,0), QPointF(cos(M_PI/10.), sin(M_PI/10.)),
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));

        triangles_.push_back(Triangle(true, QPointF(0,PHI), QPointF(0,0), QPointF(cos(9*M_PI/10.), sin(9*M_PI/10.)),
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));

        double width = cos(M_PI/10.)*2.;
        double height = PHI;
        scene.setRect(-width/2 - 0.1, -0.1, width + 0.2, height + 0.2);
    }
    else if (ui->polygon->currentIndex() == 3)
    {
        triangles_.push_back(Triangle(false, QPointF(0,0), QPointF(0,1), QPointF(cos(-M_PI/10.), sin(-M_PI/10.)),
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));

        triangles_.push_back(Triangle(false, QPointF(0,0), QPointF(0,1), QPointF(cos(-9*M_PI/10.), sin(-9*M_PI/10.)),
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));

        double width = cos(M_PI/10.)*2.;
        double height = 1 + sin(M_PI/10.);
        scene.setRect(-width/2 - 0.1, -sin(M_PI/10.) - 0.1, width + 0.2, height + 0.2);

    }
    else if (ui->polygon->currentIndex() == 4)
    {
        QPointF A(0,0);
        QPointF B(0, PHI);
        QTransform transform;
        transform.rotate(36);
        for (unsigned int i = 0; i < 10; i++)
        {
            QPointF C = A + transform.map(B-A);
            triangles_.push_back(Triangle(true, A, B, C,
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));
            if (i % 2 == 0)
                triangles_.back().invert();
            B = C;
        }

        double height = 2*PHI, width = sqrt(2.*sqrt(5.)+5.);
        scene.setRect(-width/2 - 0.1, -height/2 - 0.1, width + 0.2, height + 0.2);
    }
    else
    {
        QPointF B(0,0);
        QPointF C(0, PHI);
        QTransform transform;
        transform.rotate(36);
        for (unsigned int i = 0; i < 5; i++)
        {
            QPointF A = B + transform.map(getVector(B, C, 1));
            triangles_.push_back(Triangle(false, A, B, C,
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));
            C = B + transform.map(getVector(B, A, PHI));
            triangles_.push_back(Triangle(false, A, B, C,
                                 ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1,
                                 ui->acute_double_cut->isChecked(), ui->obtuse_double_cut->isChecked()));
        }

        double height = 2*PHI, width = 2*PHI;
        scene.setRect(-width/2 - 0.1, -height/2, width + 0.2, height + 0.2);
    }

    scene_->setSceneRect(scene);
    ui->graphicsView->setSceneRect(scene);
    ui->graphicsView->fitInView(scene, Qt::KeepAspectRatio);
    draw();
}

void MainWindow::draw()
{
    updatePensAndBrushes();

    ui->graphicsView->setUpdatesEnabled(false);
    scene_->clear();

    QPainterPath circle;
    circle.arcMoveTo(circle_, 360);
    circle.arcTo(circle_, 0, 360);

    unsigned long count[2] = {0};
    unsigned long circle_count[2] = {0};

    for (Triangle &triangle : triangles_)
    {
        for (std::pair<QPolygonF, bool> &polygon : triangle.getTriangles(ui->level->value()))
        {
            bool intersect = false;
            if (ui->circle->isChecked())
            {
                if (ui->entire_triangles->isChecked() &&
                    circle.contains(polygon.first[0]) && circle.contains(polygon.first[1]) && circle.contains(polygon.first[2]))
                    intersect = true;
                else if (!ui->entire_triangles->isChecked() &&
                         (circle.contains(polygon.first[0]) || circle.contains(polygon.first[1]) || circle.contains(polygon.first[2])))
                    intersect = true;
            }
            else
                intersect = true;

            count[!polygon.second]++;
            if (!ui->circle->isChecked() || intersect)
                circle_count[!polygon.second]++;

            if (ui->line_color->isChecked())
                scene_->addPolygon(polygon.first, pens_[!polygon.second + 1][!intersect], brushes_[!polygon.second][!intersect]);
            else
                scene_->addPolygon(polygon.first, pens_[0][0], brushes_[!polygon.second][!intersect]);
        }
    }

    ui->acute_count->setText(QString::number(count[0]));
    ui->obtuse_count->setText(QString::number(count[1]));
    ui->total->setText(QString::number(count[0] + count[1]));
    ui->ratio->setText(QString::number((double)qMax(count[0], count[1])/qMin(count[0], count[1]), 'g', 15));
    if (ui->circle->isChecked())
    {
        scene_->addPath(circle, pens_[0][0]);
        ui->circle_acute_count->setText(QString::number(circle_count[0]));
        ui->circle_obtuse_count->setText(QString::number(circle_count[1]));
        ui->circle_total->setText(QString::number(circle_count[0] + circle_count[1]));
        ui->circle_ratio->setText(QString::number((double)qMax(circle_count[0], circle_count[1])/qMin(circle_count[0], circle_count[1]), 'g', 15));
    }
    ui->graphicsView->setUpdatesEnabled(true);
}

void MainWindow::save()
{
    QString filename(QFileDialog::getSaveFileName(this, "Enregistrer le pavage sous…", QString(), "Images (*.png *.svg)"));
    if (filename.isEmpty())
        return;

    QImage image((scene_->sceneRect().size()*abs(ui->graphicsView->transform().m11())).toSize(), QImage::Format_RGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    scene_->render(&painter);
    image.transformed(QTransform().rotate(180)).save(filename);
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

        ui->zoom->setText(QString::number(abs(ui->graphicsView->matrix().m11())));
    }
}

void MainWindow::moveCircle(QMouseEvent *event)
{
    static QPoint lastMousePos = event->pos();
    static long lastTimestamp = 0;
    static int renderDuration = 0;

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
    renderDuration = drawingTime.elapsed();
    lastTimestamp = event->timestamp();
}

void MainWindow::clockwiseRotation()
{
    ui->graphicsView->rotate(36);
}

void MainWindow::counterclockwiseRotation()
{
    ui->graphicsView->rotate(-36);
}

void MainWindow::about()
{
    QMessageBox::about(this, "À propos de Penrose Generator", "<strong>Penrose Generator</strong><br/>Générateur de pavages de Penrose à l'aide de triangles d'or.<br/>Copyright © 2014 Paul DU");
}
