#pragma once

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


class Champ
{
public:
    Champ();
    Champ(int id, int idEquip, int idNumCanal, QString nom);

    int getId() const;
    int getIdEquip() const;
    int getIdNumCanal() const;
    QString getNom() const;

    void setId(int id);
    void setIdEquip(int idEquip);
    void setIdNumCanal(int idNumCanal);
    void setNom(const QString& nom);

    void insertChamp(int idEquip, int idNumCanal, const QString& nom);
    void updateChamp(int id, int idEquip, int idNumCanal, const QString& nom);
    void deleteChamp(int id);
    QList<Champ> getAllChamps();
    Champ getChampById(int id);
    QList<Champ> getChampsByEquipmentId(int idEquip);

private:
    int id;
    int idEquip;
    int idNumCanal;
    QString nom;
};


