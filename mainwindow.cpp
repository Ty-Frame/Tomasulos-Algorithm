#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mTomasuloAlgorithm = new TomasuloAlgorithm(mGeneralFunctionalUnitList,
                            mMemoryFunctionalUnitList,
                            mRegisterFunctionalUnitList,
                            mCommonDataBusFunctionalUnitList,
                            mScriptInstructionList,
                            mIssueNumber);
    connect(mTomasuloAlgorithm,SIGNAL(UpdateRunStatus()),this,SLOT(updateRunStatusOptions()));

    initializeWindow();
}

MainWindow::~MainWindow()
{
    // Delete status bar widgets
    mStatusBarWidget->deleteLater();

    // Clear and delete functional unit lists
    mGeneralFunctionalUnitList->clear();
    mMemoryFunctionalUnitList->clear();
    mRegisterFunctionalUnitList->clear();
    mCommonDataBusFunctionalUnitList->clear();
    mScriptInstructionList->clear();
    mInstructionList->clear();

    delete mGeneralFunctionalUnitList;
    delete mMemoryFunctionalUnitList;
    delete mRegisterFunctionalUnitList;
    delete mCommonDataBusFunctionalUnitList;
    delete mScriptInstructionList;
    delete mInstructionList;

    delete ui;
}

void MainWindow::initializeWindow()
{
    // Assign window title
    this->setWindowTitle("Tomasulo's Algorithm Visualizer " + QString::fromStdString(VERSION));
    this->setWindowState(Qt::WindowMaximized);

    // Initializing needed directories if needed
    if(!QDir(ARCHITECTURE_FILES_DIRECTORY_PATH).exists()){
        QDir().mkdir(ARCHITECTURE_FILES_DIRECTORY_PATH);
    }
    if(!QDir(INSTRUCTION_LIST_FILES_DIRECTORY_PATH).exists()){
        QDir().mkdir(INSTRUCTION_LIST_FILES_DIRECTORY_PATH);
    }
    if(!QDir(SCRIPT_FILES_DIRECTORY_PATH).exists()){
        QDir().mkdir(SCRIPT_FILES_DIRECTORY_PATH);
    }

    // Initialize status bar
    mStatusBarWidget = new QWidget(this);
    ui->statusbar->addWidget(mStatusBarWidget,1);

    mStatusBarLayout = new QHBoxLayout(mStatusBarWidget);

    QLabel* label = new QLabel("Architecture File: ");
    mStatusBarLayout->addWidget(label);
    mLoadedArchitectureFile = new QLabel("N/A");
    mStatusBarLayout->addWidget(mLoadedArchitectureFile);

    QSpacerItem* statusBarSpacer = new QSpacerItem(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    mStatusBarLayout->addItem(statusBarSpacer);

    label = new QLabel("Instruction List File: ");
    mStatusBarLayout->addWidget(label);
    mLoadedInstructionListFile = new QLabel("N/A");
    mStatusBarLayout->addWidget(mLoadedInstructionListFile);

    statusBarSpacer = new QSpacerItem(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    mStatusBarLayout->addItem(statusBarSpacer);

    label = new QLabel("Script File: ");
    mStatusBarLayout->addWidget(label);
    mLoadedScriptFile = new QLabel("N/A");
    mStatusBarLayout->addWidget(mLoadedScriptFile);

    statusBarSpacer = new QSpacerItem(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    mStatusBarLayout->addItem(statusBarSpacer);

    ui->actionManual_Step_Algorithm->setEnabled(false);
    ui->actionClock_Step_Algorithm->setEnabled(false);
    ui->actionFull_Speed_Algorithm->setEnabled(false);
    ui->actionStep_Algorithm->setEnabled(false);
    ui->actionPause_Algorithm->setEnabled(false);
    ui->actionReset_Algorithm->setEnabled(false);
    connect(mRunClock,SIGNAL(timeout()),this,SLOT(individualStep()));
    connect(this,SIGNAL(processStep()),mTomasuloAlgorithm,SLOT(processStep()));


    // Initialize tables in status tab
    // Instruction status table columns: instruction, destination register, source 1 register, source 2 register, issue clock cycle, execution complete clock cycle, write result clock cycle
    ui->instructionStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->instructionStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->instructionStatusTableWidget->setColumnCount(7);
    ui->instructionStatusTableWidget->setRowCount(1);
    auto model = ui->instructionStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Instruction"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Destination Register"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Source 1 Register"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Source 2 Register"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Issue CC"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Execution Completion CC"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,6),QStringLiteral("Write Result CC"));
    model->setData(model->index(0,6), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Functional unit reservation status columns: time, name, busy, operation, source 1 register, FU for source 1 register, source 2 register, FU for source 2 register
    ui->functionalUnitReservationStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->functionalUnitReservationStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->functionalUnitReservationStatusTableWidget->setColumnCount(6);
    ui->functionalUnitReservationStatusTableWidget->setRowCount(1);
    model = ui->functionalUnitReservationStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Time"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Name"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Busy"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Operation"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Source 1"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Source 2"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Memory reservation status columns: name, busy, address
    ui->memoryReservationStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->memoryReservationStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->memoryReservationStatusTableWidget->setColumnCount(4);
    ui->memoryReservationStatusTableWidget->setRowCount(1);
    model = ui->memoryReservationStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Name"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Operation"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Busy"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Address"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Register hold table has column for all registers, row is labeled 'Functional Unit'
    ui->commonDataBusAndRegisterStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->commonDataBusAndRegisterStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->commonDataBusAndRegisterStatusTableWidget->setColumnCount(1);
    ui->commonDataBusAndRegisterStatusTableWidget->setRowCount(4);
    model = ui->commonDataBusAndRegisterStatusTableWidget->model();
    model->setData(model->index(1,0),QStringLiteral("Funcitonal Unit"));
    model->setData(model->index(1,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(3,0),QStringLiteral("Funcitonal Unit"));
    model->setData(model->index(3,0), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Execution Table
    ui->executionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->executionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->executionTableWidget->setColumnCount(6);
    ui->executionTableWidget->setRowCount(1);
    model = ui->executionTableWidget->model();

    model->setData(model->index(0,0),QStringLiteral("Instruction"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Issue"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Execution"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Read Access"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Write on CDB"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Commit"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
}

//void MainWindow::popupStartMenu()
//{
//    if(mLoadedArchitectureFile->text()=="N/A" ||mLoadedInstructionListFile->text()=="N/A" ||mLoadedScriptFile->text()=="N/A"){
//        QMessageBox::critical(this,"Error Starting Algorithm","Atleast one file has not been imported.\nTherefore the algorithm cannot be started.");
//        return;
//    }
//    if(mTomasuloAlgorithm->getRunStatus()!=TomasuloRunStatus::NotStarted){
//        QMessageBox::critical(this,"Error Starting Algorithm","Algorithm is either in progress or done.\nAlgorithm must be reset before it can be restarted.");
//        return;
//    }

//    QPoint mousePT = this->mapToGlobal(QCursor::pos());
//    QMenu* popup = new QMenu(this);

//    QAction* manualAction = new QAction("Manual Step",this);
//    QAction* clockAction = new QAction("Clock Step",this);
//    QAction* automaticAction = new QAction("Full Speed",this);
//    popup->addAction(manualAction);
//    popup->addAction(clockAction);
//    popup->addAction(automaticAction);

//    QAction* chosenAction = popup->exec(mousePT);
//    if(chosenAction==manualAction){
//        mStatusBarLayout->removeWidget(mStatusBarStartButton);
//        mStatusBarLayout->addWidget(mStatusBarStepButton);
//        mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::ManualStep);
//    }
//    else if(chosenAction==clockAction){
//        bool ok;
//        float val = QInputDialog::getDouble(this,"Clock Speed","What would you like the clock cycle to be?",1.00,0.1,5,2,&ok);
//        if(!ok){
//            return;
//        }
//        mStatusBarLayout->removeWidget(mStatusBarStartButton);
//        mStatusBarLayout->addWidget(mStatusBarPauseButton);
//        mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::ClockStep);

//        mRunClock->setInterval(val*1000);
//        mRunClock->start();
//    }
//    else if(chosenAction==automaticAction){
//        mStatusBarLayout->addWidget(mStatusBarPauseButton);
//        mStatusBarLayout->removeWidget(mStatusBarStartButton);
//        mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::AutomaticStep);
//        this->fullSpeedStep();
//    }
//    else{
//        return;
//    }
//}

void MainWindow::individualStep()
{
    emit processStep();
    updateAllTables();
    if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::Done){
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        if(mRunClock->isActive()){
            ui->actionPause_Algorithm->setEnabled(false);
            mRunClock->stop();
        }
        else{
            ui->actionStep_Algorithm->setEnabled(false);
        }
    }
}

void MainWindow::fullSpeedStep()
{
    while(mTomasuloAlgorithm->getRunStatus()!=TomasuloRunStatus::Done && mTomasuloAlgorithm->getRunStatus()!=TomasuloRunStatus::Paused){
        emit processStep();
        updateAllTables();
    }

    if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::Done){
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionPause_Algorithm->setEnabled(false);
    }
}

void MainWindow::pauseClock()
{
    if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::ClockStep){
        mRunClock->stop();
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionPause_Algorithm->setEnabled(false);
        mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::Paused);
        qDebug()<<"clock step paused";
    }
    else if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::AutomaticStep){
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionPause_Algorithm->setEnabled(false);
        mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::Paused);
        qDebug()<<"automatic step paused";
    }
    else{
        QMessageBox::critical(this,"Error With Pause Button","Run status not accounted for: "+QString::number(static_cast<int>(mTomasuloAlgorithm->getRunStatus())));
    }
}

