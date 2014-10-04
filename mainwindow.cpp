#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "triangle.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QPolygonF>
#include <QPen>
#include <QMessageBox>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene_ = new QGraphicsScene(this);
    scene_->setItemIndexMethod(QGraphicsScene::NoIndex);
    regenerate();
    ui->graphicsView->rotate(180);
    ui->graphicsView->scale(pow(1.15, 40.), pow(1.15, 40.));
    ui->graphicsView->setSceneRect(-0.1, -0.1, 1.2, (sqrt(2*sqrt(5)+5)/2)+0.2);
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->viewport()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete triangle_;
    delete scene_;
    delete ui;
}

void MainWindow::draw()
{
    QPen blue;
    blue.setCosmetic(true);
    QPen yellow;
    yellow.setCosmetic(true);
    if (!ui->draw_lines->isChecked())
    {
        blue.setColor(Qt::blue);
        yellow.setColor(Qt::yellow);
    }
    scene_->clear();
    for (std::pair<QPolygonF, bool> &polygon : triangle_->getTriangles(ui->level->value()))
        if (polygon.second) //Is acute
            scene_->addPolygon(polygon.first, blue, Qt::blue);
        else
            scene_->addPolygon(polygon.first, yellow, Qt::yellow);
}

void MainWindow::regenerate()
{
    if (ui->double_cut->isChecked())
        ui->acute_pattern->setMaximum(4);
    else
        ui->acute_pattern->setMaximum(2);
    triangle_ = new Triangle(true, QPointF(0.5, sqrt(2*sqrt(5)+5)/2), QPointF(0,0), QPointF(1,0), ui->acute_pattern->value()-1, ui->obtuse_pattern->value()-1, ui->double_cut->isChecked());
    draw();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->graphicsView->viewport() && event->type() == QEvent::Wheel) {
        wheelEvent(reinterpret_cast<QWheelEvent*>(event));
        return true;
    }
    return false;
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    double scaleFactor = 1.15;
    if(event->delta() > 0)
        ui->graphicsView->scale(scaleFactor, scaleFactor);
    else
        ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    event->accept();
}

void MainWindow::a_propos()
{
    QMessageBox::about(this, "À propos de Penrose Generator", "Penrose Generator est un programme générant des pavages de Penrose. Copyright © 2014 Paul DU");
}
