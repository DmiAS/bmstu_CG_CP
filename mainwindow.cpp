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

    auto f = [&](trans_type t, float dist){
        manager.moveCamera(t, dist);
    };

    auto filter = new Filter(f);
    ui->canvas->installEventFilter(filter);

    manager = SceneManager(width, height, Qt::black, ui->canvas->scene());

    const QStringList textures = {
        "Куб",
        "Сфера",
        "Пирамида",
        "Цилиндр",
        "Конус"
    };

    ui->objects_list->addItems(textures);

    auto stringList = new QStringList();
    model = new QStringListModel(*stringList);
//    model->event

    ui->scene_list->setModel(model);
    ui->scene_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->scene_list->setSelectionMode(QAbstractItemView::Selec);

    connect(ui->scene_list, SIGNAL(clicked(QModelIndex)), this, SLOT(fetch(QModelIndex)));

    manager.init();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fetch(QModelIndex index){
    if (!index.isValid()) return;
    auto text = model->index(index.row()).data(Qt::DisplayRole).toString();
    manager.setCurrentModel(text_uid.at(text));
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

const int w = 17, d = 32, a = 30, s = 31;
float move_dist= 0.5;
const int pUp = 328, pDn = 336, home = 331, end = 333, rot_angle = 15;

bool Filter::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->nativeScanCode()) {
        case w:
            f(shift_z, move_dist);
            break;
        case d:
            f(shift_x, -move_dist);
            break;
        case a:
            f(shift_x, move_dist);
            break;
        case s:
            f(shift_z, -move_dist);
            break;
        case pUp:
            f(rot_x, rot_angle);
            break;
        case pDn:
            f(rot_x, -rot_angle);
            break;
        case home:
            f(rot_y, -rot_angle);
            break;
        case end:
            f(rot_y, rot_angle);
            break;
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::on_add_object_button_clicked()
{
    auto text = ui->objects_list->currentText();
    auto updated_text = text;
    if (!name_amount.count(text))
        name_amount.insert({text, 0});
    else{
        auto val = ++name_amount.at(text);
        updated_text += QString("%1").arg(val);
    }
    uint32_t uid = 0;
    manager.uploadModel(text.toStdString(), uid);

    if (uid){
        model->insertRow(model->rowCount());
        QModelIndex index = model->index(model->rowCount()-1);
        model->setData(index, updated_text);
        text_uid.insert({updated_text, uid});
    }
    return;

}

void MainWindow::on_delete_object_button_clicked()
{
    auto index = ui->scene_list->currentIndex();
    if (!index.isValid()) return;
    model->removeRow(index.row());
    manager.removeModel();
}
