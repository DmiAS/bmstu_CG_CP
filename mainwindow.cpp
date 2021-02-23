#include "mainwindow.h"
#include "ui_mainwindow.h"

UI_data::UI_data(){
    img.load("C:\\raster\\ui_mode\\bricks.jpg");
}

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
        "Конус",
        "Плоскость"
    };

    const QStringList lights = {"Точечный источник", "Направленный"};

    ui->objects_list->addItems(textures);

    ui->add_light_list->addItems(lights);

    auto stringList = new QStringList();
    model = new QStringListModel(*stringList);
//    model->event

    ui->scene_list->setModel(model);
    ui->scene_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->scene_list->setSelectionMode(QAbstractItemView::Selec);

    connect(ui->scene_list, SIGNAL(clicked(QModelIndex)), this, SLOT(fetch(QModelIndex)));

    ui->color_preview->setScene(new QGraphicsScene);
    ui->texture_img->setScene(new QGraphicsScene(0, 0, ui->texture_img->width(), ui->texture_img->height()));
    ui->texture_img->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->texture_img->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hideButtons();

    manager.init();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::lockSignals(bool flag){

    ui->offset_x_spin->blockSignals(flag);
    ui->offset_y_spin->blockSignals(flag);
    ui->offset_z_spin->blockSignals(flag);

    ui->rotate_x_spin->blockSignals(flag);
    ui->rotate_y_spin->blockSignals(flag);
    ui->rotate_z_spin->blockSignals(flag);

    ui->scale_x_spin->blockSignals(flag);
    ui->scale_y_spin->blockSignals(flag);
    ui->scale_z_spin->blockSignals(flag);
}

void MainWindow::fill_data(const UI_data& data){

    lockSignals(true);

    ui->offset_x_spin->setValue(data.shift_x);
    ui->offset_y_spin->setValue(data.shift_y);
    ui->offset_z_spin->setValue(data.shift_z);

    ui->rotate_x_spin->setValue(data.rot_x);
    ui->rotate_y_spin->setValue(data.rot_y);
    ui->rotate_z_spin->setValue(data.rot_z);

    ui->scale_x_spin->setValue(data.scale_x);
    ui->scale_y_spin->setValue(data.scale_y);
    ui->scale_z_spin->setValue(data.scale_z);

    ui->color_flag->setChecked(data.color_flag);
    ui->texture_flag->setChecked(data.texture_flag);

    ui->texture_img->scene()->addPixmap(QPixmap::fromImage(data.img));

    ui->color_preview->scene()->setBackgroundBrush(
                QColor(data.color.x *255.f, data.color.y * 255.f, data.color.z * 255.f));

    // block and lock buttons for adding color and texture
    ui->color_add_button->setDisabled(data.texture_flag);
    ui->add_texture_button->setDisabled(data.color_flag);

    lockSignals(false);

}

void MainWindow::save_data(UI_data& data){

    data.shift_x = ui->offset_x_spin->value();
    data.shift_y = ui->offset_y_spin->value();
    data.shift_z = ui->offset_z_spin->value();

    data.rot_x = ui->rotate_x_spin->value();
    data.rot_y = ui->rotate_y_spin->value();
    data.rot_z = ui->rotate_z_spin->value();

    data.scale_x = ui->scale_x_spin->value();
    data.scale_y = ui->scale_y_spin->value();
    data.scale_z = ui->scale_z_spin->value();

    data.texture_flag = ui->texture_flag->isChecked();
    data.color_flag = ui->color_flag->isChecked();

    auto color = ui->color_preview->scene()->backgroundBrush().color();

    data.color = Vec3f(color.redF(), color.greenF(), color.blueF());

}

