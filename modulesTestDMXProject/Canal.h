#pragma once
#ifndef CANAL_H
#define CANAL_H

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



class Canal : public QObject
{
    Q_OBJECT

public:
    Canal();

    int getId() const;
    void setId(int id);

    int getIdEquip() const;
    void setIdEquip(int idEquip);

    int getNumCanal() const;
    void setNumCanal(int numCanal);

    QString getNom() const;
    void setNom(const QString& nom);

    void insertCanal(int idScene, const QList<QPair<int, int>>& channelData);
    void updateCanal(int id, int idEquip, int numCanal, const QString& nom);
    QList<Canal> getAllCanaux();
    QList<Canal> getCanauxByEquipmentId(int idEquip);

private:
    int id;
    int idEquip;
    int numCanal;
    QString nom;
};

#endif // CANAL_H


