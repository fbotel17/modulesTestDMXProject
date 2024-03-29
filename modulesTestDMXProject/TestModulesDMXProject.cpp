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
    // Cette m�thode est ex�cut�e une fois avant tous les tests.
    // Vous pouvez y initialiser la base de donn�es et effectuer d'autres t�ches de configuration.

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.64.213");
    db.setDatabaseName("testCodeDMX");
    db.setUserName("root");
    db.setPassword("root");

    if (!db.open()) {
        qDebug() << "�chec de la connexion � la base de donn�es.";
        // G�rer les erreurs de connexion
        return;
    }
}

void TestModulesDMXProject::cleanupTestCase()
{
    // Cette m�thode est ex�cut�e une fois apr�s tous les tests.
    // Vous pouvez y nettoyer la base de donn�es et effectuer d'autres t�ches de nettoyage.

    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
    }
}

void TestModulesDMXProject::testAjouterScene()
{
    // Cr�ez une instance de votre classe � tester
    modulesTestDMXProject testObject;

    // Appelez la m�thode � tester
    testObject.on_pushButtonValider_clicked();

    // V�rifiez que la sc�ne a �t� ajout�e � la base de donn�es
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'Nouvelle sc�ne'");
    QVERIFY(query.next());
    QVERIFY(query.value(0).toInt() == 1); // V�rifiez que l'ID est correct (dans ce cas, nous supposons que l'ID est 1)
    QVERIFY(query.value(1).toString().compare("Nouvelle sc�ne") == 0);
}

void TestModulesDMXProject::testModifierScene()
{
    // Cr�ez une instance de votre classe � tester
    modulesTestDMXProject testObject;

    // Ajoutez une sc�ne � la base de donn�es pour la modifier
    testObject.insertScene("Sc�ne � modifier");

    // Modifiez la sc�ne
    // (Vous devez impl�menter une m�thode pour modifier une sc�ne existante dans votre classe modulesTestDMXProject)
    //testObject.modifierScene(1, "Sc�ne modifi�e");

    // V�rifiez que la sc�ne a �t� modifi�e dans la base de donn�es
    QSqlQuery query("SELECT * FROM scene WHERE nom = 'Sc�ne modifi�e'");
    QVERIFY(query.next());
    QVERIFY(query.value(0).toInt() == 1); // V�rifiez que l'ID est correct (dans ce cas, nous supposons que l'ID est 1)
    QVERIFY(query.value(1).toString().compare("Sc�ne modifi�e") == 0);
}
