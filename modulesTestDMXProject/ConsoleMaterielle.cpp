#include "ConsoleMaterielle.h"
#include <QDebug>

ConsoleMaterielle::ConsoleMaterielle(QSlider* slider, QObject* parent)
    : QObject(parent), slider(slider)
{
    port = new QSerialPort("COM8", this);
    connect(port, &QSerialPort::readyRead, this, &ConsoleMaterielle::onDataReceived);
    port->setBaudRate(9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (!port->open(QIODevice::ReadWrite)) {
        qWarning() << "Failed to open port" << port->portName() << ", error:" << port->errorString();
    }
}

ConsoleMaterielle::~ConsoleMaterielle()
{
    if (port->isOpen()) {
        port->close();
    }
    delete port;
}

void ConsoleMaterielle::onDataReceived()
{
    if (!port->canReadLine())
        return;

    QByteArray data = port->readLine();

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


