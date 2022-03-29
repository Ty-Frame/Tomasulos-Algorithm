#ifndef ARCHITECTUREITEMEDITORDIALOG_H
#define ARCHITECTUREITEMEDITORDIALOG_H

#include <QDialog>
#include <QtMath>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include "functionalunit.h"

namespace Ui {
class ArchitectureItemEditorDialog;
}

class ArchitectureItemEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArchitectureItemEditorDialog(QWidget *parent = nullptr, FunctionalUnit* fuPointer = nullptr);
    ~ArchitectureItemEditorDialog();
    FunctionalUnit returnFunctionalUnit();

private slots:
    void on_functionalUnitTypeComboBox_currentTextChanged(const QString &arg1);

    void on_okPushButton_clicked();

    void on_cancelPushButton_clicked();

private:
    Ui::ArchitectureItemEditorDialog *ui;
    void initializeDialog();
};

#endif // ARCHITECTUREITEMEDITORDIALOG_H
