#include "TestModulesDMXProject.h"
#include <QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QtTest/QTest>
#include <QSignalSpy>

TestModulesDMXProject::TestModulesDMXProject()
    : testSlider(nullptr), consoleMaterielle(nullptr), testObject(nullptr)
{
}

TestModulesDMXProject::~TestModulesDMXProject()
{
}

void TestModulesDMXProject::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "test_connection");
    db.setHostName("192.168.64.213");
    db.setDatabaseName("testCodeDMX");
    db.setUserName("root");
    db.setPassword("root");

    QVERIFY(db.open());

    testObject = new modulesTestDMXProject();
    QVERIFY(testObject != nullptr);

    testObject->show();

    testSlider = new QSlider(Qt::Horizontal);
    consoleMaterielle = new ConsoleMaterielle(testSlider, this);

    connect(consoleMaterielle, &ConsoleMaterielle::channelValueChanged, this, [this](int value) {
        qDebug() << "Slider value changed:" << value;
        });

    connect(consoleMaterielle, &ConsoleMaterielle::previousChannel, this, [this]() {
        qDebug() << "Joystick moved left";
        });

    connect(consoleMaterielle, &ConsoleMaterielle::nextChannel, this, [this]() {
        qDebug() << "Joystick moved right";
        });

    connect(consoleMaterielle, &ConsoleMaterielle::confirmButtonPressed, this, [this]() {
        qDebug() << "Confirm button pressed";
        });
}

void TestModulesDMXProject::cleanupTestCase()
{
    delete testSlider;
    delete consoleMaterielle;

    QSqlDatabase::database().close();
    delete testObject;
    testObject = nullptr;
}

void TestModulesDMXProject::testInsertScene()
{
    deleteScene("TestScene");

    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La scène 'TestScene' existe déjà dans la base de données.";
        deleteScene("TestScene");
        return;
    }

    scene->insertScene("TestScene");

    query.prepare("SELECT * FROM scene WHERE nom = 'TestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("TestScene");
}

void TestModulesDMXProject::testUpdateScene()
{
    deleteScene("TestScene");

    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La scène 'TestScene' existe déjà dans la base de données.";
        return;
    }

    scene->insertScene("TestScene");
    scene->updateScene("TestScene", "NewTestScene");
    qDebug() << "Modification de la scene en BDD";

    query.prepare("SELECT * FROM scene WHERE nom = 'NewTestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("NewTestScene");
}

void TestModulesDMXProject::deleteScene(const QString& sceneName)
{
    QSqlQuery query;
    query.prepare("DELETE FROM scene WHERE nom = :sceneName");
    query.bindValue(":sceneName", sceneName);
    QVERIFY(query.exec());
}

void TestModulesDMXProject::testTcpConnection()
{
    QTcpSocket client;
    client.connectToHost("192.168.64.170", 12345);
    if (client.waitForConnected()) {
        qDebug() << "Connexion au serveur réussie";
    }
    else {
        qDebug() << "Échec de la connexion au serveur";
        QFAIL("Échec de la connexion au serveur");
    }

    if (client.isOpen()) {
        qDebug() << "La connexion est établie";
    }
    else {
        qDebug() << "La connexion n'est pas établie";
        QFAIL("La connexion n'est pas établie");
    }

    QByteArray dmxFrame(512, 0);
    dmxFrame[3] = 255;
    dmxFrame[4] = 128;
    dmxFrame[5] = 64;

    if (client.isWritable()) {
        qint64 bytesWritten = client.write(dmxFrame);
        if (bytesWritten == dmxFrame.size()) {
            qDebug() << "Trame DMX envoyée au serveur";
        }
        else {
            qDebug() << "Échec de l'envoi de la trame DMX au serveur";
            QFAIL("Échec de l'envoi de la trame DMX au serveur");
        }
    }
    else {
        qDebug() << "Le client n'est pas prêt à écrire";
        QFAIL("Le client n'est pas prêt à écrire");
    }

    client.disconnectFromHost();
    if (client.state() != QAbstractSocket::UnconnectedState) {
        if (client.waitForDisconnected()) {
            qDebug() << "Déconnexion du serveur réussie";
        }
        else {
            qDebug() << "Échec de la déconnexion du serveur";
            QFAIL("Échec de la déconnexion du serveur");
        }
    }
}

void TestModulesDMXProject::testSliderValue()
{
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::channelValueChanged);
    QByteArray command = "READ_SLIDER";
    consoleMaterielle->getPort()->write(command); // Utilisez getPort()

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    QList<QVariant> args = spy.takeFirst();
    int sliderValue = args.at(0).toInt();
    qDebug() << "Slider value:" << sliderValue;
}

void TestModulesDMXProject::testJoystickValue()
{
    QSignalSpy spyLeft(consoleMaterielle, &ConsoleMaterielle::previousChannel);
    QSignalSpy spyRight(consoleMaterielle, &ConsoleMaterielle::nextChannel);

    QByteArray command = "READ_JOYSTICK";
    consoleMaterielle->getPort()->write(command); // Utilisez getPort()

    QVERIFY(spyLeft.wait(1000) || spyRight.wait(1000));
    if (spyLeft.count() == 1) {
        qDebug() << "Joystick moved left";
    }
    else if (spyRight.count() == 1) {
        qDebug() << "Joystick moved right";
    }
    else {
        QFAIL("No joystick movement detected");
    }
}

void TestModulesDMXProject::testButtonValue()
{
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::confirmButtonPressed);
    QByteArray command = "READ_BUTTON";
    consoleMaterielle->getPort()->write(command); // Utilisez getPort()

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);
    qDebug() << "Button press detected";
}
