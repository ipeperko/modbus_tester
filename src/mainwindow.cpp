#include "mainwindow.h"
#include "client_tab.h"
#include "server_tab.h"
#include "ui_mainwindow.h"
#include <QDebug>

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Modbus tester");

    client = new client_tab;
    server = new server_tab;

    ui->tabWidget->addTab(client, "Client");
    ui->tabWidget->addTab(server, "Server");

    connect(client, &client_tab::connection_status_changed, this, [this](bool active) {
        update_tab_status(0, active);
    });
    connect(server, &server_tab::connection_status_changed, this, [this](bool active) {
        update_tab_status(1, active);
    });

    update_tab_status(0, false);
    update_tab_status(1, false);
}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::update_tab_status(int index, bool active)
{
    ui->tabWidget->setTabIcon(index, active ? QIcon(":/img/bus.png") : QIcon(":/img/minus.png"));
}