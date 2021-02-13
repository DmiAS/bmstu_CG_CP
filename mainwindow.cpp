#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int width = ui->canvas->width();
    int height = ui->canvas->height();

    ui->canvas->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->canvas->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->canvas->setScene(new QGraphicsScene(0, 0, width, height));

    manager = SceneManager(width, height, Qt::black, ui->canvas->scene());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_render_button_clicked()
{
    manager.init();
}

void MainWindow::on_rotate_x_spin_valueChanged(double arg1)
{
//    static auto prev_value = 0.f;
//    float step = ui->rotate_x_spin->singleStep();
//    if (arg1 < prev_value)
//        step *= -1;
//    prev_value = arg1;
    manager.rotate(rot_x, arg1);
}

void MainWindow::on_rotate_y_spin_valueChanged(double arg1)
{
//    static auto prev_value = 0.f;
//    float step = ui->rotate_y_spin->singleStep();
//    if (arg1 < prev_value)
//        step *= -1;
//    prev_value = arg1;
    manager.rotate(rot_y, arg1);
}

void MainWindow::on_rotate_z_spin_valueChanged(double arg1)
{
//    static auto prev_value = 0.f;
//    float step = ui->rotate_z_spin->singleStep();
//    if (arg1 < prev_value)
//        step *= -1;
//    prev_value = arg1;
    manager.rotate(rot_z, arg1);
}

void MainWindow::on_offset_x_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->offset_x_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.shift(shift_x, step);
}

void MainWindow::on_offset_y_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->offset_y_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.shift(shift_y, step);
}

void MainWindow::on_offset_z_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->offset_z_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.shift(shift_z, step);
}

void MainWindow::on_scale_z_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->scale_z_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.scale(scale_z, step);
}

void MainWindow::on_scale_x_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->scale_x_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.scale(scale_x, step);
}

void MainWindow::on_scale_y_spin_valueChanged(double arg1)
{
    static auto prev_value = 0.f;
    float step = ui->scale_y_spin->singleStep();
    if (arg1 < prev_value)
        step *= -1;
    prev_value = arg1;
    manager.scale(scale_y, step);
}
