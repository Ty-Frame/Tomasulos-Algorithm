#ifndef INSTRUCTIONLISTFILEEDITORDIALOG_H
#define INSTRUCTIONLISTFILEEDITORDIALOG_H

#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include "instructioneditordialog.h"
#include "common.h"

namespace Ui {
class InstructionListFileEditorDialog;
}

class InstructionListFileEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionListFileEditorDialog(QWidget *parent = nullptr, QString *architectureFileName = nullptr);
    ~InstructionListFileEditorDialog();

private slots:
    void on_addInstructionPushButton_clicked();

    void on_editInstructionPushButton_clicked();

    void on_removeInstructionPushButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::InstructionListFileEditorDialog *ui;
    QList<Instruction>* mInstructionList = nullptr;
    void populateInstructionTable();
};

#endif // INSTRUCTIONLISTFILEEDITORDIALOG_H
