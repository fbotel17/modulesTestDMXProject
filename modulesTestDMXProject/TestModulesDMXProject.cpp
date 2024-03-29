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
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.1.59");
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
    // V�rifier si la sc�ne existe d�j� dans la base de donn�es
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    if (query.exec() && query.size() > 0) {
        qDebug() << "La sc�ne 'TestScene' existe d�j� dans la base de donn�es.";
        return;
    }

    // Appeler la m�thode insertScene avec un nom de sc�ne de test
    testObject->insertScene("TestScene");

    // V�rifier si la sc�ne a �t� ins�r�e dans la base de donn�es
    query.prepare("SELECT * FROM scene WHERE nom = 'TestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);

    // V�rifier les valeurs de la sc�ne ins�r�e
    query.first();
    QCOMPARE(query.value("id").toInt(), 1);
    QCOMPARE(query.value("nom").toString(), QString("TestScene"));
}

void TestModulesDMXProject::testAfficherScenes()
{
    // Ins�rer une sc�ne de test dans la base de donn�es
    testObject->insertScene("TestScene");

    // Appeler la m�thode afficherScenes
    testObject->afficherScenes();

    // V�rifier si la sc�ne a �t� ajout�e � la liste des sc�nes dans l'IHM
    QCOMPARE(testObject->getSceneListWidget()->count(), 1);
    QCOMPARE(testObject->getSceneListWidget()->item(0)->text(), QString("TestScene"));

    // Nettoyer la base de donn�es apr�s le test
    QSqlQuery query("DELETE FROM scene WHERE nom = 'TestScene'", QSqlDatabase::database());
    QVERIFY(query.exec());
}
