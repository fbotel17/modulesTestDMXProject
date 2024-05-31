#include "ConsoleMaterielle.h"
#include <QDebug>

ConsoleMaterielle::ConsoleMaterielle(QSlider* slider, QObject* parent)
    : QObject(parent), slider(slider)
{
    /*port = new QSerialPort("COM8", this);
    connect(port, &QSerialPort::readyRead, this, &ConsoleMaterielle::onDataReceived);
    port->setBaudRate(9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);*/



    
}

ConsoleMaterielle::~ConsoleMaterielle()
{
    
}

bool ConsoleMaterielle::connectToArduino(const QString& portName)
{
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(QSerialPort::Baud9600);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to Arduino on" << portName;
        return true;
    }
    else {
        qDebug() << "Failed to connect to Arduino on" << portName;
        return false;
    }
}

void ConsoleMaterielle::sendCommand(const QByteArray& command)
{
    if (port->isOpen()) {
        port->write(command);
    }
}

bool ConsoleMaterielle::isPortOpen() const
{
    return port->isOpen();
}

QSerialPort* ConsoleMaterielle::getPort() const
{
    return port;
}

void ConsoleMaterielle::onDataReceived()
{
    while (port->canReadLine()) {
        QByteArray data = port->readLine();
        //qDebug() << "Received data from Arduino:" << data;

        if (data.startsWith("V")) { // Valeur potentiomètre
            QString str = QString::fromUtf8(data);
            QStringRef substr(&str, 1, str.length() - 1);
            QString valueStr = substr.toString();

            int val = valueStr.toInt();
            double dval = val / 1024.0 * 255.0;
            val = static_cast<int>(dval);

            emit channelValueChanged(val);

            if (slider) {
                slider->setValue(val);
            }
        }
        else if (data.startsWith("J")) { // Mouvement du joystick
            QString str = QString::fromUtf8(data);
            if (str.contains("LEFT")) {
                emit previousChannel();
            }
            else if (str.contains("RIGHT")) {
                emit nextChannel();
            }
        }
        else if (data.startsWith("CONFIRM")) { // Bouton de confirmation
            emit confirmButtonPressed();
        }
    }
}
