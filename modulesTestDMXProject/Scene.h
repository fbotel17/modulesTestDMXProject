#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <QSqlDatabase>
#include <QtSql>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidgetItem> 
#include <QScrollArea>
#include <QStringList>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QWizard>

class Scene
{
public:


    Scene();
    Scene(int id, const QString& nom);

    int getId() const;
    void setId(int id);

    QString getNom() const;
    void setNom(const QString& nom);

    void insertScene(const QString& nomScene);
    void updateScene(const QString& oldNomScene, const QString& newNomScene);
    QList<Scene> getAllScenes();
    void afficherScenes(QListWidget* listWidget);
    int getSceneId(const QString& sceneName);

private:
    int id;
    QString nom;
};

#endif // SCENE_H
