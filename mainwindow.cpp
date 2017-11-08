#include "mainwindow.h"

std::vector<std::string> buffer;
QMutex mutex;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    logg(new logger)
{
    debounceTimer = new QTimer(this);
    timer = new QTimer(this);

    timer->start(250);

    QThread* thread = new QThread;
    myThread* bufferThread = new myThread( &mutex, &buffer );
    bufferThread->moveToThread(thread);
    thread->start();

    ui->setupUi(this);
    ui->orderLabel->setText("Pending...");
    ui->partLabel->clear();
    ui->ssButton->setText("START");

    ui->goodBox->setMinimum(0);
    ui->goodBox->setMaximum(10);
    ui->badBox->setMinimum(0);
    ui->badBox->setMaximum(10);

    connect(ui->ssButton, SIGNAL(clicked()), this, SLOT(ssButtonClicked()));
    connect(ui->goodButton, SIGNAL(clicked()), this, SLOT(goodButtonClicked()));
    connect(ui->errorButton, SIGNAL(clicked()), this, SLOT(errorButtonClicked()));
    connect(ui->customErrorButton, SIGNAL(clicked()), this, SLOT(customErrorCuttonClicked()));
    connect(debounceTimer, SIGNAL(timeout()), this, SLOT(clearTimerFlag()));
    connect(timer, SIGNAL(timeout()), this, SLOT(elapsedTime()));
    connect(timer, SIGNAL(timeout()), bufferThread, SLOT(process()));
    connect(bufferThread, SIGNAL(newOrder()), this, SLOT(orderUpdateFromMES()));
    connect(timer, SIGNAL(timeout()), this, SLOT(orderUpdateFromMES()));

    this->machineRunning = false;
    this->readyForNextOrder = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearTimerFlag()
{
    timerFlag = true;
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
    ui->missingLabel->setText(QString::number(buffer.size()));
    if( this->machineRunning && this->readyForNextOrder ){
        std::string order;
        mutex.lock(); // To avoid collision
        usleep(10);
        if (!buffer.empty()) {
            order = buffer.front();
            buffer.erase(buffer.begin());
        }
        mutex.unlock(); // Let's release the lock
        usleep(1);
        if (!order.empty()) {
            displayOrder( order );
            readyForNextOrder = false;
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

    numberOfParts = parts.size();

    QString partLabel;
    int goodParts = 0, badParts = 0;
    for ( auto &part : parts ) {
        if(part.back() == '1'){
            goodParts++;
        }else{
            badParts++;
        }
    }
    partLabel += "GOOD: " + QString::number(goodParts) +  "\n";
    partLabel += "BAD: " + QString::number(badParts) +  "\n";
    ui->partLabel->setText(partLabel);
    currentPartIDs = parts;
}

void MainWindow::goodButtonClicked()
{
    if ( timerFlag && new_order )
    {
        logg->log_order_ok(currentOrderID, true, currentPartIDs);

        ui->errorLine->clear();
        ui->orderLabel->setText("Pending...");
        ui->partLabel->clear();

        debounceTimer->start(1000);
        timerFlag = false;
        new_order = false;
        readyForNextOrder = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        goodButtonClicked();
    }
}

void MainWindow::errorButtonClicked()
{
    if (timerFlag && new_order)
    {
        std::string error_msg;

        int goodParts = std::stoi(ui->goodBox->text().toUtf8().constData());
        int badParts = std::stoi(ui->badBox->text().toUtf8().constData());

        if ( numberOfParts == 0 ) {
            error_msg += "no parts";
        } else if ( numberOfParts > goodParts + badParts ) {
            error_msg += "too few parts";
        } else if ( numberOfParts < goodParts + badParts ) {
            error_msg += "too many parts";
        } else {
            error_msg += "right amount, wrong parts";
        }

        std::string goodCount = ui->goodBox->text().toUtf8().constData();
        std::string badCount = ui->badBox->text().toUtf8().constData();

        error_msg += " (OK=" + goodCount + ", " + "NG=" + badCount + ")";

        logg->log_order_ng(currentOrderID, false, currentPartIDs, error_msg);

        ui->errorLine->clear();
        ui->orderLabel->setText("Pending...");
        ui->partLabel->clear();

        ui->badBox->setValue(0);
        ui->goodBox->setValue(0);

        debounceTimer->start(1000);
        timerFlag = false;
        new_order = false;
        readyForNextOrder = true;
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
