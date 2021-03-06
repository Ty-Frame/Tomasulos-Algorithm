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
        ui->reservationStationCountSpinBox->setValue(fuPointer->mReservationStationCount);

        for(auto cb : ui->dataTypeGroupBox->findChildren<QCheckBox *>()){
            if(IsOfDataType(fuPointer, StringToDataType(cb->text()))) cb->setChecked(true);
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
    returnFU.mReservationStationCount = ui->reservationStationCountSpinBox->value();

    if(ui->dataTypeGroupBox->isEnabled()){
        for(auto cb : ui->dataTypeGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                returnFU.mDataType = returnFU.mDataType | StringToDataType(cb->text());
            }
        }
    }
    if(ui->arithmeticOptionsGroupBox->isEnabled()){
        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                returnFU.mArithmeticOptions = returnFU.mArithmeticOptions | StringToArithmeticOptions(cb->text());
            }
        }
    }
    if(ui->memoryOptionsGroupBox->isEnabled()){
        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                returnFU.mMemoryOptions = returnFU.mMemoryOptions | StringToMemoryOptions(cb->text());
            }
        }
    }
    return returnFU;
}

void ArchitectureItemEditorDialog::initializeDialog()
{
    this->setWindowTitle("Create Architecture Item");

    for (auto fuType : AllFunctionalUnitType) {
        ui->functionalUnitTypeComboBox->addItem(ToString(fuType));
    }

    QVBoxLayout* dataTypeGroupBoxLayout = new QVBoxLayout();
    ui->dataTypeGroupBox->setLayout(dataTypeGroupBoxLayout);
    for(auto fuDType: AllDataType){
        if(fuDType != DataType::None){
            dataTypeGroupBoxLayout->addWidget(new QCheckBox(ToString(fuDType)));
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
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        ui->reservationStationCountSpinBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Arithmetic:{
        ui->dataTypeGroupBox->setEnabled(true);
        ui->arithmeticOptionsGroupBox->setEnabled(true);
        ui->memoryOptionsGroupBox->setEnabled(false);
        ui->reservationStationCountSpinBox->setEnabled(true);
        break;
    }
    case FunctionalUnitType::Memory:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(true);
        ui->reservationStationCountSpinBox->setEnabled(true);
        break;
    }
    case FunctionalUnitType::CommonDataBus:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        ui->reservationStationCountSpinBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Register:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        ui->reservationStationCountSpinBox->setEnabled(false);
        break;
    }
    case FunctionalUnitType::Issuer:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        ui->reservationStationCountSpinBox->setEnabled(false);
        break;
    }
    default: throw "[Unknown FunctionalUnitType]";
    }
}


void ArchitectureItemEditorDialog::on_okPushButton_clicked()
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

    if(ui->functionalUnitTypeComboBox->currentText() == ToString(FunctionalUnitType::None)){
        QMessageBox::critical(this,"Architecture Item Creation Error", "Functional unit cannot be set to "+ToString(FunctionalUnitType::None)+".");
        return;
    }

    bool noneChecked = false;
    if(ui->dataTypeGroupBox->isEnabled()){
        noneChecked = true;
        for(auto cb : ui->dataTypeGroupBox->findChildren<QCheckBox *>()){
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


void ArchitectureItemEditorDialog::on_cancelPushButton_clicked()
{
    QDialog::reject();
}

