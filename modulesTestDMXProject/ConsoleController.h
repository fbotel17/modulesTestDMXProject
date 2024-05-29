#pragma once

#ifndef CONSOLECONTROLLER_H
#define CONSOLECONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QSerialPortInfo>

class ConsoleController : public QObject
{
    Q_OBJECT

public:
    explicit ConsoleController(QObject* parent = nullptr);
    ~ConsoleController();
    bool connectToArduino(const QString& portName);
    void disconnectFromArduino();
    bool isConnected() const;
    void sendSceneNames(const QStringList& scenes);
    void sendData(const QByteArray& data); // Déclaration de la méthode sendData



public slots:
    void processSerialData();

signals:
    void sceneAddRequested();
    void sceneEditRequested();
    void sceneDeleteRequested();
    void sendSceneNamesRequested(const QStringList& scenes);

private:
    QSerialPort m_serialPort;
};

#endif // CONSOLECONTROLLER_H



