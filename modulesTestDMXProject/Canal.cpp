#include "Canal.h"


Canal::Canal()
{
    id = -1;
    idEquip = -1;
    numCanal = -1;
    nom = "";
}


int Canal::getId() const
{
    return id;
}

void Canal::setId(int id)
{
    this->id = id;
}

int Canal::getIdEquip() const
{
    return idEquip;
}

void Canal::setIdEquip(int idEquip)
{
    this->idEquip = idEquip;
}

int Canal::getNumCanal() const
{
    return numCanal;
}

void Canal::setNumCanal(int numCanal)
{
    this->numCanal = numCanal;
}

QString Canal::getNom() const
{
    return nom;
}

void Canal::setNom(const QString& nom)
{
    this->nom = nom;
}

void Canal::insertCanal(int idScene, const QList<QPair<int, int>>& channelData) {
    QSqlQuery query;
    query.prepare("INSERT INTO canaux (idScene, numCanal, valeur) VALUES (:idScene, :numCanal, :valeur)");
    query.bindValue(":idScene", idScene);

    for (const auto& channel : channelData) {
        int numCanal = channel.first;
        int valeur = channel.second;

        // Vérifier si le numéro de canal existe dans la table champ
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM champ WHERE idNumCanal = :numCanal");
        checkQuery.bindValue(":numCanal", numCanal);
        if (checkQuery.exec() && checkQuery.next()) {
            int count = checkQuery.value(0).toInt();
            if (count == 0) {
                qDebug() << "Le numéro de canal" << numCanal << "n'existe pas dans la table champ.";
                continue; // Ignorer cette insertion
            }
        }
        else {
            qDebug() << "Erreur lors de la vérification de l'existence du numéro de canal" << numCanal << ":" << checkQuery.lastError().text();
            continue; // Ignorer cette insertion
        }

        query.bindValue(":numCanal", numCanal);
        query.bindValue(":valeur", valeur);

        if (!query.exec()) {
            qDebug() << "Erreur lors de l'insertion des données dans la table 'canaux' : " << query.lastError().text();
        }
    }
}

void Canal::updateCanal(int id, int idEquip, int numCanal, const QString& nom)
{
    QSqlQuery query;
    query.prepare("UPDATE canal SET idEquip = :idEquip, numCanal = :numCanal, nom = :nom WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":idEquip", idEquip);
    query.bindValue(":numCanal", numCanal);
    query.bindValue(":nom", nom);
    query.exec();
}

QList<Canal> Canal::getAllCanaux()
{
    QList<Canal> canaux;
    QSqlQuery query("SELECT * FROM canal");
    while (query.next()) {
        int id = query.value(0).toInt();
        int idEquip = query.value(1).toInt();
        int numCanal = query.value(2).toInt();
        QString nom = query.value(3).toString();
    }
    return canaux;
}

QList<Canal> Canal::getCanauxByEquipmentId(int idEquip)
{
    QList<Canal> canaux;
    QSqlQuery query;
    query.prepare("SELECT * FROM canal WHERE idEquip = :idEquip");
    query.bindValue(":idEquip", idEquip);
    query.exec();
    while (query.next()) {
        int id = query.value(0).toInt();
        int numCanal = query.value(2).toInt();
        QString nom = query.value(3).toString();
    }
    return canaux;
}
