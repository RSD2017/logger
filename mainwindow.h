#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <chrono>
#include <future>

#include "logger.h"

#include <QElapsedTimer>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void ssButtonClicked();
        void goodButtonClicked();
        void errorButtonClicked();
        void customErrorCuttonClicked();
        void clearTimerFlag();
        void orderUpdateFromMES();
        void missButtonClicked();
        void elapsedTime();

        void displayOrder(std::string);

    private:
        Ui::MainWindow *ui;

        logger *logg;

        QTimer *debounceTimer;
        QTimer *orderTimer;
        QTimer *timer;
        QElapsedTimer etimer;
        bool timerFlag = true;
        bool machineRunning = false;
        bool new_order = false;
        bool firstPress = true;

        std::string currentOrderID;
        std::vector<std::string> currentPartIDs;
};

#endif // MAINWINDOW_H
