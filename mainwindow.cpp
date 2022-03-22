#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeWindow();
}

MainWindow::~MainWindow()
{
    // Delete status bar widgets
    mStatusBarWidget->deleteLater();

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

    // Initialize status bar
    mStatusBarWidget = new QWidget(this);
    ui->statusbar->addWidget(mStatusBarWidget,1);

    mStatusBarLayout = new QHBoxLayout(mStatusBarWidget);

    QSpacerItem* statusBarSpacer = new QSpacerItem(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    mStatusBarLayout->addItem(statusBarSpacer);

    mStatusBarStartPauseButton = new QPushButton("Start");
    mStatusBarLayout->addWidget(mStatusBarStartPauseButton);

    mStatusBarCancelButton = new QPushButton("Cancel");
    mStatusBarLayout->addWidget(mStatusBarCancelButton);

    // Initialize tables in status tab
    // Instruction status table columns: label, instruction, destination register, source 1 register, source 2 register, issue clock cycle, execution complete clock cycle, write result clock cycle
    ui->instructionStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->instructionStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->instructionStatusTableWidget->setColumnCount(8);
    ui->instructionStatusTableWidget->setRowCount(1);
    auto model = ui->instructionStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Label"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Instruction"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Destination Register"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("Source 1 Register"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("Source 2 Register"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("Issue CC"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,6),QStringLiteral("Execution Completion CC"));
    model->setData(model->index(0,6), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,7),QStringLiteral("Write Result CC"));
    model->setData(model->index(0,7), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Functional unit reservation status columns: time, name, busy, operation, source 1 register, FU for source 1 register, source 2 register, FU for source 2 register
    ui->functionalUnitReservationStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->functionalUnitReservationStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->functionalUnitReservationStatusTableWidget->setColumnCount(8);
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
    model->setData(model->index(0,4),QStringLiteral("Source 1 Register"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("FU for Source 1 Register"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,6),QStringLiteral("Source 2 Register"));
    model->setData(model->index(0,6), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,7),QStringLiteral("Fu for Source 2 Register"));
    model->setData(model->index(0,7), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Memory reservation status columns: name, busy, address
    ui->memoryReservationStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->memoryReservationStatusTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->memoryReservationStatusTableWidget->setColumnCount(3);
    ui->memoryReservationStatusTableWidget->setRowCount(1);
    model = ui->memoryReservationStatusTableWidget->model();
    model->setData(model->index(0,0),QStringLiteral("Name"));
    model->setData(model->index(0,0), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,1),QStringLiteral("Busy"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("Address"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);

    // Register hold table has column for all registers, row is labeled 'Functional Unit'
    ui->registerHoldTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->registerHoldTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->registerHoldTableWidget->setColumnCount(6);
    ui->registerHoldTableWidget->setRowCount(2);
    model = ui->registerHoldTableWidget->model();
    model->setData(model->index(0,1),QStringLiteral("F0"));
    model->setData(model->index(0,1), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,2),QStringLiteral("F1"));
    model->setData(model->index(0,2), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,3),QStringLiteral("F2"));
    model->setData(model->index(0,3), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,4),QStringLiteral("F3"));
    model->setData(model->index(0,4), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(0,5),QStringLiteral("F4"));
    model->setData(model->index(0,5), Qt::AlignCenter, Qt::TextAlignmentRole);
    model->setData(model->index(1,0),QStringLiteral("Funcitonal Unit"));
    model->setData(model->index(1,0), Qt::AlignCenter, Qt::TextAlignmentRole);
}

void MainWindow::on_actionCreate_Architecture_triggered()
{
    ArchitectureFileEditorDialog dlg(this);
    dlg.exec();
}


void MainWindow::on_actionLoad_Architecture_triggered()
{

}

void MainWindow::on_actionEdit_Architecture_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Architecture File", ARCHITECTURE_FILES_DIRECTORY_PATH, "Text Files (*.txt)");
    if(filename.isEmpty()) return;
    ArchitectureFileEditorDialog dlg(this, &filename);
    dlg.exec();
}