void MainWindow::updateAllTables()
{
    ui->clockCountLabelStatusTab->setText(QString::number(mTomasuloAlgorithm->clockCycle()-1));
    populateFunctionalUnitReservationTable();
    populateCommonDataBusAndRegisterTable();
    populateMemoryReservationTable();
    populateInstructionTable();
    populateExecutionTable();
}

void MainWindow::on_actionCreate_Architecture_triggered()
{
    ArchitectureFileEditorDialog dlg(this);
    dlg.exec();
}


void MainWindow::on_actionLoad_Architecture_triggered()
{
    if(mLoadedArchitectureFile->text()!="N/A") {
        QMessageBox::critical(this, "Error Loading Architecture File","There is an architecture that is already loaded.\n To load an architecture file, reset the algorithm.");
        return;
    }

    if(!checkNotRunning()) {
        QMessageBox::critical(this, "Error Loading Architecture File","Algorithm is already running.\n To load an architecture file, reset the algorithm.");
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Select Architecture File", ARCHITECTURE_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;

    loadArchitecture(filename);
}

void MainWindow::loadArchitecture(QString filename)
{
    QFile inFile(filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error Opening Architecture File","Could not open file:\n" + filename);
        return;
    }

    QTextStream inFileStream(&inFile);
    FunctionalUnit fu;
    while(!inFileStream.atEnd()){
        QString fuString = inFileStream.readLine();
        try {
            fu = StringToFunctionalUnit(fuString);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Reading In Architecture Item", e);
        }

        switch (fu.mFunctionalUnitType) {
        case FunctionalUnitType::Arithmetic:{
            for (int i = 0; i<fu.mFunctionalUnitCount; i++) {
                GeneralFunctionalUnit* gfu = new GeneralFunctionalUnit();
                gfu->mFunctionalUnit = fu;
                gfu->mFunctionalUnit.mName = gfu->mFunctionalUnit.mName + " " + QString::number(i+1);
                mGeneralFunctionalUnitList->append(gfu);
            }
            break;
        }
        case FunctionalUnitType::Memory:{
            for (int i = 0; i<fu.mFunctionalUnitCount; i++) {
                MemoryFunctionalUnit* mfu = new MemoryFunctionalUnit();
                mfu->mFunctionalUnit = fu;
                mfu->mFunctionalUnit.mName = mfu->mFunctionalUnit.mName + " " + QString::number(i+1);
                mMemoryFunctionalUnitList->append(mfu);
            }
            break;
        }
        case FunctionalUnitType::CommonDataBus:{
            for (int i = 0; i<fu.mFunctionalUnitCount; i++) {
                CommonDataBusFunctionalUnit* cdbfu = new CommonDataBusFunctionalUnit();
                cdbfu->mFunctionalUnit = fu;
                cdbfu->mFunctionalUnit.mName = cdbfu->mFunctionalUnit.mName + " " + QString::number(i+1);
                mCommonDataBusFunctionalUnitList->append(cdbfu);
            }
            break;
        }
        case FunctionalUnitType::Register:{
            for (int i = 0; i<fu.mFunctionalUnitCount; i++) {
                RegisterFunctionalUnit* rfu = new RegisterFunctionalUnit();
                rfu->mFunctionalUnit = fu;
                rfu->mFunctionalUnit.mName = rfu->mFunctionalUnit.mName + QString::number(i);
                mRegisterFunctionalUnitList->append(rfu);
            }
            break;
        }
        case FunctionalUnitType::Issuer:{
            mTomasuloAlgorithm->setIssueNumber(fu.mFunctionalUnitCount);
            break;
        }
        default:{
            QMessageBox::critical(this, "Error Loading Architecture", "Not sure what to do with functional unit of type: " + ToString(fu.mFunctionalUnitType) + "Funcitonal Unit: " + fuString);
            continue;
            break;
        }
        }
    }
    inFile.close();

    mLoadedArchitectureFile->setText(filename.split("/").last().split(".").first());
    mLoadedArchitectureFile->setToolTip(filename);

    populateFunctionalUnitReservationTable();
    populateMemoryReservationTable();
    populateCommonDataBusAndRegisterTable();
}

void MainWindow::populateFunctionalUnitReservationTable()
{
    ui->functionalUnitReservationStatusTableWidget->clear();

    ui->functionalUnitReservationStatusTableWidget->setColumnCount(6);
    ui->functionalUnitReservationStatusTableWidget->setRowCount(1 + mGeneralFunctionalUnitList->length());
    auto model = ui->functionalUnitReservationStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Time"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Name"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Busy"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Operation"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Source 1"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Source 2"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);

    for (int i = 0; i<mGeneralFunctionalUnitList->length(); i++) {
        if(mGeneralFunctionalUnitList->at(i)->mCountDown<0){
            model->setData(model->index(1 + i,0),"-");
        }
        else{
            model->setData(model->index(1 + i,0),mGeneralFunctionalUnitList->at(i)->mCountDown);
        }
        model->setData(model->index(1 + i,0), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,1),mGeneralFunctionalUnitList->at(i)->mFunctionalUnit.mName);
        model->setData(model->index(1 + i,1), Qt::AlignCenter, Qt::TextAlignmentRole);
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout();
        widget->setLayout(layout);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(new QCheckBox(""));
        ui->functionalUnitReservationStatusTableWidget->setCellWidget(1 + i, 2, widget);
        model->setData(model->index(1 + i,3),mGeneralFunctionalUnitList->at(i)->mOperation);
        model->setData(model->index(1 + i,3), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,4),mGeneralFunctionalUnitList->at(i)->mSourceOne);
        model->setData(model->index(1 + i,4), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,5),mGeneralFunctionalUnitList->at(i)->mSourceTwo);
        model->setData(model->index(1 + i,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    }
}

