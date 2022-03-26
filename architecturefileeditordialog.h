#ifndef ARCHITECTUREFILEEDITORDIALOG_H
#define ARCHITECTUREFILEEDITORDIALOG_H

#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include "common.h"
#include "architectureitemeditordialog.h"

namespace Ui {
class ArchitectureFileEditorDialog;
}

class ArchitectureFileEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArchitectureFileEditorDialog(QWidget *parent = nullptr, QString *architectureFileName = nullptr);
    ~ArchitectureFileEditorDialog();

private slots:
    void on_removeItemPushButton_clicked();

    void on_addItemPushButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_editItemPushButton_clicked();

private:
    Ui::ArchitectureFileEditorDialog *ui;
    QList<FunctionalUnit>* mFunctionalUnitList = nullptr;
    void populateArchitectureItemTable();
};

#endif // ARCHITECTUREFILEEDITORDIALOG_H
