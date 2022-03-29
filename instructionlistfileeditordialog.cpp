#include "instructionlistfileeditordialog.h"
#include "ui_instructionlistfileeditordialog.h"

InstructionListFileEditorDialog::InstructionListFileEditorDialog(QWidget *parent, QString *instructionListFileName) :
    QDialog(parent),
    ui(new Ui::InstructionListFileEditorDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Instruction List Editor");

    ui->instrucitonTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->addInstructionPushButton->setToolTip("Add Instruction");
    ui->editInstructionPushButton->setToolTip("Edit Instruction");
    ui->removeInstructionPushButton->setToolTip("Remove Instruction");

    if(instructionListFileName){
        mInstructionList = readInInstructionListFile(instructionListFileName);
    }
    else{
        mInstructionList = new QList<Instruction>();

        try {
            Instruction ld = StringToInstruction("ld: InstructionType{Memory}, DataType{}, ArithmeticOptions{}, MemoryOptions{Load}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(ld);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction str = StringToInstruction("str: InstructionType{Memory}, DataType{}, ArithmeticOptions{}, MemoryOptions{Store}, PipelineStages{Issue, Execution}");
            mInstructionList->append(str);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction add = StringToInstruction("add: InstructionType{Arithmetic}, DataType{Integer}, ArithmeticOptions{Add}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(add);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction sub = StringToInstruction("sub: InstructionType{Arithmetic}, DataType{Integer}, ArithmeticOptions{Subtract}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(sub);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction mul = StringToInstruction("mul: InstructionType{Arithmetic}, DataType{Integer}, ArithmeticOptions{Multiply}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(mul);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction div = StringToInstruction("div: InstructionType{Arithmetic}, DataType{Integer}, ArithmeticOptions{Divide}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(div);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction addf = StringToInstruction("addf: InstructionType{Arithmetic}, DataType{Float}, ArithmeticOptions{Add}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(addf);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction subf = StringToInstruction("subf: InstructionType{Arithmetic}, DataType{Float}, ArithmeticOptions{Subtract}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(subf);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction mulf = StringToInstruction("mulf: InstructionType{Arithmetic}, DataType{Float}, ArithmeticOptions{Multiply}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(mulf);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }

        try {
            Instruction divf = StringToInstruction("divf: InstructionType{Arithmetic}, DataType{Float}, ArithmeticOptions{Divide}, MemoryOptions{}, PipelineStages{Issue, Execution, Writeback, Commit}");
            mInstructionList->append(divf);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Instruction", e);
        }
    }

    this->populateInstructionTable();
}

InstructionListFileEditorDialog::~InstructionListFileEditorDialog()
{
    delete ui;
}

void InstructionListFileEditorDialog::on_addInstructionPushButton_clicked()
{
    InstructionEditorDialog dlg(this);
    if(dlg.exec()){
        //qDebug()<<ToString(dlg.returnInstruction());
        mInstructionList->append(dlg.returnInstruction());
        this->populateInstructionTable();
    }
}


void InstructionListFileEditorDialog::on_editInstructionPushButton_clicked()
{
    if(ui->instrucitonTableWidget->selectionModel()->selectedRows().size() == 0){
        QMessageBox::critical(this, "Error Editing Architecture Item", "An item must be selected in the architecture items table.");
        return;
    }

    QModelIndex selectedIndex = ui->instrucitonTableWidget->selectionModel()->selectedRows().first();
    Instruction selectedFU = StringToInstruction(ui->instrucitonTableWidget->item(selectedIndex.row(), selectedIndex.column())->text());
    int beforeIndex = mInstructionList->indexOf(selectedFU);

    InstructionEditorDialog dlg(this, &selectedFU);
    if(dlg.exec()){
        mInstructionList->removeAt(beforeIndex);
        mInstructionList->insert(beforeIndex, dlg.returnInstruction());
        this->populateInstructionTable();
    }
}


void InstructionListFileEditorDialog::on_removeInstructionPushButton_clicked()
{
    QList<QTableWidgetSelectionRange> selectedList = ui->instrucitonTableWidget->selectedRanges();
    int topRow, bottomRow, len, removedi, iteri;
    removedi = 0;
    auto model = ui->instrucitonTableWidget->model();
    for(auto selection: selectedList){
        iteri = 0;
        topRow = selection.topRow();
        bottomRow = selection.bottomRow();
        len = bottomRow - topRow;
        do{
            model->removeRow(topRow-removedi);
            mInstructionList->removeAt(topRow-removedi);
            removedi++;
            iteri++;
        }while(iteri<=len);
    }
}


void InstructionListFileEditorDialog::on_buttonBox_accepted()
{
    if(ui->instructionListNameLineEdit->text().isEmpty()){
        QMessageBox::critical(this, QString::fromStdString("Error Saving Instruction List File"), "Architecture must be given a name.");
        return;
    }

    QString filename = INSTRUCTION_LIST_FILES_DIRECTORY_PATH + ui->instructionListNameLineEdit->text() + ".txt";

    if(QFileInfo::exists(filename)){
        QMessageBox::StandardButton reply = QMessageBox::question(this,"Error Saving Instruction List File","Do you wish to overwrite file:\n" + filename);
        if(reply == QMessageBox::No) return;
    }

    QFile outFile(filename);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error Saving Instruction List File", "Could not open file:\n" + filename);
        return;
    }
    QTextStream outFileStream(&outFile);

    int listLen = mInstructionList->length();
    Instruction fu;
    for (int i = 0; i<listLen; i++) {
        fu = mInstructionList->at(i);
        outFileStream << ToString(fu) << "\n";
    }
    outFile.close();
}


void InstructionListFileEditorDialog::on_buttonBox_rejected()
{

}

void InstructionListFileEditorDialog::populateInstructionTable()
{
    auto model = ui->instrucitonTableWidget->model();
    int len = model->rowCount();
    for (int i = 0; i<len; i++) {
        model->removeRow(0);
    }

    int listLen = this->mInstructionList->length();

    ui->instrucitonTableWidget->setColumnCount(1);
    ui->instrucitonTableWidget->setRowCount(listLen);

    Instruction fu;
    for (int i = 0; i<listLen; i++) {
        fu = this->mInstructionList->at(i);
        model->setData(model->index(i,0),ToString(fu));
    }
}

