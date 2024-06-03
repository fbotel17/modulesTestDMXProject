#ifndef CONSOLEMATERIELLE_H
#define CONSOLEMATERIELLE_H

#include <QObject>
#include <QSerialPort>
#include <QSlider>

class ConsoleMaterielle : public QObject
{
    Q_OBJECT

public:
    ConsoleMaterielle(QSlider* slider, QObject* parent = nullptr);
    ~ConsoleMaterielle();
    void sendCommand(const QByteArray& command);
    bool isPortOpen() const;
    QSerialPort* getPort() const; // Ajoutez cette ligne
    bool connectToArduino(const QString& portName);
    int getLastPotentiometerValue() const;

signals:
    void channelValueChanged(int value);
    void previousChannel();
    void nextChannel();
    void confirmButtonPressed();

public slots:
    void onDataReceived();

private:
    QSerialPort m_serialPort;
    QSerialPort* port;
    QSlider* slider;
    int lastPotentiometerValue;
};

#endif // CONSOLEMATERIELLE_H