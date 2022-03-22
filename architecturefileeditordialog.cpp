#include "architecturefileeditordialog.h"
#include "ui_architecturefileeditordialog.h"

ArchitectureFileEditorDialog::ArchitectureFileEditorDialog(QWidget *parent, QString *architectureFileName) :
    QDialog(parent),
    ui(new Ui::ArchitectureFileEditorDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Architecture Editor");

    ui->architectureItemsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->addItemPushButton->setToolTip("Add Architecture Item");
    ui->removeItemPushButton->setToolTip("Remove Architecture Item");

    if(architectureFileName != nullptr){
        ui->architectureNameLineEdit->setText(architectureFileName->split("/").last().split(".").first());
        this->readInFile(architectureFileName);
    }
    else{
        try {
            FunctionalUnit cdb = StringToFunctionalUnit("Common Data Bus: Count{1}, Latency{0}, FunctionalUnitType{Common Data Bus}, FunctionalUnitDataType{}, ArithmeticOptions{}, MemoryOptions{}");
            mFunctionalUnitList->append(cdb);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Default "+ToString(FunctionalUnitType::CommonDataBus), e);
        }

        try {
            FunctionalUnit registers = StringToFunctionalUnit("R: Count{10}, Latency{0}, FunctionalUnitType{Register}, FunctionalUnitDataType{}, ArithmeticOptions{}, MemoryOptions{}");
            mFunctionalUnitList->append(registers);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Creating Default "+ToString(FunctionalUnitType::Register), e);
        }
    }

    this->populateArchitectureItemTable();
}

ArchitectureFileEditorDialog::~ArchitectureFileEditorDialog()
{
    delete mFunctionalUnitList;
    delete ui;
}

void ArchitectureFileEditorDialog::on_removeItemPushButton_clicked()
{
    QList<QTableWidgetSelectionRange> selectedList = ui->architectureItemsTableWidget->selectedRanges();
    int topRow, bottomRow, len, removedi, iteri;
    removedi = 0;
    auto model = ui->architectureItemsTableWidget->model();
    for(auto selection: selectedList){
        iteri = 0;
        topRow = selection.topRow();
        bottomRow = selection.bottomRow();
        len = bottomRow - topRow;
        do{
            model->removeRow(topRow-removedi);
            mFunctionalUnitList->removeAt(topRow-removedi);
            removedi++;
            iteri++;
        }while(iteri<=len);
    }
}

void ArchitectureFileEditorDialog::on_addItemPushButton_clicked()
{
    ArchitectureItemEditorDialog dlg(this);
    if(dlg.exec()){
        //qDebug()<<QString::fromStdString(ToString(dlg.returnFunctionalUnit()));
        mFunctionalUnitList->append(dlg.returnFunctionalUnit());
        this->populateArchitectureItemTable();
    }
}

void ArchitectureFileEditorDialog::on_buttonBox_accepted()
{
    if(ui->architectureNameLineEdit->text().isEmpty()){
        QMessageBox::critical(this, QString::fromStdString("Error Saving Architecture File"), "Architecture must be given a name.");
        return;
    }
    if(QFileInfo::exists(ARCHITECTURE_FILES_DIRECTORY_PATH+"/"+ui->architectureNameLineEdit->text()+".txt")){
        QMessageBox::StandardButton reply = QMessageBox::question(this,"Error Saving Architecture File","Do you wish to overwrite file:\n"+ARCHITECTURE_FILES_DIRECTORY_PATH+"/"+ui->architectureNameLineEdit->text()+".txt");
        if(reply == QMessageBox::No) return;
    }

    QFile outFile(ARCHITECTURE_FILES_DIRECTORY_PATH+"/"+ui->architectureNameLineEdit->text()+".txt");
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error Saving Architecture File", "Could not open file:\n" + ARCHITECTURE_FILES_DIRECTORY_PATH+"/"+ui->architectureNameLineEdit->text()+".txt");
        return;
    }
    QTextStream outFileStream(&outFile);

    int listLen = mFunctionalUnitList->length();
    FunctionalUnit fu;
    for (int i = 0; i<listLen; i++) {
        fu = mFunctionalUnitList->at(i);
        outFileStream << ToString(fu) << "\n";
    }
    outFile.close();
}

void ArchitectureFileEditorDialog::on_buttonBox_rejected()
{

}

void ArchitectureFileEditorDialog::populateArchitectureItemTable()
{
    auto model = ui->architectureItemsTableWidget->model();
    int len = model->rowCount();
    for (int i = 0; i<len; i++) {
        model->removeRow(0);
    }

    int listLen = this->mFunctionalUnitList->length();

    ui->architectureItemsTableWidget->setColumnCount(1);
    ui->architectureItemsTableWidget->setRowCount(listLen);

    FunctionalUnit fu;
    for (int i = 0; i<listLen; i++) {
        fu = this->mFunctionalUnitList->at(i);
        model->setData(model->index(i,0),ToString(fu));
    }
}

void ArchitectureFileEditorDialog::readInFile(QString *filename)
{
    QFile inFile(*filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, QString::fromStdString("Error Opening Architecture File"), QString::fromStdString(std::string("Could not open file:\n")) + *filename);
        return;
    }

    QTextStream inFileStream(&inFile);
    while(!inFileStream.atEnd()){
        QString fuString = inFileStream.readLine();
        try {
            FunctionalUnit fu = StringToFunctionalUnit(fuString);
            mFunctionalUnitList->append(fu);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Reading In Architecture Item", e);
        }
    }
    inFile.close();
}

void ArchitectureFileEditorDialog::on_editItemPushButton_clicked()
{
    if(ui->architectureItemsTableWidget->selectionModel()->selectedRows().size() == 0){
        QMessageBox::critical(this, "Error Editing Architecture Item", "An item must be selected in the architecture items table.");
        return;
    }

    QModelIndex selectedIndex = ui->architectureItemsTableWidget->selectionModel()->selectedRows().first();
    FunctionalUnit selectedFU = StringToFunctionalUnit(ui->architectureItemsTableWidget->item(selectedIndex.row(), selectedIndex.column())->text());
    int beforeIndex = mFunctionalUnitList->indexOf(selectedFU);

    ArchitectureItemEditorDialog dlg(this, &selectedFU);
    if(dlg.exec()){
        mFunctionalUnitList->removeAt(beforeIndex);
        mFunctionalUnitList->insert(beforeIndex, dlg.returnFunctionalUnit());
        this->populateArchitectureItemTable();
    }
}

