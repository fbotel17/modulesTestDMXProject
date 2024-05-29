#include "TestModulesDMXProject.h"
#include <QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QtTest/QTest>

TestModulesDMXProject::TestModulesDMXProject()
{
}

TestModulesDMXProject::~TestModulesDMXProject()
{
}

void TestModulesDMXProject::initTestCase()
{
    // Initialiser la base de données et l'IHM pour les tests
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "test_connection"); // Donner un nom unique à la connexion
    db.setHostName("192.168.64.213");
    db.setDatabaseName("testCodeDMX");
    db.setUserName("root");
    db.setPassword("root");

    QVERIFY(db.open());

    // Créer une instance de votre classe principale
    testObject = new modulesTestDMXProject();
    QVERIFY(testObject != nullptr);

    // Afficher la fenêtre principale pour interagir avec l'IHM
    testObject->show();
}

void TestModulesDMXProject::cleanupTestCase()
{
    // Nettoyer la base de données et fermer l'IHM après les tests
    QSqlDatabase::database().close();

    // Supprimer l'instance de la classe principale
    delete testObject;
    testObject = nullptr;
}

void TestModulesDMXProject::testInsertScene()
{
    deleteScene("TestScene");

    // Vérifier si la scène existe déjà dans la base de données
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La scène 'TestScene' existe déjà dans la base de données.";
        deleteScene("TestScene"); // Supprimer la scène même si elle existe déjà
        return;
    }

    // Appeler la méthode insertScene avec un nom de scène de test
    scene->insertScene("TestScene");


    // Vérifier si la scène a été insérée dans la base de données
    query.prepare("SELECT * FROM scene WHERE nom = 'TestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("TestScene");
}


void TestModulesDMXProject::testUpdateScene() {

    deleteScene("TestScene");

    // Ne pas supprimer la scène avant de vérifier si elle existe
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La scène 'TestScene' existe déjà dans la base de données.";
        return;
    }

    // Appeler la méthode insertScene avec un nom de scène de test
    scene->insertScene("TestScene");

    scene->updateScene("TestScene", "NewTestScene");
    qDebug() << "Modification de la scene en BDD";

    query.prepare("SELECT * FROM scene WHERE nom = 'NewTestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("NewTestScene"); // Supprimer la scène mise à jour
}

void TestModulesDMXProject::deleteScene(const QString& sceneName) {
    QSqlQuery query;
    query.prepare("DELETE FROM scene WHERE nom = :sceneName");
    query.bindValue(":sceneName", sceneName);
    QVERIFY(query.exec());
}

void TestModulesDMXProject::testTcpConnection()
{
    // Créer un client TCP temporaire pour les tests
    QTcpSocket client;

    // Tenter de se connecter au serveur
    client.connectToHost("192.168.64.170", 12345); // Remplacez 12345 par le numéro de port de votre serveur
    if (client.waitForConnected()) {
        qDebug() << "Connexion au serveur réussie";
    }
    else {
        qDebug() << "Échec de la connexion au serveur";
        QFAIL("Échec de la connexion au serveur");
    }

    // Vérifier si la connexion est établie
    if (client.isOpen()) {
        qDebug() << "La connexion est établie";
    }
    else {
        qDebug() << "La connexion n'est pas établie";
        QFAIL("La connexion n'est pas établie");
    }

    QByteArray dmxFrame(512, 0);

    // Remplir la trame DMX avec des valeurs de test (par exemple, canal 1 : 255, canal 2 : 128, canal 3 : 64)
    dmxFrame[3] = 255;
    dmxFrame[4] = 128;
    dmxFrame[5] = 64;
    dmxFrame[6] = 0;
    dmxFrame[7] = 20;
    dmxFrame[8] = 10;
    dmxFrame[9] = 240;
    dmxFrame[10] = 0;
    dmxFrame[11] = 155;
    dmxFrame[12] = 45;
    dmxFrame[13] = 0;
    dmxFrame[14] = 0;

    // Envoyer la trame DMX au serveur
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

    // Fermer la connexion
    client.disconnectFromHost();

    // Attendre que la déconnexion soit effective uniquement si le socket était connecté
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




