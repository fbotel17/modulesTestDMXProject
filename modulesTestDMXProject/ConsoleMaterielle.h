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

signals:
    void channelValueChanged(int value);
    void previousChannel();
    void nextChannel();
    void confirmButtonPressed();

private slots:
    void onDataReceived();

private:
    QSerialPort* port;
    QSlider* slider;
};

#endif // CONSOLEMATERIELLE_H