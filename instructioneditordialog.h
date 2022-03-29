#ifndef INSTRUCTIONEDITORDIALOG_H
#define INSTRUCTIONEDITORDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "instruction.h"

namespace Ui {
class InstructionEditorDialog;
}

class InstructionEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionEditorDialog(QWidget *parent = nullptr, Instruction *instruction = nullptr);
    ~InstructionEditorDialog();
    Instruction returnInstruction();

private slots:
    void on_instructionTypeComboBox_currentTextChanged(const QString &arg1);

    void on_okPushButton_clicked();

    void on_cancelPushButton_clicked();

private:
    Ui::InstructionEditorDialog *ui;
    void initializeDialog();
};

#endif // INSTRUCTIONEDITORDIALOG_H
