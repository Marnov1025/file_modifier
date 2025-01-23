#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <qfilesystemmodel.h>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QRegularExpressionValidator *valid = new QRegularExpressionValidator(QRegularExpression("^[0-9A-Fa-f]{16}$"), ui->keyTextBox);
    ui->keyTextBox->setValidator(valid);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::execute);
    connect(ui->run_button, &QPushButton::clicked, this, &MainWindow::timer_check);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timer_check() {
    if (ui->timer_mode->isChecked()
        && ui->timerSlider->value() > 0) {
        ui->timerBox->setEnabled(true);
        timer->setInterval(ui->timerSlider->value() * 1000);
        timer->start();

    }
    else {
        execute();
    }
}

void MainWindow::execute() {

    if(ui->once_mode->isChecked()) {
        timer->stop();
    }

    if(ui->listWidget->count() <= 0) {
        qDebug("Не найдено файлов под указанной маской.");
    }

    if(ui->file_mask_textbox->toPlainText().isEmpty()
        || ui->keyTextBox->text().isEmpty()
        || ui->savePathTextBox->toPlainText().isEmpty()) {
        qDebug("Одно из обязательных для заполнения полей пусто.");
        return;
    }

    if(ui->keyTextBox->text().size() != 16) {
        QMessageBox::warning(this, "Неправильный формат ключа!", "Ваш ключен должен содержать 16 символов.");
        return;
    }
    QByteArray byteKey = QByteArray::fromHex(ui->keyTextBox->text().toUtf8());
    QByteArray result;

    for (int i = 0; i < ui->listWidget->count(); i++) {
        QString path = "input/" + ui->listWidget->item(i)->text();
        QFile file(path);
        QFileInfo fileInfo(path);

        if(!file.open(QIODevice::ReadOnly)){
            qDebug("Не удается открыть файл.");
            return;
        }

        if(!file.isReadable()) {
            qDebug("Файл не закрыт.");
        }

        QByteArray byteFile = file.readAll();
        file.close();

        for(int i = 0; i < byteFile.size(); i++) {
            result.append(byteFile[i] ^ byteKey[i % byteKey.size()]);
        }

        QString finalFilePath;
        if(ui->modify_mode->isChecked()) {

            int a = 1;

            finalFilePath = QString("%1/%2_%3.%4").arg(ui->savePathTextBox->toPlainText(), fileInfo.baseName(), QString::number(a), fileInfo.suffix());
            while (QFile::exists(finalFilePath)) {
                a++;
                finalFilePath = QString("%1/%2_%3.%4").arg(ui->savePathTextBox->toPlainText(), fileInfo.baseName(), QString::number(a), fileInfo.suffix());
            }
            QFile finalFile(finalFilePath);
            finalFile.open(QIODevice::WriteOnly);
            finalFile.write(result);
            finalFile.close();
        }

        else if(ui->rewrite_mode->isChecked()) {

            finalFilePath = ui->savePathTextBox->toPlainText() + "/" + ui->listWidget->item(i)->text();
            QFile finalFile(finalFilePath);
            finalFile.open(QIODevice::WriteOnly);
            finalFile.write(result);
            finalFile.close();
        }

        if(ui->delete_mode->isChecked()) {
            if(!QFile::exists(path)) {
                qDebug("Нет файлов!");
            }
            QFile::remove(path);
        }
    }
}

void MainWindow::on_file_mask_textbox_textChanged()
{

    QString mask = ui->file_mask_textbox->toPlainText();
    qDebug() << mask;

    QDir dir(QFileInfo(mask).absolutePath());
    qDebug() << dir;

    QStringList fileList;
    fileList.clear();
    fileList = dir.entryList({QFileInfo(mask).fileName()}, QDir::Files);
    qDebug() << fileList;

    if (!fileList.empty()) {
        ui->listWidget->clear();
        for(const QString &fileInfo : fileList) {
            ui->listWidget->addItem(fileInfo);
            ui->listWidget->update();
        }
    }

    else {
        ui->listWidget->clear();
        qDebug("Нет файлов!");
    }
}

void MainWindow::on_timerSlider_valueChanged(int value)
{
    ui->gapLabel->setText(QString::number(ui->timerSlider->value()));
}

