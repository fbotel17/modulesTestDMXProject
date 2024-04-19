#include "Equipement.h"
#include "modulesTestDMXProject.h"

Equipement::Equipement()
{
    id = -1;
    nom = "";
    adresse = "";
    nbCanaux = 0;
}

Equipement::Equipement(int id, const QString& nom, const QString& adresse, int nbCanaux)
{
    this->id = id;
    this->nom = nom;
    this->adresse = adresse;
    this->nbCanaux = nbCanaux;
}

void Equipement::insertEquipement(const QString& nomEquipement, const QString& adresseEquipement, int nbCanaux)
{
    QSqlQuery query;
    query.prepare("INSERT INTO equipement (nom, adresse, nbCanal) VALUES (:nom, :adresse, :nbCanal)");
    query.bindValue(":nom", nomEquipement);
    query.bindValue(":adresse", adresseEquipement);
    query.bindValue(":nbCanal", nbCanaux);

    if (query.exec()) {
        qDebug() << "�quipement ins�r� avec succ�s!";
    }
    else {
        qDebug() << "Erreur lors de l'insertion de l'�quipement:" << query.lastError().text();
    }
}

void Equipement::updateEquipement(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement)
{
    // Connexion � la base de donn�es
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "La base de donn�es n'est pas valide.";
        return;
    }

    if (!db.isOpen()) {
        qDebug() << "La base de donn�es n'est pas ouverte.";
        return;
    }

    // Pr�paration de la requ�te SQL pour mettre � jour l'�quipement
    QSqlQuery query(db);
    query.prepare("UPDATE `equipement` SET `nom` = :nom, `adresse` = :adresse, `nbCanal` = :nbCanal WHERE `id` = :id");
    query.bindValue(":nom", nomEquipement);
    query.bindValue(":adresse", adresseEquipement);
    query.bindValue(":nbCanal", nbCanalEquipement);
    query.bindValue(":id", idEquipement);

    // Ex�cution de la requ�te
    if (query.exec()) {
        db.commit(); // Valider les modifications dans la base de donn�es
        qDebug() << "�quipement mis � jour avec succ�s dans la base de donn�es.";
    }
    else {
        qDebug() << "Erreur lors de la mise � jour de l'�quipement :" << query.lastError().text();
    }
}

void Equipement::supprimerEquipement(int idEquipement, modulesTestDMXProject* dmx)
{
    // D�but de la transaction SQL
    QSqlDatabase::database().transaction();

    // Supprimer d'abord les enregistrements dans la table 'champ' li�s � l'�quipement � supprimer
    QSqlQuery queryDeleteChamp;
    queryDeleteChamp.prepare("DELETE FROM champ WHERE idEquip = :id");
    queryDeleteChamp.bindValue(":id", idEquipement);
    if (!queryDeleteChamp.exec()) {
        qDebug() << "Erreur lors de la suppression des enregistrements dans la table champ :" << queryDeleteChamp.lastError().text();
        // En cas d'erreur, annuler la transaction et sortir de la fonction
        QSqlDatabase::database().rollback();
        return;
    }

    // Ensuite, supprimer l'�quipement de la table 'equipement'
    QSqlQuery queryDeleteEquipement;
    queryDeleteEquipement.prepare("DELETE FROM equipement WHERE id = :id");
    queryDeleteEquipement.bindValue(":id", idEquipement);
    if (!queryDeleteEquipement.exec()) {
        qDebug() << "Erreur lors de la suppression de l'�quipement :" << queryDeleteEquipement.lastError().text();
        // En cas d'erreur, annuler la transaction et sortir de la fonction
        QSqlDatabase::database().rollback();
        return;
    }

    // Valider la transaction SQL
    QSqlDatabase::database().commit();

    qDebug() << "�quipement supprim� avec succ�s de la base de donn�es.";

    dmx->Gerer_un_equipement();
}


QList<Equipement> Equipement::getAllEquipements()
{
    QList<Equipement> equipements;

    QSqlQuery query("SELECT * FROM equipement");

    while (query.next()) {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString adresse = query.value(2).toString();
        int nbCanaux = query.value(3).toInt();

        Equipement equipement(id, nom, adresse, nbCanaux);
        equipements.append(equipement);
    }

    return equipements;
}

void Equipement::afficherEquipements(QVBoxLayout* mainLayoutEquipements)
{


    // Ex�cuter une requ�te pour r�cup�rer tous les �quipements existants
    QSqlQuery query("SELECT nom FROM equipement");

    // Cr�er un widget de d�filement pour contenir le layout des �quipements
    QScrollArea* scrollArea = new QScrollArea;
    QWidget* scrollWidget = new QWidget;
    QVBoxLayout* layoutEquipements = new QVBoxLayout(scrollWidget);
    scrollWidget->setLayout(layoutEquipements);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    // R�cup�rer le layout pour les �quipements
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(mainLayoutEquipements->layout());
    if (!mainLayout) {
        qDebug() << "Erreur : le layout des �quipements est invalide.";
        return;
    }

    // Effacer le contenu existant du layout principal
    QLayoutItem* child;
    while ((child = mainLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Parcourir les r�sultats de la requ�te et ajouter chaque �quipement en tant que case � cocher
    while (query.next()) {
        QString nomEquipement = query.value(0).toString();

        // Cr�er une case � cocher pour l'�quipement
        QCheckBox* checkBox = new QCheckBox(nomEquipement);
        // Ajouter la case � cocher au layout des �quipements
        layoutEquipements->addWidget(checkBox);
    }

    // Ajouter le widget de d�filement au layout principal
    mainLayout->addWidget(scrollArea);
}

int Equipement::getEquipmentId(const QString& equipmentName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM equipement WHERE nom = :nom");
    query.bindValue(":nom", equipmentName);

    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        qDebug() << "Erreur lors de la r�cup�ration de l'ID de l'�quipement : " << query.lastError().text();
        return -1;
    }
}

int Equipement::getId() const
{
    return id;
}

void Equipement::setId(int id)
{
    this->id = id;
}

QString Equipement::getNom() const
{
    return nom;
}

void Equipement::setNom(const QString& nom)
{
    this->nom = nom;
}

QString Equipement::getAdresse() const
{
    return adresse;
}

void Equipement::setAdresse(const QString& adresse)
{
    this->adresse = adresse;
}

int Equipement::getNbCanaux() const
{
    return nbCanaux;
}

void Equipement::setNbCanaux(int nbCanaux)
{
    this->nbCanaux = nbCanaux;
}