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
        // qDebug() << "Slider value changed:" << value;
        });

    connect(consoleMaterielle, &ConsoleMaterielle::previousChannel, this, [this]() {
        // qDebug() << "Joystick moved left";
        });

    connect(consoleMaterielle, &ConsoleMaterielle::nextChannel, this, [this]() {
        // qDebug() << "Joystick moved right";
        });

    connect(consoleMaterielle, &ConsoleMaterielle::confirmButtonPressed, this, [this]() {
        // qDebug() << "Confirm button pressed";
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
        qDebug() << "La sc�ne 'TestScene' existe d�j� dans la base de donn�es.";
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
        qDebug() << "La sc�ne 'TestScene' existe d�j� dans la base de donn�es.";
        return;
    }

    scene->insertScene("TestScene");
    scene->updateScene("TestScene", "NewTestScene");
    // qDebug() << "Modification de la scene en BDD";

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
        // qDebug() << "Connexion au serveur r�ussie";
    }
    else {
        qDebug() << "�chec de la connexion au serveur";
        QFAIL("�chec de la connexion au serveur");
    }

    if (client.isOpen()) {
        // qDebug() << "La connexion est �tablie";
    }
    else {
        qDebug() << "La connexion n'est pas �tablie";
        QFAIL("La connexion n'est pas �tablie");
    }

    QByteArray dmxFrame(512, 0);
    dmxFrame[3] = 255;
    dmxFrame[4] = 128;
    dmxFrame[5] = 64;

    if (client.isWritable()) {
        qint64 bytesWritten = client.write(dmxFrame);
        if (bytesWritten == dmxFrame.size()) {
            // qDebug() << "Trame DMX envoy�e au serveur";
        }
        else {
            qDebug() << "�chec de l'envoi de la trame DMX au serveur";
            QFAIL("�chec de l'envoi de la trame DMX au serveur");
        }
    }
    else {
        qDebug() << "Le client n'est pas pr�t � �crire";
        QFAIL("Le client n'est pas pr�t � �crire");
    }

    client.disconnectFromHost();
    if (client.state() != QAbstractSocket::UnconnectedState) {
        if (client.waitForDisconnected()) {
            // qDebug() << "D�connexion du serveur r�ussie";
        }
        else {
            qDebug() << "�chec de la d�connexion du serveur";
            QFAIL("�chec de la d�connexion du serveur");
        }
    }
}

void TestModulesDMXProject::testSliderValue()
{
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::channelValueChanged);

    // Simulate slider value change
    testSlider->setValue(50);
    emit consoleMaterielle->channelValueChanged(testSlider->value());

    // Ajout d'un d�lai pour permettre au signal d'�tre trait�
    QTest::qWait(100); // Attendre pendant 100 millisecondes

    // V�rifier si le signal a �t� captur�
    QCOMPARE(spy.count(), 1);

    // R�cup�rer la valeur captur�e
    QList<QVariant> args = spy.takeFirst();
    int sliderValue = args.at(0).toInt();
    qDebug() << "Slider value:" << sliderValue;
    QCOMPARE(sliderValue, 50);

    return;
}

void TestModulesDMXProject::testJoystickValue()
{
    QSignalSpy spyLeft(consoleMaterielle, &ConsoleMaterielle::previousChannel);
    QSignalSpy spyRight(consoleMaterielle, &ConsoleMaterielle::nextChannel);

    // Simulate joystick movement
    emit consoleMaterielle->previousChannel();
    QTest::qWait(100); // Attendre pendant 100 millisecondes
    emit consoleMaterielle->nextChannel();
    QTest::qWait(100); // Attendre pendant 100 millisecondes

    // V�rifier si les signaux ont �t� captur�s
    QCOMPARE(spyLeft.count(), 1);
    QCOMPARE(spyRight.count(), 1);

    if (spyLeft.count() == 1) {
        qDebug() << "Joystick moved left";
    }
    else if (spyRight.count() == 1) {
        qDebug() << "Joystick moved right";
    }
    else {
        QFAIL("No joystick movement detected");
    }
    return;
}

void TestModulesDMXProject::testButtonValue()
{
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::confirmButtonPressed);

    // Simulate button press
    emit consoleMaterielle->confirmButtonPressed();

    // Ajout d'un d�lai pour permettre au signal d'�tre trait�
    QTest::qWait(100); // Attendre pendant 100 millisecondes

    // V�rifier si le signal a �t� captur�
    QCOMPARE(spy.count(), 1);
    qDebug() << "Button press detected";

    return;
}





void TestModulesDMXProject::testReceiveSignal()
{
    // Cr�er un QSignalSpy pour surveiller le signal
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::channelValueChanged);

    // Effectuer une action qui devrait d�clencher le signal
    // Par exemple, simuler un changement de canal
    emit consoleMaterielle->channelValueChanged(50);

    // V�rifier si le signal a �t� �mis
    QVERIFY(spy.isValid()); // V�rifie si la connexion au signal a r�ussi
    QCOMPARE(spy.count(), 1); // V�rifie le nombre de fois que le signal a �t� �mis
}
