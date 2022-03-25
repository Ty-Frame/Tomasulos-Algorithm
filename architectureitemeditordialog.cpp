#include "architectureitemeditordialog.h"
#include "ui_architectureitemeditordialog.h"

ArchitectureItemEditorDialog::ArchitectureItemEditorDialog(QWidget *parent, FunctionalUnit* fuPointer) :
    QDialog(parent),
    ui(new Ui::ArchitectureItemEditorDialog)
{
    ui->setupUi(this);

    initializeDialog();

    if(fuPointer){
        ui->nameLineEdit->setText(fuPointer->mName);
        ui->countSpinBox->setValue(fuPointer->mFunctionalUnitCount);
        ui->latencySpinBox->setValue(fuPointer->mLatency);
        ui->functionalUnitTypeComboBox->setCurrentText(ToString(fuPointer->mFunctionalUnitType));

        for(auto cb : ui->functionalUnitDataTypeGroupBox->findChildren<QCheckBox *>()){
            if(IsOfFunctionalUnitDataType(fuPointer, StringToFunctionalUnitDataType(cb->text()))) cb->setChecked(true);
        }

        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(IsOfArithmeticOptions(fuPointer, StringToArithmeticOptions(cb->text()))) cb->setChecked(true);
        }

        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(IsOfMemoryOptions(fuPointer, StringToMemoryOptions(cb->text()))) cb->setChecked(true);
        }
    }
}

ArchitectureItemEditorDialog::~ArchitectureItemEditorDialog()
{
    delete ui;
}

FunctionalUnit ArchitectureItemEditorDialog::returnFunctionalUnit()
{
    FunctionalUnit returnFU;
    returnFU.mName = ui->nameLineEdit->text();
    returnFU.mFunctionalUnitCount = ui->countSpinBox->value();
    returnFU.mLatency = ui->latencySpinBox->value();
    returnFU.mFunctionalUnitType = StringToFunctionalUnitType(ui->functionalUnitTypeComboBox->currentText());
    //for(auto obj : ui->functionalUnitDataTypeGroupBox->layout()->children()){
    if(ui->functionalUnitDataTypeGroupBox->isEnabled()){
        for(auto cb : ui->functionalUnitDataTypeGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnFU.mFunctionalUnitDataType = returnFU.mFunctionalUnitDataType | StringToFunctionalUnitDataType(cb->text());
            }
        }
    }
    if(ui->arithmeticOptionsGroupBox->isEnabled()){
        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnFU.mArithmeticOptions = returnFU.mArithmeticOptions | StringToArithmeticOptions(cb->text());
            }
        }
    }
    if(ui->memoryOptionsGroupBox->isEnabled()){
        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnFU.mMemoryOptions = returnFU.mMemoryOptions | StringToMemoryOptions(cb->text());
            }
        }
    }
    return returnFU;
}

void ArchitectureItemEditorDialog::initializeDialog()
{
    this->setWindowTitle("Create Architecture Item");

    FunctionalUnit fu;
    ui->countSpinBox->setMaximum(pow(2,sizeof(fu.mFunctionalUnitCount))*8-1);
    ui->latencySpinBox->setMaximum(pow(2,sizeof(fu.mLatency))*8-1);

    for (auto fuType : AllFunctionalUnitType) {
        ui->functionalUnitTypeComboBox->addItem(ToString(fuType));
    }

    QVBoxLayout* functionalUnitDataTypeGroupBoxLayout = new QVBoxLayout();
    ui->functionalUnitDataTypeGroupBox->setLayout(functionalUnitDataTypeGroupBoxLayout);
    for(auto fuDType: AllFunctionalUnitDataType){
        if(fuDType != DataType::None){
            functionalUnitDataTypeGroupBoxLayout->addWidget(new QCheckBox(ToString(fuDType)));
        }
    }

    QVBoxLayout* arithmeticOptionsGroupBoxLayout = new QVBoxLayout();
    ui->arithmeticOptionsGroupBox->setLayout(arithmeticOptionsGroupBoxLayout);
    for(auto arithOpt: AllArithmeticOptions){
        if(arithOpt != ArithmeticOptions::None){
            arithmeticOptionsGroupBoxLayout->addWidget(new QCheckBox(ToString(arithOpt)));
        }
    }

    QVBoxLayout* memoryOptionsGroupBoxLayout = new QVBoxLayout();
    ui->memoryOptionsGroupBox->setLayout(memoryOptionsGroupBoxLayout);
    for(auto memOpt: AllMemoryOptions){
        if(memOpt != MemoryOptions::None){
            memoryOptionsGroupBoxLayout->addWidget(new QCheckBox(ToString(memOpt)));
        }
    }
}

void ArchitectureItemEditorDialog::on_functionalUnitTypeComboBox_currentTextChanged(const QString &arg1)
{
    FunctionalUnitType fuTypeSelected = StringToFunctionalUnitType(arg1);
    switch (fuTypeSelected) {
    case FunctionalUnitType::None:{
        ui->functionalUnitDataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Arithmetic:{
        ui->functionalUnitDataTypeGroupBox->setEnabled(true);
        ui->arithmeticOptionsGroupBox->setEnabled(true);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Memory:{
        ui->functionalUnitDataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(true);
        break;
    }
    case FunctionalUnitType::CommonDataBus:{
        ui->functionalUnitDataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Register:{
        ui->functionalUnitDataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    default: throw "[Unknown FunctionalUnitType]";
    }
}

void ArchitectureItemEditorDialog::on_buttonBox_accepted()
{
    if(ui->nameLineEdit->text().isEmpty()){
       QMessageBox::critical(this,"Architecture Item Creation Error","The name line edit must have content.");
       return;
    }
    try {
        StringToFunctionalUnitType(ui->functionalUnitTypeComboBox->currentText());
    }  catch (std::string e) {
        QString msg = "Invalid Functional Unit Type:\n";
        msg.append(QString::fromStdString(e));
        QMessageBox::critical(this,"Architecture Item Creation Error", msg);
        return;
    }

    bool noneChecked = false;
    if(ui->functionalUnitDataTypeGroupBox->isEnabled()){
        noneChecked = true;
        for(auto cb : ui->functionalUnitDataTypeGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                noneChecked = false;
            }
        }
    }
    if(noneChecked){
        QMessageBox::critical(this,"Architecture Item Creation Error", "Atleast one functional unit data type must be selected.");
        return;
    }

    if(ui->arithmeticOptionsGroupBox->isEnabled()){
        noneChecked = true;
        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                noneChecked = false;
            }
        }
    }
    if(noneChecked){
        QMessageBox::critical(this,"Architecture Item Creation Error", "Atleast one arithmetic otion must be selected.");
        return;
    }

    if(ui->memoryOptionsGroupBox->isEnabled()){
        noneChecked = true;
        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                noneChecked = false;
            }
        }
    }
    if(noneChecked){
        QMessageBox::critical(this,"Architecture Item Creation Error", "Atleast one memory option must be selected.");
        return;
    }

    QDialog::accept();
}

void ArchitectureItemEditorDialog::on_buttonBox_rejected()
{
    QDialog::reject();
}

