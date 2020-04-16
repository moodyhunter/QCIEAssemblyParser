#include "MainWindow.hpp"

#include "core/CIEAssemRunner.hpp"
#include "core/Highlighter.hpp"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new CIEAsmHighlighter(ui->assmTxt->document());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ClearData()
{
    ui->memoryTable->model()->removeRows(0, ui->memoryTable->rowCount());
    ui->memoryTable->model()->removeColumns(0, ui->memoryTable->columnCount());
    ui->memoryTable->clear();
    CIR = 0;
    execCycles = 0;
}

void MainWindow::on_runBtn_clicked()
{
    on_stopBtn_clicked();
    QString errorMessage;
    ParseAssemblyCode(ui->assmTxt->toPlainText(), &errorMessage);
    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Invalid CIE Assembly Code"), errorMessage);
        return;
    }
    while (CIR < CODE_MODEL.count() && CIR >= 0)
    {
        on_stepBtn_clicked();
    }
    if (CIR < 0)
    {
        QMessageBox::warning(this, tr("Error"), "Stopped executing.");
    }
}

void MainWindow::on_stepBtn_clicked()
{
    QString errorMessage;

    if (CODE_MODEL.isEmpty())
    {
        ParseAssemblyCode(ui->assmTxt->toPlainText(), &errorMessage);
        if (!errorMessage.isEmpty())
        {
            QMessageBox::warning(this, tr("Invalid CIE Assembly Code"), errorMessage);
            return;
        }
    }
    if (CIR >= CODE_MODEL.count())
    {
        QMessageBox::warning(this, "Execution Finished", "Completed stepping.");
        return;
    }
    //
    execCycles++;
    ui->execCyclesLabel->setText(QString::number(execCycles));
    //
    const auto &code = CODE_MODEL.at(CIR);
    QStringList changedMem;
    auto jumpInstruction = ExecuteSingleInstruction(code, &errorMessage, &changedMem);
    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, "Code execution error", errorMessage);
        CIR = CODE_MODEL.count() + 1;
        return;
    }
    if (!jumpInstruction.isEmpty())
    {
        CIR = FindOffsetByLabel(jumpInstruction);
        if (CIR < 0)
        {
            QMessageBox::warning(this, "Code execution error", "Cannot find label: " + jumpInstruction);
            return;
        }
    }
    else
    {
        CIR++;
    }
    if (!changedMem.isEmpty())
    {
        PrintMemory(code.label, changedMem);
    }
    // Set next instruction label.
    if (CIR < CODE_MODEL.count())
    {
        ui->nextInstructionLabel->setText(CODE_MODEL.at(CIR).toString());
    }
}

void MainWindow::on_stopBtn_clicked()
{
    MEMORY.clear();
    ClearData();
}

void MainWindow::PrintMemory(const QString &label, const QStringList &changedMem)
{
    static const auto getHeaderIndexByName = [&](const QString &header) -> int {
        for (auto i = 0; i < ui->memoryTable->horizontalHeader()->count(); i++)
            if (ui->memoryTable->horizontalHeaderItem(i) && ui->memoryTable->horizontalHeaderItem(i)->text() == header)
                return i;
        return -1;
    };
    auto row = ui->memoryTable->rowCount();
    ui->memoryTable->insertRow(row);
    ui->memoryTable->setVerticalHeaderItem(row, new QTableWidgetItem(label));
    for (const auto &key : changedMem.isEmpty() ? MEMORY.keys() : QList<QString>(changedMem))
    {
        auto col = getHeaderIndexByName(key);
        if (col < 0)
        {
            col = ui->memoryTable->columnCount();
            ui->memoryTable->insertColumn(col);
            ui->memoryTable->setHorizontalHeaderItem(col, new QTableWidgetItem(key));
        }
        ui->memoryTable->setItem(row, col, new QTableWidgetItem(NumberToString(MEMORY[key])));
    }
    //
    ui->memoryTable->scrollToBottom();
}

void MainWindow::on_assmTxt_textChanged()
{
    auto labels = GetLabels(ui->assmTxt->toPlainText());
    ui->labelList->clear();
    for (const auto &label : labels)
    {
        ui->labelList->addItem(label);
    }
}

void MainWindow::on_setMemBtn_clicked()
{
    auto addr = ui->memAddrTxt->text();
    if (!addr.isEmpty())
    {
        MEMORY[addr] = ui->memDataTxt->value();
    }
    PrintMemory("MEMSET", {});
}

void MainWindow::on_binOutputRad_clicked()
{
    Base = BASE2;
}

void MainWindow::on_decOutputRad_clicked()
{
    Base = BASE10;
}

void MainWindow::on_hexOutputRad_clicked()
{
    Base = BASE16;
}

void MainWindow::on_asciiOutputRad_clicked()
{
    Base = ASCII;
}
