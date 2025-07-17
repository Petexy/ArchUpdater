#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // Clear any previous output in the QTextEdit (using its default objectName 'textEdit')
    ui->textEdit->clear();
    ui->textEdit->append("Executing command..."); // Indicate that the command is starting

    // ... (rest of your command setup remains the same) ...
    QString command = "run0 pacman -Syu --noconfirm && flatpak update --assumeyes";
    QString program = "bash";
    QStringList arguments;
    arguments << "-c" << command;

    QProcess *process = new QProcess(this);

    // Connect to readyReadStandardOutput to capture normal output as it becomes available
    connect(process, &QProcess::readyReadStandardOutput, [=]() {
        QString output = process->readAllStandardOutput();
        ui->textEdit->append(output.trimmed()); // Append to ui->textEdit
    });

    // Connect to readyReadStandardError to capture error output
    connect(process, &QProcess::readyReadStandardError, [=]() {
        QString errorOutput = process->readAllStandardError();
        ui->textEdit->append("<font color=\"red\">ERROR: " + errorOutput.trimmed() + "</font>"); // Append to ui->textEdit
    });

    // Connect to the finished signal
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    ui->textEdit->append("\n--- Command Finished Successfully (Exit Code: 0) ---");
                } else {
                    ui->textEdit->append(QString("\n<font color=\"orange\">--- Command Failed (Exit Code: %1, Status: %2) ---</font>")
                                             .arg(exitCode)
                                             .arg(exitStatus == QProcess::CrashExit ? "Crashed" : "Error"));
                }
                process->deleteLater();
            });

    process->start(program, arguments);

    if (!process->waitForStarted(-1)) {
        qDebug() << "Failed to start process:" << process->errorString();
        ui->textEdit->append("<font color=\"darkred\"><b>Failed to start command:</b> " + process->errorString() + "</font>");
        process->deleteLater();
    } else {
        qDebug() << "Process started successfully.";
    }
}

