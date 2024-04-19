#include "Champ.h"

Champ::Champ()
{
    id = -1;
    idEquip = -1;
    idNumCanal = -1;
    nom = "";
}

Champ::Champ(int id, int idEquip, int idNumCanal, QString nom)
{
    this->id = id;
    this->idEquip = idEquip;
    this->idNumCanal = idNumCanal;
    this->nom = nom;
}

int Champ::getId() const
{
    return id;
}

int Champ::getIdEquip() const
{
    return idEquip;
}

int Champ::getIdNumCanal() const
{
    return idNumCanal;
}

QString Champ::getNom() const
{
    return nom;
}

void Champ::setId(int id)
{
    this->id = id;
}

void Champ::setIdEquip(int idEquip)
{
    this->idEquip = idEquip;
}

void Champ::setIdNumCanal(int idNumCanal)
{
    this->idNumCanal = idNumCanal;
}

void Champ::setNom(const QString& nom)
{
    this->nom = nom;
}

void Champ::insertChamp(int idEquip, int idNumCanal, const QString& nom)
{
    QSqlQuery query;
    query.prepare("INSERT INTO champ (idEquip, idNumCanal, nom) VALUES (:idEquip, :idNumCanal, :nom)");
    query.bindValue(":idEquip", idEquip);
    query.bindValue(":idNumCanal", idNumCanal);
    query.bindValue(":nom", nom);

    if (query.exec()) {
        qDebug() << "Champ inséré avec succès!";
    }
    else {
        qDebug() << "Erreur lors de l'insertion duchamp:" << query.lastError().text();
    }
}

void Champ::updateChamp(int id, int idEquip, int idNumCanal, const QString& nom)
{
    QSqlQuery query;
    query.prepare("UPDATE champ SET idEquip = :idEquip, idNumCanal = :idNumCanal, nom = :nom WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":idEquip", idEquip);
    query.bindValue(":idNumCanal", idNumCanal);
    query.bindValue(":nom", nom);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la mise à jour du champ : " << query.lastError().text();
    }
}

void Champ::deleteChamp(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM champ WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression du champ : " << query.lastError().text();
    }
}

QList<Champ> Champ::getAllChamps()
{
    QList<Champ> champs;

    QSqlQuery query("SELECT * FROM champ");

    while (query.next()) {
        int id = query.value(0).toInt();
        int idEquip = query.value(1).toInt();
        int idNumCanal = query.value(2).toInt();
        QString nom = query.value(3).toString();

        Champ champ(id, idEquip, idNumCanal, nom);
        champs.append(champ);
    }

    return champs;
}

Champ Champ::getChampById(int id)
{
    Champ champ;

    QSqlQuery query;
    query.prepare("SELECT * FROM champ WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        int idEquip = query.value(1).toInt();
        int idNumCanal = query.value(2).toInt();
        QString nom = query.value(3).toString();

        champ = Champ(id, idEquip, idNumCanal, nom);
    }
    else {
        qDebug() << "Erreur lors de la récupération du champ : " << query.lastError().text();
    }

    return champ;
}

QList<Champ> Champ::getChampsByEquipmentId(int idEquip)
{
    QList<Champ> champs;

    QSqlQuery query;
    query.prepare("SELECT * FROM champ WHERE idEquip = :idEquip");
    query.bindValue(":idEquip", idEquip);

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int idNumCanal = query.value(2).toInt();
            QString nom = query.value(3).toString();

            Champ champ(id, idEquip, idNumCanal, nom);
            champs.append(champ);
        }
    }
    else {
        qDebug() << "Erreur lors de la récupération des champs : " << query.lastError().text();
    }

    return champs;
}