void MainWindow::changeHidence(bool flag){
    ui->offset_x_spin->setHidden(flag);
    ui->offset_y_spin->setHidden(flag);
    ui->offset_z_spin->setHidden(flag);

    ui->rotate_x_spin->setHidden(flag);
    ui->rotate_y_spin->setHidden(flag);
    ui->rotate_z_spin->setHidden(flag);

    ui->scale_x_spin->setHidden(flag);
    ui->scale_y_spin->setHidden(flag);
    ui->scale_z_spin->setHidden(flag);

    ui->offset_label->setHidden(flag);
    ui->rotate_label->setHidden(flag);
    ui->scale_label->setHidden(flag);

    ui->x_label->setHidden(flag);
    ui->y_label->setHidden(flag);
    ui->z_label->setHidden(flag);

    ui->texture_img->setHidden(flag);
    ui->texture_flag->setHidden(flag);
    ui->add_texture_button->setHidden(flag);

    ui->color_flag->setHidden(flag);
    ui->color_add_button->setHidden(flag);
    ui->color_preview->setHidden(flag);

    ui->reflection_spin->setHidden(flag);
    ui->refraction_spin->setHidden(flag);
    ui->glitter_spin->setHidden(flag);
    ui->transparency_spin->setHidden(flag);

    ui->glitter_label->setHidden(flag);
    ui->refraction_label->setHidden(flag);
    ui->transparency_label->setHidden(flag);
    ui->reflection_label->setHidden(flag);
}

void MainWindow::showButtons(){
    changeHidence(false);
}

void MainWindow::hideButtons(){
    changeHidence(true);
}

void MainWindow::fetch(QModelIndex index){
    qDebug() << "fetching";
    if (!index.isValid()) return;
    auto text = model->index(index.row()).data(Qt::DisplayRole).toString();
    if (prev_selected == "")
        showButtons();
    else
        save_data(name_data.at(prev_selected));
    manager.setCurrentModel(text_uid.at(text));
    if (name_data.count(text))
        fill_data(name_data.at(text));
    prev_selected = text;
}


void MainWindow::on_render_button_clicked()
{
//    manager.init();
    manager.trace();
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
    manager.rotate(rot_y, -arg1);
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
    qDebug() << "val";
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
    if (!name_data.count(text))
        name_data.insert({text, UI_data{}});
    else{
        auto val = ++name_data.at(text).amount;
        updated_text += QString("%1").arg(val);
        name_data.insert({updated_text, UI_data{}});
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
    prev_selected = "";
    hideButtons();
    manager.removeModel();
}

void MainWindow::on_color_add_button_clicked()
{
    QColor color = QColorDialog::getColor(Qt::gray, this, QStringLiteral("Выберите цвет модели"));
    qDebug() << "picked";
    if (!color.isValid()) return;
    auto cred = QString::number(color.red()).toFloat();
    auto cgreen = QString::number(color.green()).toFloat();
    auto cblue = QString::number(color.blue()).toFloat();

//    ui->color_preview->scene()->setBackgroundBrush(QBrush(QColor(cred, cgreen, cblue)));
    ui->color_preview->scene()->setBackgroundBrush(QColor(cred, cgreen, cblue));

    auto color_f = Vec3f(cred / 255.f, cgreen / 255.f, cblue / 255.f);
    manager.setColor(color_f);
    name_data.at(prev_selected).color = color_f;

}

void MainWindow::on_texture_flag_clicked()
{
    ui->add_texture_button->setDisabled(false);
    auto img = name_data.at(prev_selected).img;
    ui->texture_img->scene()->addPixmap(QPixmap::fromImage(img));
    ui->color_add_button->setDisabled(true);
    manager.setFlagTexture(true, Vec3f{1.f, 1.f, 1.f});
}

void MainWindow::on_color_flag_clicked()
{
    ui->color_add_button->setDisabled(false);
    ui->add_texture_button->setDisabled(true);
    manager.setFlagTexture(false, name_data.at(prev_selected).color);
}

void MainWindow::on_add_texture_button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите текстуру" );
    QImage img;
    qDebug() << img.load(fileName);
    name_data.at(prev_selected).img = img;
    manager.setTexture(img);
}

void MainWindow::on_add_light_button_clicked()
{
    auto text = ui->add_light_list->currentText();
    auto updated_text = text;
    if (!name_data.count(text))
        name_data.insert({text, UI_data{}});
    else{
        auto val = ++name_data.at(text).amount;
        updated_text += QString("%1").arg(val);
        name_data.insert({updated_text, UI_data{}});
    }
    uint32_t uid = 0;
    manager.uploadLight(text.toStdString(), uid);

    if (uid){
        model->insertRow(model->rowCount());
        QModelIndex index = model->index(model->rowCount()-1);
        model->setData(index, updated_text);
        text_uid.insert({updated_text, uid});
    }
    return;
}
