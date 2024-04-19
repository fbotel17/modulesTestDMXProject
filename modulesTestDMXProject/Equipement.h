#pragma once
#ifndef EQUIPEMENT_H
#define EQUIPEMENT_H

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

class modulesTestDMXProject;

class Equipement
{

public:
    Equipement();
    Equipement(int id, const QString& nom, const QString& adresse, int nbCanaux);

    void insertEquipement(const QString& nomEquipement, const QString& adresseEquipement, int nbCanaux);
    void updateEquipement(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement);
    QList<Equipement> getAllEquipements();
    void afficherEquipements(QVBoxLayout* mainLayoutEquipements);
    int getEquipmentId(const QString& equipmentName);
    void supprimerEquipement(int idEquipement, modulesTestDMXProject* dmx);

    int getId() const;
    void setId(int id);

    QString getNom() const;
    void setNom(const QString& nom);

    QString getAdresse() const;
    void setAdresse(const QString& adresse);

    int getNbCanaux() const;
    void setNbCanaux(int nbCanaux);

private:
    int id;
    QString nom;
    QString adresse;
    int nbCanaux;

};

#endif // EQUIPEMENT_H

