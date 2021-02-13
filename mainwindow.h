#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QKeyEvent>
#include "scene_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    SceneManager manager;
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_render_button_clicked();

    void on_rotate_x_spin_valueChanged(double arg1);

    void on_rotate_y_spin_valueChanged(double arg1);

    void on_rotate_z_spin_valueChanged(double arg1);

    void on_offset_x_spin_valueChanged(double arg1);

    void on_offset_y_spin_valueChanged(double arg1);

    void on_offset_z_spin_valueChanged(double arg1);

    void on_scale_z_spin_valueChanged(double arg1);

    void on_scale_x_spin_valueChanged(double arg1);

    void on_scale_y_spin_valueChanged(double arg1);


private:
    Ui::MainWindow *ui;
};

class Filter: public QObject{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // MAINWINDOW_H
