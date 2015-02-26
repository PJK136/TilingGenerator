#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updatePensAndBrushes();
    void regenerate();
    void draw();
    void save();
    void about();
    void clockwiseRotation();
    void counterclockwiseRotation();

protected:
    void zoom(QWheelEvent *);
    void moveCircle(QMouseEvent *);
    bool eventFilter(QObject *object, QEvent *e);

private:
    Ui::MainWindow *ui;
    class QGraphicsScene *scene_;
    std::list<class Surface> surfaces_;
    QPen pens_[3][2];
    QBrush brushes_[2][2];
    QRectF circle_;
};

#endif // MAINWINDOW_H
