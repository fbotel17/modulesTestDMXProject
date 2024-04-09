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
    testObject->insertScene("TestScene");


    // Vérifier si la scène a été insérée dans la base de données
    query.prepare("SELECT * FROM scene WHERE nom = 'TestScene'");
    QVERIFY(query.exec());
    QCOMPARE(query.size(), 1);
    deleteScene("TestScene");
}

void TestModulesDMXProject::testAfficherScenes()
{

    // Vider la liste des scènes dans l'IHM avant d'insérer une nouvelle scène
    testObject->getSceneListWidget()->clear();

    // Insérer une scène de test dans la base de données
    testObject->insertScene("TestScene");
    deleteScene("NewTestScene"); // Supprimer la scène mise à jour


    // Appeler la méthode afficherScenes
    testObject->afficherScenes();

    // Vérifier si la scène a été ajoutée à la liste des scènes dans l'IHM
    QCOMPARE(testObject->getSceneListWidget()->count(), 1);
    QCOMPARE(testObject->getSceneListWidget()->item(0)->text(), QString("TestScene"));

    // Nettoyer la base de données après le test
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
    testObject->insertScene("TestScene");

    testObject->updateScene("TestScene", "NewTestScene");
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
