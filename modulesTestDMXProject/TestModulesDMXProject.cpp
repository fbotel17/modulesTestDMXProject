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
    // ID de scène prédéfini pour les tests
    int predefinedSceneId = 45;

    // Convertir l'ID de la scène en QByteArray
    QByteArray sceneIdData = QByteArray::number(predefinedSceneId);

    // Envoyer l'ID de la scène au serveur Linux en utilisant une connexion TCP
    QTcpSocket socket;
    socket.connectToHost("192.168.64.170", 12345); // Remplacez par l'adresse IP et le port de votre serveur Linux
    if (socket.waitForConnected()) {
        socket.write(sceneIdData);
        socket.waitForBytesWritten();
        socket.disconnectFromHost();
        qDebug() << "ID de la scène envoyé au serveur :" << predefinedSceneId;
    }
    else {
        qDebug() << "Erreur : impossible de se connecter au serveur.";
        QFAIL("Erreur : impossible de se connecter au serveur.");
    }

    // Simulation de connexion directe pour vérification
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
    if (!consoleMaterielle->isPortOpen()) {
        QFAIL("Arduino not connected");
        return;
    }

    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::channelValueChanged);

    // Ajout d'un délai pour permettre la lecture des données série
    QTest::qWait(100); // Attendre pendant 100 millisecondes

    // vérifie qu'au moins un signal a été capturé
    QVERIFY(spy.count() > 0);

    bool valueFound = false;
    for (int i = 0; i < spy.count(); ++i) {
        QList<QVariant> args = spy.takeAt(i);
        int sliderValue = args.at(0).toInt();
        qDebug() << "Slider value:" << sliderValue;

        if (sliderValue >= 0 && sliderValue <= 255) {
            valueFound = true;
            break;
        }
    }

    // Vérifiez qu'au moins une valeur valide a été trouvée
    QVERIFY(valueFound);
}




void TestModulesDMXProject::testJoystickValue()
{
    if (!consoleMaterielle->isPortOpen()) {
        QFAIL("Arduino not connected");
        return;
    }

    QSignalSpy spyLeft(consoleMaterielle, &ConsoleMaterielle::previousChannel);
    QSignalSpy spyRight(consoleMaterielle, &ConsoleMaterielle::nextChannel);

    // Attendre un moment pour que la lecture série soit effectuée
    QTest::qWait(1000); // Attendre pendant 100 millisecondes

    // Appeler manuellement onDataReceived pour traiter les données série
    consoleMaterielle->onDataReceived();

    // Vérifier si les signaux ont été capturés plus d'une fois
    QVERIFY(spyLeft.count() > 1 || spyRight.count() > 1);

    if (spyLeft.count() > 1) {
        qDebug() << "Joystick moved left, count:" << spyLeft.count();
    }
    else if (spyRight.count() > 1) {
        qDebug() << "Joystick moved right, count:" << spyRight.count();
    }
    else {
        QFAIL("No joystick movement detected");
    }
    return;
}



void TestModulesDMXProject::testButtonValue()
{
    if (!consoleMaterielle->isPortOpen()) {
        QFAIL("Arduino not connected");
        return;
    }

    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::confirmButtonPressed);

    // Attendre un moment pour que la lecture série soit effectuée
    QTest::qWait(1000); // Attendre pendant 100 millisecondes

    // Appeler manuellement onDataReceived pour traiter les données série
    consoleMaterielle->onDataReceived();

    // Vérifier si le signal a été capturé plus d'une fois
    QVERIFY(spy.count() > 1);
    qDebug() << "Button press detected, count:" << spy.count();

    return;
}







void TestModulesDMXProject::testReceiveSignal()
{
    // Créer un QSignalSpy pour surveiller le signal
    QSignalSpy spy(consoleMaterielle, &ConsoleMaterielle::channelValueChanged);

    // Effectuer une action qui devrait déclencher le signal
    // Par exemple, simuler un changement de canal
    emit consoleMaterielle->channelValueChanged(50);

    // Vérifier si le signal a été émis
    QVERIFY(spy.isValid()); // Vérifie si la connexion au signal a réussi
    QCOMPARE(spy.count(), 1); // Vérifie le nombre de fois que le signal a été émis
}