void MainWindow::populateCommonDataBusAndRegisterTable()
{
    ui->commonDataBusAndRegisterStatusTableWidget->clear();

    if(mCommonDataBusFunctionalUnitList->length() > mRegisterFunctionalUnitList->length()) ui->commonDataBusAndRegisterStatusTableWidget->setColumnCount(1 + mCommonDataBusFunctionalUnitList->length());
    else ui->commonDataBusAndRegisterStatusTableWidget->setColumnCount(1 + mRegisterFunctionalUnitList->length());

    ui->commonDataBusAndRegisterStatusTableWidget->setRowCount(4);
    auto model = ui->commonDataBusAndRegisterStatusTableWidget->model();
    model->setData(model->index(1,0),QStringLiteral("Funcitonal Unit"));
    model->setData(model->index(1,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(3,0),QStringLiteral("Funcitonal Unit"));
    model->setData(model->index(3,0), Qt::AlignCenter, Qt::TextAlignmentRole);

    for (int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++) {
        model->setData(model->index(2, 1 + i), mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnit.mName);
        model->setData(model->index(2, 1 + i), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(3, 1 + i), mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim);
        model->setData(model->index(3, 1 + i), Qt::AlignCenter, Qt::TextAlignmentRole);
    }

    for (int i = 0; i<mRegisterFunctionalUnitList->length(); i++) {
        model->setData(model->index(0, 1 + i), mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName);
        model->setData(model->index(0, 1 + i), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1, 1 + i), mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim);
        model->setData(model->index(1, 1 + i), Qt::AlignCenter, Qt::TextAlignmentRole);
    }
}

