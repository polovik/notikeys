#include <QLocalSocket>
#include <QByteArray>
#include "mainwindow.h"
#include "../../../gui/ExternalPluginProtocol.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect()
{
    m_socket.connectToServer(SERVER_SOCKET_NAME);
}

void MainWindow::send()
{
    pluginPacket_s packet;
    packet.preamble = PLUGIN_PACKET_PREAMBLE;
    packet.uid = 3;
    packet.eventsCount = 1;
    m_socket.write((char *)&packet, PLUGIN_PACKET_LENGTH);
}
