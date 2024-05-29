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
    // Initialiser la base de donn�es et l'IHM pour les tests
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "test_connection"); // Donner un nom unique � la connexion
    db.setHostName("192.168.64.213");
    db.setDatabaseName("testCodeDMX");
    db.setUserName("root");
    db.setPassword("root");

    QVERIFY(db.open());

    // Cr�er une instance de votre classe principale
    testObject = new modulesTestDMXProject();
    QVERIFY(testObject != nullptr);

    // Afficher la fen�tre principale pour interagir avec l'IHM
    testObject->show();
}

void TestModulesDMXProject::cleanupTestCase()
{
    // Nettoyer la base de donn�es et fermer l'IHM apr�s les tests
    QSqlDatabase::database().close();

    // Supprimer l'instance de la classe principale
    delete testObject;
    testObject = nullptr;
}

void TestModulesDMXProject::testInsertScene()
{
    deleteScene("TestScene");

    // V�rifier si la sc�ne existe d�j� dans la base de donn�es
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La sc�ne 'TestScene' existe d�j� dans la base de donn�es.";
        deleteScene("TestScene"); // Supprimer la sc�ne m�me si elle existe d�j�
        return;
    }

    // Appeler la m�thode insertScene avec un nom de sc�ne de test
    scene->insertScene("TestScene");


    // V�rifier si la sc�ne a �t� ins�r�e dans la base de donn�es
    query.prepare("SELECT * FROM scene WHERE nom = 'TestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("TestScene");
}


void TestModulesDMXProject::testUpdateScene() {

    deleteScene("TestScene");

    // Ne pas supprimer la sc�ne avant de v�rifier si elle existe
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La sc�ne 'TestScene' existe d�j� dans la base de donn�es.";
        return;
    }

    // Appeler la m�thode insertScene avec un nom de sc�ne de test
    scene->insertScene("TestScene");

    scene->updateScene("TestScene", "NewTestScene");
    qDebug() << "Modification de la scene en BDD";

    query.prepare("SELECT * FROM scene WHERE nom = 'NewTestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("NewTestScene"); // Supprimer la sc�ne mise � jour
}

void TestModulesDMXProject::deleteScene(const QString& sceneName) {
    QSqlQuery query;
    query.prepare("DELETE FROM scene WHERE nom = :sceneName");
    query.bindValue(":sceneName", sceneName);
    QVERIFY(query.exec());
}

void TestModulesDMXProject::testTcpConnection()
{
    // Cr�er un client TCP temporaire pour les tests
    QTcpSocket client;

    // Tenter de se connecter au serveur
    client.connectToHost("192.168.64.170", 12345); // Remplacez 12345 par le num�ro de port de votre serveur
    if (client.waitForConnected()) {
        qDebug() << "Connexion au serveur r�ussie";
    }
    else {
        qDebug() << "�chec de la connexion au serveur";
        QFAIL("�chec de la connexion au serveur");
    }

    // V�rifier si la connexion est �tablie
    if (client.isOpen()) {
        qDebug() << "La connexion est �tablie";
    }
    else {
        qDebug() << "La connexion n'est pas �tablie";
        QFAIL("La connexion n'est pas �tablie");
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
            qDebug() << "Trame DMX envoy�e au serveur";
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

    // Fermer la connexion
    client.disconnectFromHost();

    // Attendre que la d�connexion soit effective uniquement si le socket �tait connect�
    if (client.state() != QAbstractSocket::UnconnectedState) {
        if (client.waitForDisconnected()) {
            qDebug() << "D�connexion du serveur r�ussie";
        }
        else {
            qDebug() << "�chec de la d�connexion du serveur";
            QFAIL("�chec de la d�connexion du serveur");
        }
    }
}




