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
    // Cette méthode est exécutée une fois avant tous les tests.
    // Vous pouvez y initialiser la base de données et effectuer d'autres tâches de configuration.

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.64.213");
    db.setDatabaseName("testCodeDMX");
    db.setUserName("root");
    db.setPassword("root");

    if (!db.open()) {
        qDebug() << "Échec de la connexion à la base de données.";
        // Gérer les erreurs de connexion
        return;
    }
}

void TestModulesDMXProject::cleanupTestCase()
{
    // Cette méthode est exécutée une fois après tous les tests.
    // Vous pouvez y nettoyer la base de données et effectuer d'autres tâches de nettoyage.

    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
    }
}

void TestModulesDMXProject::testAjouterScene()
{
    // Créez une instance de votre classe à tester
    modulesTestDMXProject testObject;

    // Appelez la méthode à tester
    testObject.on_pushButtonValider_clicked();

    // Vérifiez que la scène a été ajoutée à la base de données
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'Nouvelle scène'");
    QVERIFY(query.next());
    QVERIFY(query.value(0).toInt() == 1); // Vérifiez que l'ID est correct (dans ce cas, nous supposons que l'ID est 1)
    QVERIFY(query.value(1).toString().compare("Nouvelle scène") == 0);
}

void TestModulesDMXProject::testModifierScene()
{
    // Créez une instance de votre classe à tester
    modulesTestDMXProject testObject;

    // Ajoutez une scène à la base de données pour la modifier
    testObject.insertScene("Scène à modifier");

    // Modifiez la scène
    // (Vous devez implémenter une méthode pour modifier une scène existante dans votre classe modulesTestDMXProject)
    //testObject.modifierScene(1, "Scène modifiée");

    // Vérifiez que la scène a été modifiée dans la base de données
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'Scène modifiée'");
    QVERIFY(query.next());
    QVERIFY(query.value(0).toInt() == 1); // Vérifiez que l'ID est correct (dans ce cas, nous supposons que l'ID est 1)
    QVERIFY(query.value(1).toString().compare("Scène modifiée") == 0);
}
