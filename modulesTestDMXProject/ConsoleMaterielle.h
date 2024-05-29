#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSlider>
#include <QDebug>

class ConsoleMaterielle : public QObject
{
    Q_OBJECT

public:
    ConsoleMaterielle(QSlider* slider, QObject* parent = nullptr);
    ~ConsoleMaterielle();

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
