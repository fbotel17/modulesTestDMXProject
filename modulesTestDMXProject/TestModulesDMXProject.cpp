#include "TestModulesDMXProject.h"
#include <QSqlDatabase>
#include <QtSql/QSqlQuery>
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
