#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QKeyEvent>
#include <QStringListModel>
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


private slots:
    void fetch(QModelIndex index);

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


    void on_add_object_button_clicked();

    void on_delete_object_button_clicked();

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    std::map<QString, uint32_t> text_uid;
    std::map<QString, uint32_t> name_amount;
};

class Filter: public QObject{
    Q_OBJECT
public:
    Filter(std::function<void (trans_type, float)> f_): f{f_}{}
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    std::function<void (trans_type, float)> f;
};
#endif // MAINWINDOW_H
