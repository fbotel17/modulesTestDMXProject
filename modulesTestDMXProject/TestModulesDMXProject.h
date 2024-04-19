#pragma once
#include <QObject>
#include "modulesTestDMXProject.h"
#include "Scene.h"
#include <QtTest/QTest>

class TestModulesDMXProject : public QObject
{
    Q_OBJECT

public:
    TestModulesDMXProject();
    ~TestModulesDMXProject();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testInsertScene();

    void testUpdateScene();
    void deleteScene(const QString& sceneName);

private:
    modulesTestDMXProject* testObject;
    Scene* scene;

};
