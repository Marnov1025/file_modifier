#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer;

private slots:
    void on_file_mask_textbox_textChanged();

    void on_timerSlider_valueChanged(int value);

    void timer_check();

    void update_timer();

    void execute();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
