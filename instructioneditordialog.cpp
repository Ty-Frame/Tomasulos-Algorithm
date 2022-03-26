#include "instructioneditordialog.h"
#include "ui_instructioneditordialog.h"

InstructionEditorDialog::InstructionEditorDialog(QWidget *parent, Instruction *instruction) :
    QDialog(parent),
    ui(new Ui::InstructionEditorDialog)
{
    ui->setupUi(this);

    initializeDialog();

    if(instruction){
        ui->nameLineEdit->setText(instruction->mName);
        ui->instructionTypeComboBox->setCurrentText(ToString(instruction->mInstructionType));

        for(auto cb : ui->dataTypeGroupBox->findChildren<QCheckBox *>()){
            if(IsOfDataType(instruction, StringToDataType(cb->text()))) cb->setChecked(true);
        }

        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(IsOfArithmeticOptions(instruction, StringToArithmeticOptions(cb->text()))) cb->setChecked(true);
        }

        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(IsOfMemoryOptions(instruction, StringToMemoryOptions(cb->text()))) cb->setChecked(true);
        }
    }
}

InstructionEditorDialog::~InstructionEditorDialog()
{
    delete ui;
}

Instruction InstructionEditorDialog::returnInstruction()
{
    Instruction returnI;
    returnI.mName = ui->nameLineEdit->text();
    returnI.mInstructionType = StringToInstructionType(ui->instructionTypeComboBox->currentText());
    if(ui->dataTypeGroupBox->isEnabled()){
        for(auto cb : ui->dataTypeGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnI.mDataType = returnI.mDataType | StringToDataType(cb->text());
            }
        }
    }
    if(ui->arithmeticOptionsGroupBox->isEnabled()){
        for(auto cb : ui->arithmeticOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnI.mArithmeticOptions = returnI.mArithmeticOptions | StringToArithmeticOptions(cb->text());
            }
        }
    }
    if(ui->memoryOptionsGroupBox->isEnabled()){
        for(auto cb : ui->memoryOptionsGroupBox->findChildren<QCheckBox *>()){
            if(cb->isChecked()){
                //qDebug()<<"Bitwise OR: "<<cb->text();
                returnI.mMemoryOptions = returnI.mMemoryOptions | StringToMemoryOptions(cb->text());
            }
        }
    }
    return returnI;
}

void InstructionEditorDialog::initializeDialog()
{
    this->setWindowTitle("Create Instruction");

    for (auto iType : AllInstructionType) {
        ui->instructionTypeComboBox->addItem(ToString(iType));
    }

    QVBoxLayout* dataTypeGroupBoxLayout = new QVBoxLayout();
    ui->dataTypeGroupBox->setLayout(dataTypeGroupBoxLayout);
    for(auto dType: AllDataType){
        if(dType != DataType::None){
            dataTypeGroupBoxLayout->addWidget(new QCheckBox(ToString(dType)));
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

void InstructionEditorDialog::on_instructionTypeComboBox_currentTextChanged(const QString &arg1)
{
    InstructionType iTypeSelected = StringToInstructionType(arg1);
    switch (iTypeSelected) {
    case InstructionType::None:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    case InstructionType::Arithmetic:{
        ui->dataTypeGroupBox->setEnabled(true);
        ui->arithmeticOptionsGroupBox->setEnabled(true);
        ui->memoryOptionsGroupBox->setEnabled(false);
        break;
    }
    case InstructionType::Memory:{
        ui->dataTypeGroupBox->setEnabled(false);
        ui->arithmeticOptionsGroupBox->setEnabled(false);
        ui->memoryOptionsGroupBox->setEnabled(true);
        break;
    }
    default: throw "[Unknown InstructionType]";
    }
}


void InstructionEditorDialog::on_buttonBox_rejected()
{
    QDialog::reject();
}


void InstructionEditorDialog::on_buttonBox_accepted()
{
    if(ui->nameLineEdit->text().isEmpty()){
       QMessageBox::critical(this,"Instruction Creation Error","The name line edit must have content.");
       return;
    }
    try {
        StringToInstructionType(ui->instructionTypeComboBox->currentText());
    }  catch (std::string e) {
        QString msg = "Invalid Instruction Type:\n";
        msg.append(QString::fromStdString(e));
        QMessageBox::critical(this,"Instruction Creation Error", msg);
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
        QMessageBox::critical(this,"Instruction Creation Error", "Atleast one data type must be selected.");
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
        QMessageBox::critical(this,"Instruction Creation Error", "Atleast one arithmetic otion must be selected.");
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
        QMessageBox::critical(this,"Instruction Creation Error", "Atleast one memory option must be selected.");
        return;
    }

    QDialog::accept();
}