void MainWindow::populateMemoryReservationTable()
{
    ui->memoryReservationStatusTableWidget->clear();

    ui->memoryReservationStatusTableWidget->setColumnCount(4);
    ui->memoryReservationStatusTableWidget->setRowCount(1 + mMemoryFunctionalUnitList->length());
    auto model = ui->memoryReservationStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Name"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Operation"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Busy"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Address"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);

    for (int i = 0; i<mMemoryFunctionalUnitList->length(); i++) {
        model->setData(model->index(1 + i, 0), mMemoryFunctionalUnitList->at(i)->mFunctionalUnit.mName);
        model->setData(model->index(1 + i, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i, 1), mMemoryFunctionalUnitList->at(i)->mOperation);
        model->setData(model->index(1 + i, 1), Qt::AlignCenter, Qt::TextAlignmentRole);
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout();
        widget->setLayout(layout);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(new QCheckBox(""));
        ui->memoryReservationStatusTableWidget->setCellWidget(1 + i, 2, widget);
        model->setData(model->index(1 + i, 3), mMemoryFunctionalUnitList->at(i)->mSourceOne);
        model->setData(model->index(1 + i, 3), Qt::AlignCenter, Qt::TextAlignmentRole);
    }
}

void MainWindow::on_actionEdit_Architecture_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Architecture File", ARCHITECTURE_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;
    ArchitectureFileEditorDialog dlg(this, &filename);
    dlg.exec();
}

