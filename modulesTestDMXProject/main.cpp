#include "modulesTestDMXProject.h"
#include "TestModulesDMXProject.h" // Assurez-vous d'inclure le fichier contenant vos tests

#include <QApplication>
#include <QTest>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    
    // Instancier vos tests
    TestModulesDMXProject test;

    // Exécuter les tests et stocker le résultat
    int testResult = QTest::qExec(&test);

    // Créer et afficher l'interface graphique uniquement si les tests réussissent
    if (testResult == 0) {
        modulesTestDMXProject w;
        w.show();
        return a.exec();
    }
    else {
        qDebug() << "Some tests failed. Aborting application.";
        return testResult;
    }
}

