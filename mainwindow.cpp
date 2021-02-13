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

    auto filter = new Filter;
    ui->canvas->installEventFilter(filter);

    manager = SceneManager(width, height, Qt::black, ui->canvas->scene());

    QWidget::setEnabled(true);
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

const int w = 17, d = 32, a = 30, s = 31, move_dist= 1;
void MainWindow::keyPressEvent(QKeyEvent *event){
    // w = 17 d = 32 a = 30 s = 31

    switch (event->nativeScanCode()) {
    case w:
        manager.moveCamera(shift_z, move_dist);
        break;
    case d:
        manager.moveCamera(shift_x, move_dist);
        break;
    case a:
        manager.moveCamera(shift_x, -move_dist);
        break;
    case s:
        manager.moveCamera(shift_z, -move_dist);
        break;
    }
}

bool Filter::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
//        switch (keyEvent->nativeScanCode()) {
//        case w:
//            manager.moveCamera(shift_z, move_dist);
//            break;
//        case d:
//            manager.moveCamera(shift_x, move_dist);
//            break;
//        case a:
//            manager.moveCamera(shift_x, -move_dist);
//            break;
//        case s:
//            manager.moveCamera(shift_z, -move_dist);
//            break;
//        }
        return true;
    }else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
