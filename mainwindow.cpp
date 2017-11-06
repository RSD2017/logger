#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    logg(new logger)
{
   // QTimer::singleShot(20, this, SLOT(showFullScreen()));

    debounceTimer = new QTimer(this);
    orderTimer = new QTimer(this);
    timer = new QTimer(this);

    timer->start(1000);
    orderTimer->start(2000);

    ui->setupUi(this);
    //ui->staticOrderLabel->setText("Order:");
    //ui->staticPartLabel->setText("Part:");
    ui->orderLabel->setText("Pending...");
    ui->partLabel->clear();
    ui->ssButton->setText("START");

    ui->goodBox->setMinimum(0);
    ui->goodBox->setMaximum(5);
    ui->badBox->setMinimum(0);
    ui->badBox->setMaximum(5);

    connect(ui->missButton, SIGNAL(clicked()), this, SLOT(missButtonClicked()));
    connect(ui->ssButton, SIGNAL(clicked()), this, SLOT(ssButtonClicked()));
    connect(ui->goodButton, SIGNAL(clicked()), this, SLOT(goodButtonClicked()));
    connect(ui->errorButton, SIGNAL(clicked()), this, SLOT(errorButtonClicked()));
    connect(ui->customErrorButton, SIGNAL(clicked()), this, SLOT(customErrorCuttonClicked()));
    connect(debounceTimer, SIGNAL(timeout()), this, SLOT(clearTimerFlag()));
    connect(orderTimer, SIGNAL(timeout()), this, SLOT(orderUpdateFromMES()));
    connect(timer, SIGNAL(timeout()), this, SLOT(elapsedTime()));

    this->machineRunning = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearTimerFlag()
{
    timerFlag = true;
}

std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void MainWindow::elapsedTime()
{
    int time = etimer.elapsed();

    if ( firstPress )
        time = 0;

    int seconds = (int) (time / 1000) % 60 ;
    int minutes = (int) ((time / (1000*60)) % 60);
    int hours   = (int) ((time / (1000*60*60)) % 24);
    QString timeString = QString::number(hours) + "h " +
                         QString::number(minutes) + "m " +
                         QString::number(seconds) + "s";
    ui->elapsedTime->setText( timeString );
    timer->start(1000);
}


void MainWindow::orderUpdateFromMES()
{
    if(this->machineRunning){
        auto future = std::async(std::launch::async, GetLineFromCin);
        auto line = future.get();

        if (!line.empty()) {
            displayOrder(line);
            new_order = true;
        }
    }
}

void MainWindow::displayOrder(std::string order){
    std::string delimiter = ":";

    size_t pos = 0;
    std::string token;
    std::vector<std::string> parts;

    // Order ID
    std::string orderID = order.substr(0, order.find(delimiter));
    order.erase(0, order.find(delimiter) + delimiter.length());

    QString orderLabel = QString::fromUtf8(orderID.c_str());
    ui->orderLabel->setText(orderLabel);
    currentOrderID = orderID;

    // Part ID and type
    while ((pos = order.find(delimiter)) != std::string::npos) {
        token = order.substr( 0, pos );
        parts.push_back( token );
        order.erase( 0, pos + delimiter.length() );
    }
    parts.push_back( order );

    QString partLabel;
    int i = 0;
    for ( auto &part : parts ) {
        if(part.back() == '1'){
            partLabel += QString::number(i) +  ": GOOD\n";
        }else{
            partLabel += QString::number(i) +  ": BAD\n";
        }
        i++;
    }
    ui->partLabel->setText(partLabel);
    currentPartIDs = parts;
}

void MainWindow::goodButtonClicked()
{
    if (timerFlag && new_order)
    {
        logg->log_order_ok(currentOrderID, true, currentPartIDs);

        ui->errorLine->clear();
        ui->orderLabel->setText("Pending...");
        ui->partLabel->clear();

        debounceTimer->start(1000);
        timerFlag = false;
        new_order = false;
    }
}

void MainWindow::errorButtonClicked()
{
    if (timerFlag && new_order)
    {
        std::string error_msg = ui->errorBox->currentText().toUtf8().constData();

        if (error_msg.compare(0, 3, "Too") == 0) {
            std::string goodCount = ui->goodBox->text().toUtf8().constData();
            std::string badCount = ui->badBox->text().toUtf8().constData();
            error_msg += " (OK=" + goodCount + ", " + "NG=" + badCount + ")";
        }


        logg->log_order_ng(currentOrderID, false, currentPartIDs, error_msg);

        ui->errorLine->clear();
        ui->orderLabel->setText("Pending...");
        ui->partLabel->clear();

        debounceTimer->start(1000);
        timerFlag = false;
        new_order = false;
    }
}

void MainWindow::customErrorCuttonClicked()
{
    if (timerFlag)
    {
        std::string error_msg = ui->errorLine->text().toUtf8().constData();
        if ( !error_msg.empty() ) {
            logg->log_additonal_error( error_msg );

            ui->errorLine->clear();

            debounceTimer->start(1000);
            timerFlag = false;
        }
    }
}

void MainWindow::missButtonClicked()
{
    if (timerFlag)
    {
        std::string error_msg = "missed_part";
        logg->log_additonal_error(error_msg);

        debounceTimer->start(1000);
        timerFlag = false;
    }
}

void MainWindow::ssButtonClicked()
{
    if (timerFlag)
    {
        QString text;
        if(this->machineRunning){
            text = "system_stop";
            ui->ssButton->setText("START");
            this->machineRunning = false;
        } else {
            text = "system_start";
            ui->ssButton->setText("STOP");
            this->machineRunning = true;
            if (firstPress) {
                etimer.start();
                firstPress = false;
            }
        }

        /**
        *   Connect to stuff and stop everything
        **/

        logg->log_additonal_error(text.toUtf8().constData());
        debounceTimer->start(1000);
        timerFlag = false;
    }
}