void MainWindow::on_actionCreate_Instruction_List_triggered()
{
    InstructionListFileEditorDialog dlg(this, nullptr);
    dlg.exec();
}


void MainWindow::on_actionEdit_Instruction_List_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Instruction List File", INSTRUCTION_LIST_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;
    InstructionListFileEditorDialog dlg(this, &filename);
    dlg.exec();
}


void MainWindow::on_actionLoad_Instruction_List_triggered()
{
    if(mLoadedInstructionListFile->text()!="N/A") {
        QMessageBox::critical(this, "Error Loading Instruction List File","There is an istruction list that is already loaded.\n To load an instruction list file, reset the algorithm.");
        return;
    }

    if(!checkNotRunning()) {
        QMessageBox::critical(this, "Error Loading Instruction List File","Algorithm is already running.\n To load an instruction list file, reset the algorithm.");
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Select Instruction List File", INSTRUCTION_LIST_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;

    loadInstructionList(filename);
}


void MainWindow::on_actionCreate_Script_triggered()
{

}


void MainWindow::on_actionEdit_Script_triggered()
{

}


void MainWindow::on_actionLoad_Script_triggered()
{
    if(mLoadedScriptFile->text()!="N/A") {
        QMessageBox::critical(this, "Error Loading Script File","There is a script that is already loaded.\n To load an script file, reset the algorithm.");
        return;
    }

    if(!checkNotRunning()) {
        QMessageBox::critical(this, "Error Loading Script File","Algorithm is already running.\n To load a script file, reset the algorithm.");
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Select Script File", SCRIPT_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;

    loadScript(filename);
}

void MainWindow::loadScript(QString filename)
{
    if(mInstructionList->length()==0){
        QMessageBox::critical(this, "Error Opening Script File", "Must load instruction list before a script.");
        return;
    }

    QFile inFile(filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error Opening Script File", "Could not open file:\n" + filename);
        return;
    }

    mScriptInstructionList->clear();
    QTextStream inFileStream(&inFile);
    QString iString;
    ScriptInstruction *scrInst;
    while(!inFileStream.atEnd()){
        iString = inFileStream.readLine();
        try {
            scrInst = StringToScriptInstruction(iString);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Reading In Script Instruction", e);
        }

        for (int i = 0; i<mInstructionList->length(); i++) {
            if(mInstructionList->at(i).mName == scrInst->mInstructionName){
                scrInst->mInstruction = mInstructionList->at(i);
                mScriptInstructionList->append(scrInst);
                break;
            }
            if(i == mInstructionList->length()-1){
                QMessageBox::critical(this, "Error Bad Script Import", "Instruction could not be found in instruction list: " + scrInst->mInstructionName);
                mScriptInstructionList->clear();
                return;
            }
        }
    }
    inFile.close();

    mLoadedScriptFile->setText(filename.split("/").last().split(".").first());
    mLoadedScriptFile->setToolTip(filename);

    populateInstructionTable();
    populateExecutionTable();
}

void MainWindow::loadInstructionList(QString filename)
{
    QFile inFile(filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, "Error Opening Instruction List File","Could not open file:\n" + filename);
        return;
    }

    mInstructionList->clear();

    QTextStream inFileStream(&inFile);
    Instruction inst;
    while(!inFileStream.atEnd()){
        QString iString = inFileStream.readLine();
        try {
            inst = StringToInstruction(iString);
        }  catch (QString e) {
            QMessageBox::critical(this, "Error Reading In Instruction", e);
        }

        mInstructionList->append(inst);
    }
    inFile.close();

    mLoadedInstructionListFile->setText(filename.split("/").last().split(".").first());
    mLoadedInstructionListFile->setToolTip(filename);
}

bool MainWindow::checkNotRunning()
{
    if(mTomasuloAlgorithm->getRunStatus()!=TomasuloRunStatus::NotStarted){
        int ret = QMessageBox::warning(this,"Load Error","Algorithm is in progress. Do you wish to reset the algorithm?\nThis will clear all tables.",QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(ret==QMessageBox::No){
            return false;
        }
        clearAllTables();
    }
    else if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::Done){
        int ret = QMessageBox::warning(this,"Load Error","Algorithm is done. Do you wish to reset the algorithm?\nThis will clear all tables.",QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(ret==QMessageBox::No){
            return false;
        }
        clearAllTables();
    }

    return true;
}

void MainWindow::populateInstructionTable()
{
    ui->instructionStatusTableWidget->clear();

    ui->instructionStatusTableWidget->setColumnCount(7);
    ui->instructionStatusTableWidget->setRowCount(1 + mScriptInstructionList->length());
    auto model = ui->instructionStatusTableWidget->model();

    model->setData(model->index(0,0),QStringLiteral("Instruction"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Destination Register"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Source 1 Register"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Source 2 Register"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Issue CC"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Execution Completion CC"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,6),QStringLiteral("Write Result CC"));
    model->setData(model->index(0,6), Qt::AlignCenter, Qt::TextAlignmentRole);

    for (int i = 0; i<mScriptInstructionList->length(); i++) {
        model->setData(model->index(1 + i,0),mScriptInstructionList->at(i)->mInstructionName);
        model->setData(model->index(1 + i,0), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,1),mScriptInstructionList->at(i)->mDestinationRegister);
        model->setData(model->index(1 + i,1), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,2),mScriptInstructionList->at(i)->mSourceOneRegister);
        model->setData(model->index(1 + i,2), Qt::AlignCenter, Qt::TextAlignmentRole);
        model->setData(model->index(1 + i,3),mScriptInstructionList->at(i)->mSourceTwoRegister);
        model->setData(model->index(1 + i,3), Qt::AlignCenter, Qt::TextAlignmentRole);
        if(mScriptInstructionList->at(i)->mIssueClockCycle>=0){
            model->setData(model->index(1 + i,4),mScriptInstructionList->at(i)->mIssueClockCycle);
            model->setData(model->index(1 + i,4), Qt::AlignCenter, Qt::TextAlignmentRole);
        }
        if(mScriptInstructionList->at(i)->mExecutionCompletionClockCycle>=0){
            model->setData(model->index(1 + i,5),mScriptInstructionList->at(i)->mExecutionCompletionClockCycle);
            model->setData(model->index(1 + i,5), Qt::AlignCenter, Qt::TextAlignmentRole);
        }
        if(mScriptInstructionList->at(i)->mWriteResultClockCycle>=0){
            model->setData(model->index(1 + i,6),mScriptInstructionList->at(i)->mWriteResultClockCycle);
            model->setData(model->index(1 + i,6), Qt::AlignCenter, Qt::TextAlignmentRole);
        }
    }
}

void MainWindow::populateExecutionTable()
{
    ui->executionTableWidget->clear();

    ui->executionTableWidget->setColumnCount(6);
    ui->executionTableWidget->setRowCount(1 + mScriptInstructionList->length());
    auto model = ui->executionTableWidget->model();

    model->setData(model->index(0,0),QStringLiteral("Instruction"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Issue"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Execution"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Read Access"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Write on CDB"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Commit"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);

    for (int i = 0; i<mScriptInstructionList->length(); i++) {
        model->setData(model->index(1 + i,0),mScriptInstructionList->at(i)->mInstructionName+" "+mScriptInstructionList->at(i)->mDestinationRegister+" "+mScriptInstructionList->at(i)->mSourceOneRegister+" "+mScriptInstructionList->at(i)->mSourceTwoRegister);
        model->setData(model->index(1 + i,0), Qt::AlignCenter, Qt::TextAlignmentRole);

        if(mScriptInstructionList->at(i)->mIssueClockCycle>=0){
            model->setData(model->index(1 + i,1),mScriptInstructionList->at(i)->mIssueClockCycle);
            model->setData(model->index(1 + i,1), Qt::AlignCenter, Qt::TextAlignmentRole);
        }

        if(mScriptInstructionList->at(i)->mExecutionStartClockCycle>=0 && mScriptInstructionList->at(i)->mExecutionCompletionClockCycle>=0){
                    model->setData(model->index(1 + i,2),QString::number(mScriptInstructionList->at(i)->mExecutionStartClockCycle)+"-"+QString::number(mScriptInstructionList->at(i)->mExecutionCompletionClockCycle));
                    model->setData(model->index(1 + i,2), Qt::AlignCenter, Qt::TextAlignmentRole);
        }
        else if(mScriptInstructionList->at(i)->mExecutionStartClockCycle>=0){
            model->setData(model->index(1 + i,2),mScriptInstructionList->at(i)->mExecutionStartClockCycle);
            model->setData(model->index(1 + i,2), Qt::AlignCenter, Qt::TextAlignmentRole);
        }

        if(mScriptInstructionList->at(i)->mReadAccessClockCycle>=0){
            model->setData(model->index(1 + i,3),mScriptInstructionList->at(i)->mReadAccessClockCycle);
            model->setData(model->index(1 + i,3), Qt::AlignCenter, Qt::TextAlignmentRole);
        }

        if(mScriptInstructionList->at(i)->mWriteResultClockCycle>=0){
            model->setData(model->index(1 + i,4),mScriptInstructionList->at(i)->mWriteResultClockCycle);
            model->setData(model->index(1 + i,4), Qt::AlignCenter, Qt::TextAlignmentRole);
        }

        if(mScriptInstructionList->at(i)->mCommitClockCycle>=0){
            model->setData(model->index(1 + i,5),mScriptInstructionList->at(i)->mCommitClockCycle);
            model->setData(model->index(1 + i,5), Qt::AlignCenter, Qt::TextAlignmentRole);
        }
    }
}

void MainWindow::clearAllTables()
{
    mGeneralFunctionalUnitList->clear();
    mMemoryFunctionalUnitList->clear();
    mRegisterFunctionalUnitList->clear();
    mCommonDataBusFunctionalUnitList->clear();
    mScriptInstructionList->clear();
    mInstructionList->clear();

    mLoadedArchitectureFile->setText("N/A");
    mLoadedInstructionListFile->setText("N/A");
    mLoadedScriptFile->setText("N/A");
    mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::NotStarted);
    ui->clockCountLabelStatusTab->setText(QString::number(mTomasuloAlgorithm->clockCycle()));

    populateFunctionalUnitReservationTable();
    populateCommonDataBusAndRegisterTable();
    populateMemoryReservationTable();
    populateInstructionTable();
    populateExecutionTable();
}

void MainWindow::on_actionClose_Application_triggered()
{
    this->close();
}

void MainWindow::unloadFiles()
{
    clearAllTables();
}

void MainWindow::resetAlgorithm()
{
    if(mLoadedArchitectureFile->text()=="N/A" ||mLoadedInstructionListFile->text()=="N/A" ||mLoadedScriptFile->text()=="N/A"){
        QMessageBox::critical(this,"Error Resetting Algorithm","Atleast one file has not been imported.\nTherefore the algorithm should not be running and cannot be reset.");
        return;
    }
    if(mTomasuloAlgorithm->getRunStatus()==TomasuloRunStatus::NotStarted){
        QMessageBox::critical(this,"Error Resetting Algorithm","Algorithm has not been started so it cannot be reset.");
        return;
    }

    QString archFile, instListFile, scriptFile;
    archFile = mLoadedArchitectureFile->toolTip();
    instListFile = mLoadedInstructionListFile->toolTip();
    scriptFile = mLoadedScriptFile->toolTip();
    mTomasuloAlgorithm->reset();
    clearAllTables();

    loadArchitecture(archFile);
    loadInstructionList(instListFile);
    loadScript(scriptFile);
}

void MainWindow::on_actionUnload_Files_triggered()
{
    unloadFiles();
}


void MainWindow::on_actionReset_Algorithm_triggered()
{
    resetAlgorithm();
    ui->actionReset_Algorithm->setEnabled(false);
}


void MainWindow::on_actionLoad_Files_triggered()
{
    on_actionLoad_Architecture_triggered();
    on_actionLoad_Instruction_List_triggered();
    on_actionLoad_Script_triggered();
}

void MainWindow::on_actionStep_Algorithm_triggered()
{
    individualStep();
}

void MainWindow::on_actionPause_Algorithm_triggered()
{
    pauseClock();
}


void MainWindow::on_actionManual_Step_Algorithm_triggered()
{
    mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::ManualStep);
}


void MainWindow::on_actionClock_Step_Algorithm_triggered()
{
    bool ok;
    float val = QInputDialog::getDouble(this,"Clock Speed","What would you like the clock cycle to be?",1.00,0.1,5,2,&ok);
    if(!ok){
        return;
    }
    mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::ClockStep);

    mRunClock->setInterval(val*1000);
    mRunClock->start();
}


void MainWindow::on_actionFull_Speed_Algorithm_triggered()
{
    mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::AutomaticStep);
    this->fullSpeedStep();
}


