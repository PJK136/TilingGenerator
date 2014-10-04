#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void draw();
    void regenerate();
    void a_propos();

protected:
    void wheelEvent(QWheelEvent *);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::MainWindow *ui;
    class QGraphicsScene *scene_;
    class Triangle *triangle_;
};

#endif // MAINWINDOW_H
