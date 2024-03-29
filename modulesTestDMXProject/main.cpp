#include "modulesTestDMXProject.h"
#include "TestModulesDMXProject.h" // Assurez-vous d'inclure le fichier contenant vos tests

#include <QApplication>
#include <QTest>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    
    // Instancier vos tests
    TestModulesDMXProject test;

    // Ex�cuter les tests et stocker le r�sultat
    int testResult = QTest::qExec(&test);

    // Cr�er et afficher l'interface graphique uniquement si les tests r�ussissent
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

