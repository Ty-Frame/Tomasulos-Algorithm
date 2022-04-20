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
        ui->dataTypeComboBox->setCurrentText(ToString(instruction->mDataType));
        ui->arithmeticOptionsComboBox->setCurrentText(ToString(instruction->mArithmeticOptions));
        ui->memoryOptionsComboBox->setCurrentText(ToString(instruction->mMemoryOptions));
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
    if(ui->dataTypeComboBox->isEnabled()){
        returnI.mDataType = returnI.mDataType | StringToDataType(ui->dataTypeComboBox->currentText());
    }
    if(ui->arithmeticOptionsComboBox->isEnabled()){
        returnI.mArithmeticOptions = returnI.mArithmeticOptions | StringToArithmeticOptions(ui->arithmeticOptionsComboBox->currentText());
    }
    if(ui->memoryOptionsComboBox->isEnabled()){
        returnI.mMemoryOptions = returnI.mMemoryOptions | StringToMemoryOptions(ui->memoryOptionsComboBox->currentText());
    }
    return returnI;
}

void InstructionEditorDialog::initializeDialog()
{
    this->setWindowTitle("Create Instruction");

    for (auto iType : AllInstructionType) {
        ui->instructionTypeComboBox->addItem(ToString(iType));
    }

    for(auto dType: AllDataType){
        if(dType != DataType::None){
            ui->dataTypeComboBox->addItem(ToString(dType));
        }
    }

    for(auto arithOpt: AllArithmeticOptions){
        if(arithOpt != ArithmeticOptions::None){
            ui->arithmeticOptionsComboBox->addItem(ToString(arithOpt));
        }
    }

    for(auto memOpt: AllMemoryOptions){
        if(memOpt != MemoryOptions::None){
            ui->memoryOptionsComboBox->addItem(ToString(memOpt));
        }
    }
}

void InstructionEditorDialog::on_instructionTypeComboBox_currentTextChanged(const QString &arg1)
{
    InstructionType iTypeSelected = StringToInstructionType(arg1);
    switch (iTypeSelected) {
    case InstructionType::None:{
        ui->dataTypeComboBox->setEnabled(false);
        ui->arithmeticOptionsComboBox->setEnabled(false);
        ui->memoryOptionsComboBox->setEnabled(false);
        break;
    }
    case InstructionType::Arithmetic:{
        ui->dataTypeComboBox->setEnabled(true);
        ui->arithmeticOptionsComboBox->setEnabled(true);
        ui->memoryOptionsComboBox->setEnabled(false);
        break;
    }
    case InstructionType::Memory:{
        ui->dataTypeComboBox->setEnabled(false);
        ui->arithmeticOptionsComboBox->setEnabled(false);
        ui->memoryOptionsComboBox->setEnabled(true);
        break;
    }
    default: throw "[Unknown InstructionType]";
    }
}

void InstructionEditorDialog::on_okPushButton_clicked()
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

    if(ui->instructionTypeComboBox->currentText() == ToString(InstructionType::None)){
        QMessageBox::critical(this,"Instruction Creation Error", "Instruction type cannot be set to "+ToString(InstructionType::None)+".");
        return;
    }

    QDialog::accept();
}

void InstructionEditorDialog::on_cancelPushButton_clicked()
{
    QDialog::reject();
}