void MainWindow::on_actionPrerun_Check_triggered()
{
    if(mLoadedArchitectureFile->text()=="N/A" || mLoadedInstructionListFile->text()=="N/A" || mLoadedScriptFile->text()=="N/A"){
        QMessageBox::critical(this,"Prerun Check Error","Atleast one file has not been loaded.\nNeeded:\nArchitecture File\nInstruction List File\nScript File");
        return;
    }

    if(mTomasuloAlgorithm->getRunStatus()!=TomasuloRunStatus::NotStarted){
        QMessageBox::critical(this,"Prerun Check Error","Algorithm is currently in progress, it must be reset.");
        return;
    }

    mTomasuloAlgorithm->setRunStatus(TomasuloRunStatus::PrerunCheckComplete);
    QMessageBox::information(this,"Prerun Check Completion","Prerun check is clear.");
}

void MainWindow::updateRunStatusOptions()
{
    switch (mTomasuloAlgorithm->getRunStatus()) {
    case TomasuloRunStatus::NotStarted:
        ui->actionPrerun_Check->setEnabled(true);
        ui->actionManual_Step_Algorithm->setEnabled(false);
        ui->actionClock_Step_Algorithm->setEnabled(false);
        ui->actionFull_Speed_Algorithm->setEnabled(false);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(false);
        break;
    case TomasuloRunStatus::PrerunCheckComplete:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(false);
        break;
    case TomasuloRunStatus::ManualStep:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionStep_Algorithm->setEnabled(true);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(true);
        break;
    case TomasuloRunStatus::ClockStep:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(false);
        ui->actionClock_Step_Algorithm->setEnabled(false);
        ui->actionFull_Speed_Algorithm->setEnabled(false);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(true);
        ui->actionReset_Algorithm->setEnabled(true);
        break;
    case TomasuloRunStatus::AutomaticStep:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(false);
        ui->actionClock_Step_Algorithm->setEnabled(false);
        ui->actionFull_Speed_Algorithm->setEnabled(false);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(false);
        break;
    case TomasuloRunStatus::Paused:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(true);
        ui->actionClock_Step_Algorithm->setEnabled(true);
        ui->actionFull_Speed_Algorithm->setEnabled(true);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(true);
        break;
    case TomasuloRunStatus::Done:
        ui->actionPrerun_Check->setEnabled(false);
        ui->actionManual_Step_Algorithm->setEnabled(false);
        ui->actionClock_Step_Algorithm->setEnabled(false);
        ui->actionFull_Speed_Algorithm->setEnabled(false);
        ui->actionStep_Algorithm->setEnabled(false);
        ui->actionPause_Algorithm->setEnabled(false);
        ui->actionReset_Algorithm->setEnabled(true);
        break;
    }
}

