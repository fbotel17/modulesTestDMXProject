#pragma once
#include <QObject>
#include "modulesTestDMXProject.h"

class TestModulesDMXProject : public QObject
{
    Q_OBJECT

public:
    TestModulesDMXProject();
    ~TestModulesDMXProject();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testAjouterScene();
    void testModifierScene();
    // Ajoutez d'autres méthodes de test ici
};

