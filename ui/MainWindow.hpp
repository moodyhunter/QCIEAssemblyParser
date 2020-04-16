#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_runBtn_clicked();

    void on_stepBtn_clicked();

    void on_stopBtn_clicked();

    void on_assmTxt_textChanged();

    void on_setMemBtn_clicked();

    void on_binOutputRad_clicked();

    void on_decOutputRad_clicked();

    void on_hexOutputRad_clicked();

    void on_asciiOutputRad_clicked();

  private:
    void ClearData();
    void PrintMemory(const QString &label, const QStringList &changedMem);
    Ui::MainWindow *ui;
    //
    int CIR = 0;
    int execCycles = 0;
};
