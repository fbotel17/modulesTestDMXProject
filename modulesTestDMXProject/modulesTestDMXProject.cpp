#include "modulesTestDMXProject.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidgetItem> // Ajoutez cette ligne
#include <QScrollArea>
#include <QStringList>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>

modulesTestDMXProject::modulesTestDMXProject(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.1.59");
	db.setDatabaseName("testCodeDMX");
	db.setUserName("root");
	db.setPassword("root");

	if (!db.open()) {
		qDebug() << "Échec de la connexion à la base de données.";
		// Gérer les erreurs de connexion
		return;
	}



	// Afficher toutes les scènes existantes
	afficherScenes();
	afficherEquipements();
	afficherScenesCheckbox();
	Gerer_un_equipement();
}

modulesTestDMXProject::~modulesTestDMXProject()
{}

void modulesTestDMXProject::on_pushButtonValider_clicked()
{
	// Récupérer le contenu de lineEditNomScene
	QString nomScene = ui.lineEditNomScene->text();

	// Exécuter une requête SQL pour insérer les données dans la table scene
	insertScene(nomScene);

	ui.listWidget->clear();

	afficherScenes();

}

void modulesTestDMXProject::insertScene(QString nomScene)
{
	QSqlQuery query;
	query.prepare("INSERT INTO scene (nom) VALUES (:nom)");
	query.bindValue(":nom", nomScene);

	if (query.exec()) {
		qDebug() << "Scène insérée avec succès!";
		// Faire quelque chose après l'insertion réussie, si nécessaire
	}
	else {
		qDebug() << "Erreur lors de l'insertion de la scène:" << query.lastError().text();
		// Gérer les erreurs d'insertion ici
	}
}



void modulesTestDMXProject::on_actionCreer_une_sc_ne_triggered()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void modulesTestDMXProject::on_actionConfigurer_une_sc_ne_2_triggered()
{
	afficherEquipements();
	afficherScenesCheckbox();
	ui.stackedWidget->setCurrentIndex(1);
}

void modulesTestDMXProject::on_actionAjouter_un_equipement_triggered()
{
	ui.stackedWidget->setCurrentIndex(2);
}

void modulesTestDMXProject::on_actionSupprimer_un_equipement_triggered()
{
	Gerer_un_equipement();
	ui.stackedWidget->setCurrentIndex(5);
}

void modulesTestDMXProject::afficherScenes()
{


	// Exécuter une requête pour récupérer toutes les scènes existantes
	QSqlQuery query("SELECT * FROM scene");

	// Parcourir les résultats de la requête et ajouter chaque scène à la liste
	while (query.next()) {
		int id = query.value(0).toInt();
		QString nom = query.value(1).toString();

		// Créer un nouvel élément pour chaque scène et l'ajouter à la liste
		QListWidgetItem* item = new QListWidgetItem(nom);
		ui.listWidget->addItem(item);
	}
}

void modulesTestDMXProject::afficherEquipements()
{


	// Exécuter une requête pour récupérer tous les équipements existants
	QSqlQuery query("SELECT nom FROM equipement");

	// Créer un widget de défilement pour contenir le layout des équipements
	QScrollArea* scrollArea = new QScrollArea;
	QWidget* scrollWidget = new QWidget;
	QVBoxLayout* layoutEquipements = new QVBoxLayout(scrollWidget);
	scrollWidget->setLayout(layoutEquipements);
	scrollArea->setWidget(scrollWidget);
	scrollArea->setWidgetResizable(true);

	// Récupérer le layout pour les équipements
	QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui.verticalLayoutEquipements->layout());
	if (!mainLayout) {
		qDebug() << "Erreur : le layout des équipements est invalide.";
		return;
	}

	// Effacer le contenu existant du layout principal
	QLayoutItem* child;
	while ((child = mainLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Parcourir les résultats de la requête et ajouter chaque équipement en tant que case à cocher
	while (query.next()) {
		QString nomEquipement = query.value(0).toString();

		// Créer une case à cocher pour l'équipement
		QCheckBox* checkBox = new QCheckBox(nomEquipement);
		// Ajouter la case à cocher au layout des équipements
		layoutEquipements->addWidget(checkBox);
	}

	// Ajouter le widget de défilement au layout principal
	mainLayout->addWidget(scrollArea);
}


// Modifiez votre slot on_buttonEquip_clicked pour appeler cette fonction
void modulesTestDMXProject::on_buttonEquip_clicked() {
	QString nomEquipement = ui.nomEquipEdit->text();
	QString adresseEquipement = ui.adresseEquipEdit->text();
	int startChannelAddress = ui.adresseEquipEdit->text().toInt(); // Convertir le texte en entier
	int nbCanaux = ui.nbCannauxEdit->text().toInt(); // Convertir le texte en entier
	createChannelLabelsAndLineEdits(nbCanaux, startChannelAddress);

	insertEquipement(nomEquipement, adresseEquipement, nbCanaux);
}

void modulesTestDMXProject::insertEquipement(QString nomEquipement, QString adresseEquipement, int nbCanaux) {

	QSqlQuery query;
	query.prepare("INSERT INTO equipement (nom, adresse, nbCanal) VALUES (:nom, :adresse, :nbCanal)");
	query.bindValue(":nom", nomEquipement);
	query.bindValue(":adresse", adresseEquipement);
	query.bindValue(":nbCanal", nbCanaux);

	if (query.exec()) {
		qDebug() << "Équipement inséré avec succès!";
	}
	else {
		qDebug() << "Erreur lors de l'insertion de l'équipement:" << query.lastError().text();
	}
}



void modulesTestDMXProject::afficherScenesCheckbox()
{


	// Exécuter une requête pour récupérer tous les équipements existants
	QSqlQuery query("SELECT nom FROM scene");

	// Créer un widget de défilement pour contenir le layout des scènes
	QScrollArea* scrollArea = new QScrollArea;
	QWidget* scrollWidget = new QWidget;
	QVBoxLayout* layoutScenes = new QVBoxLayout(scrollWidget);
	scrollWidget->setLayout(layoutScenes);
	scrollArea->setWidget(scrollWidget);
	scrollArea->setWidgetResizable(true);

	// Récupérer le layout pour les scènes
	QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui.verticalLayoutScenes->layout());
	if (!mainLayout) {
		qDebug() << "Erreur : le layout des scènes est invalide.";
		return;
	}

	// Effacer le contenu existant du layout principal
	QLayoutItem* child;
	while ((child = mainLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Garder une liste de toutes les cases à cocher
	QList<QCheckBox*> checkBoxes;

	// Parcourir les résultats de la requête et ajouter chaque scène en tant que case à cocher
	while (query.next()) {
		QString nomScene = query.value(0).toString();

		// Créer une case à cocher pour la scène
		QCheckBox* checkBox = new QCheckBox(nomScene);
		// Ajouter la case à cocher au layout des scènes
		layoutScenes->addWidget(checkBox);
		checkBoxes.append(checkBox); // Ajouter la case à cocher à la liste
	}

	// Définir une largeur fixe pour le widget de défilement
	scrollArea->setFixedWidth(200); // Vous pouvez ajuster cette valeur selon vos besoins

	// Ajouter le widget de défilement au layout principal
	mainLayout->addWidget(scrollArea);

	// Connecter le signal toggled() de chaque case à cocher à une fonction de gestion
	for (QCheckBox* checkBox : checkBoxes) {
		connect(checkBox, &QCheckBox::toggled, [this, checkBoxes, checkBox](bool checked) {
			if (checked) {
				// Désélectionner toutes les autres cases à cocher sauf celle qui vient d'être cochée
				for (QCheckBox* otherCheckBox : checkBoxes) {
					if (otherCheckBox != checkBox) {
						otherCheckBox->setChecked(false);
					}
				}
			}
			});
	}
}




void modulesTestDMXProject::createChannelLabelsAndLineEdits(int channelCount, int startChannelAddress) {
	// Supprimez les widgets précédemment ajoutés s'il y en a
	ui.stackedWidget->setCurrentIndex(3);
	QLayoutItem* child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Affichez les QLabel et les QLineEdit pour chaque canal
	for (int i = 0; i < channelCount; ++i) {
		QLabel* label = new QLabel("Nom du canal " + QString::number(startChannelAddress) + ":");
		QLineEdit* lineEdit = new QLineEdit;
		ui.verticalLayout_17->addWidget(label);
		ui.verticalLayout_17->addWidget(lineEdit);
		startChannelAddress++; // Incrémenter l'adresse de départ pour le prochain équipement
	}
}


void modulesTestDMXProject::on_validateButtonEquip_clicked() {
	// Récupérer l'ID du dernier équipement inséré
	QSqlQuery query;
	query.exec("SELECT LAST_INSERT_ID()");
	int idEquip = query.next() ? query.value(0).toInt() : -1;

	if (idEquip == -1) {
		qDebug() << "Erreur : impossible de récupérer l'ID du dernier équipement inséré.";
		return;
	}

	// Récupérer la valeur de numCanal
	int idNumCanal = ui.adresseEquipEdit->text().toInt();

	// Parcourir tous les QLineEdit et insérer les données dans la table "champ"
	QLayoutItem* child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr) {
		QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(child->widget());
		if (lineEdit) {
			QString nom = lineEdit->text();

			// Insérer les données dans la table "champ"
			query.prepare("INSERT INTO champ (idEquip, idNumCanal, nom) VALUES (:idEquip, :idNumCanal, :nom)");
			query.bindValue(":idEquip", idEquip);
			query.bindValue(":idNumCanal", idNumCanal);
			query.bindValue(":nom", nom);

			if (!query.exec()) {
				qDebug() << "Erreur lors de l'insertion des données dans la table 'champ' : " << query.lastError().text();
			}

			idNumCanal++; // Incrémenter idNumCanal pour le prochain enregistrement
		}
		delete child->widget();
		delete child;
	}
}


void modulesTestDMXProject::on_pushButton_clicked()
{
	qDebug() << "Number of children in verticalLayoutEquipements:" << ui.verticalLayoutEquipements->count();

	// Find the widgetEquipements and widgetScenes widgets
	QWidget* widgetEquipements = ui.verticalLayoutEquipements->itemAt(0)->widget();
	QWidget* widgetScenes = ui.verticalLayoutScenes->itemAt(0)->widget();

	// Récupérer les équipements sélectionnés
	QList<QCheckBox*> checkedEquipements = widgetEquipements->findChildren<QCheckBox*>();
	QList<QString> selectedEquipements;

	if (checkedEquipements.isEmpty())
	{
		qDebug() << "No QCheckBox objects found in widgetEquipements.";
	}
	else
	{
		qDebug() << "Found" << checkedEquipements.size() << "QCheckBox objects in widgetEquipements.";

		for (QCheckBox* checkBox : checkedEquipements)
		{
			if (checkBox->isChecked())
			{
				// Récupérer le nom de l'équipement à partir du texte de la QCheckBox
				QString equipementName = checkBox->text();
				selectedEquipements.append(equipementName);
			}
		}
	}

	// Récupérer la scène sélectionnée
	QList<QCheckBox*> checkedScenes = widgetScenes->findChildren<QCheckBox*>();
	QString selectedScene;

	if (checkedScenes.isEmpty())
	{
		qDebug() << "No QCheckBox objects found in widgetScenes.";
	}
	else
	{
		qDebug() << "Found" << checkedScenes.size() << "QCheckBox objects in widgetScenes.";

		for (QCheckBox* checkBox : checkedScenes)
		{
			if (checkBox->isChecked())
			{
				// Récupérer le nom de la scène à partir du texte de la QCheckBox
				QString sceneName = checkBox->text();
				selectedScene = sceneName;
				m_selectedScene = sceneName;
				break;
			}
		}
	}

	if (!selectedEquipements.isEmpty() && !selectedScene.isEmpty())
	{

		// Créer le formulaire dynamique
		createFormForSelectedEquipements(selectedEquipements, selectedScene);

		// Passer à la page d'index 4
		ui.stackedWidget->setCurrentIndex(4);
	}
	else
	{
		qDebug() << "No equipments or scene selected.";
	}
}



void modulesTestDMXProject::createFormForSelectedEquipements(const QList<QString>& selectedEquipements, const QString& selectedScene)
{
	clearForm();
	// Effacer le layout verticalLayout_18
	QLayoutItem* child;
	while ((child = ui.verticalLayout_18->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Récupérer les informations sur les canaux pour chaque équipement sélectionné
	QString joinedEquipementNames = selectedEquipements.join("','");
	QSqlQuery query(QString("SELECT e.nom, c.idNumCanal, c.nom AS canalNom "
		"FROM equipement e "
		"JOIN champ c ON e.id = c.idEquip "
		"WHERE e.nom IN ('%1') "
		"ORDER BY e.nom, c.idNumCanal").arg(joinedEquipementNames));

	QMap<QString, QList<QPair<int, QString>>> equipementCanaux;
	while (query.next())
	{
		QString equipementName = query.value(0).toString();
		int canalId = query.value(1).toInt();
		QString canalNom = query.value(2).toString();

		if (!equipementCanaux.contains(equipementName))
		{
			equipementCanaux[equipementName] = QList<QPair<int, QString>>();
		}

		equipementCanaux[equipementName].append(qMakePair(canalId, canalNom));
	}

	// Créer les QLabel et QLineEdit pour chaque canal des équipements sélectionnés
	for (const QString& equipementName : selectedEquipements)
	{
		QList<QPair<int, QString>> canaux = equipementCanaux[equipementName];
		for (const QPair<int, QString>& canal : canaux)
		{
			QLabel* label = new QLabel(canal.second + " :");
			int idNumCanal = getEquipmentCanalNumber(equipementName, canal.first); // Appel de la méthode getEquipmentCanalNumber
			numCanal = canal.first;
			if (idNumCanal != -1)
			{
				label->setText(QString("Nom du canal %1 :").arg(idNumCanal)); // Utilisation du numéro de canal récupéré
			}
			else
			{
				qDebug() << "Erreur : impossible de récupérer le numéro de canal pour l'équipement" << equipementName;
			}

			QLineEdit* lineEdit = new QLineEdit;
			ui.verticalLayout_18->addWidget(label);
			ui.verticalLayout_18->addWidget(lineEdit);
		}
	}


}

void modulesTestDMXProject::insertChannelData(int idScene, QList<QPair<int, int>> channelData)
{
	QSqlQuery query;

	for (const auto& channel : channelData)
	{
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

		query.prepare("INSERT INTO canaux (numCanal, idScene, valeur) VALUES (:numCanal, :idScene, :valeur)");
		query.bindValue(":numCanal", numCanal);
		query.bindValue(":idScene", idScene);
		query.bindValue(":valeur", valeur);

		if (!query.exec())
		{
			qDebug() << "Erreur lors de l'insertion des données dans la table 'canaux' : " << query.lastError().text();
		}
	}
}

int modulesTestDMXProject::getEquipmentId(const QString& equipmentName)
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
		qDebug() << "Erreur lors de la récupération de l'ID de l'équipement : " << query.lastError().text();
		return -1;
	}
}


int modulesTestDMXProject::getSceneId(const QString& sceneName)
{
	QSqlQuery query;
	query.prepare("SELECT id FROM scene WHERE nom = :nom");
	query.bindValue(":nom", sceneName);

	if (query.exec() && query.next())
	{
		return query.value(0).toInt();
	}
	else
	{
		qDebug() << "Erreur lors de la récupération de l'ID de la scène : " << query.lastError().text();
		return -1;
	}
}

void modulesTestDMXProject::on_ValidateButtonCanal_clicked()
{
	// Récupérer l'ID de la scène sélectionnée
	int idScene = getSceneId(m_selectedScene);
	if (idScene == -1)
	{
		qDebug() << "Erreur : impossible de récupérer l'ID de la scène.";
		return;
	}

	// Récupérer les données des canaux pour l'équipement actuel
	QList<QPair<int, int>> channelData;
	QLayoutItem* child;
	int channelDataSize = 0; // Initialiser la variable pour stocker la taille de channelData
	while ((child = ui.verticalLayout_18->takeAt(0)) != nullptr)
	{
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(child->widget());
		if (lineEdit)
		{
			QString text = lineEdit->text();

			// Convertir le texte en entier
			bool conversionOk;
			int value = text.toInt(&conversionOk);

			// Vérifier si la conversion a réussi
			if (conversionOk)
			{
				// Utiliser la valeur convertie ici
				channelData.append(qMakePair(0, value)); // Le numéro du canal sera ajusté plus tard
				channelDataSize++; // Incrémenter la taille de la liste
			}
			else
			{
				// Gérer le cas où la conversion échoue
				qDebug() << "La conversion a échoué. Le texte n'est pas un entier valide :" << text;
				// Ajouter une valeur par défaut
				channelData.append(qMakePair(0, 0));
				channelDataSize++; // Incrémenter la taille de la liste
			}
		}
		delete child;
	}

	// Calculer le numéro du premier canal en soustrayant la taille de channelData de numCanal
	int currentChannelNumber = numCanal - channelDataSize + 1;

	// Mettre à jour les numéros de canal dans channelData avec les valeurs calculées
	for (int i = 0; i < channelData.size(); ++i)
	{
		channelData[i].first = currentChannelNumber++;
	}

	// Insérer les données des canaux dans la table "canaux"
	insertChannelData(idScene, channelData);
}



int modulesTestDMXProject::getEquipmentCanalNumber(const QString& equipmentName, int canalNumber)
{

	QSqlQuery query;
	query.prepare("SELECT idNumCanal FROM champ WHERE idEquip = (SELECT id FROM equipement WHERE nom = :nom) AND idNumCanal = :idNumCanal");
	query.bindValue(":nom", equipmentName);
	query.bindValue(":idNumCanal", canalNumber);


	if (query.exec() && query.next())
	{

		return canalNumber;

	}
	else
	{
		qDebug() << "Erreur lors de la récupération du numéro de canal : " << query.lastError().text();
		return -1;
	}
}


void modulesTestDMXProject::clearForm()
{
	// Effacer le layout existant
	QLayoutItem* child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Vider la liste des pointeurs de QLineEdit
	m_lineEdits.clear();
}


// Mettre à jour la méthode Supprimer_un_equipement pour utiliser la variable membre m_idEquipementASupprimer
void modulesTestDMXProject::Gerer_un_equipement()
{


	// Vérifier si un équipement doit être supprimé

		// Effacer tous les widgets précédents du layout
	QLayoutItem* child;
	while ((child = ui.verticalLayout_3->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Vider la liste des pointeurs de QLineEdit
	m_lineEdits.clear();

	// Créer un nouveau QTableView
	QTableView* tableView = new QTableView;

	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

	// Créer un modèle de tableau vide avec cinq colonnes
	QStandardItemModel* model = new QStandardItemModel(0, 5);
	tableView->setModel(model);

	// Définir les en-têtes de colonne
	model->setHorizontalHeaderLabels(QStringList() << "Nom de l'équipement" << "Adresse" << "Nombre de Canal" << "Modifier" << "Supprimer");

	// Exécuter une requête pour récupérer tous les équipements existants
	QSqlQuery query("SELECT id, nom, adresse, nbCanal FROM equipement");

	int row = 0; // Variable pour garder une trace du numéro de ligne

	// Parcourir les résultats de la requête et ajouter chaque équipement en tant que ligne dans le modèle de tableau
	while (query.next()) {
		int currentIdEquipement = query.value(0).toInt(); // Récupérer l'ID de l'équipement actuel

		// Vérifier si l'ID de l'équipement actuel correspond à l'ID de l'équipement à supprimer
		if (currentIdEquipement == m_idEquipementASupprimer) {
			continue; // Ignorer cet équipement car il doit être supprimé
		}

		QString nomEquipement = query.value(1).toString();
		QString adresseEquipement = query.value(2).toString();
		int nbCanalEquipement = query.value(3).toInt();

		QList<QStandardItem*> rowData;
		rowData << new QStandardItem(nomEquipement);
		rowData << new QStandardItem(adresseEquipement);
		rowData << new QStandardItem(QString::number(nbCanalEquipement));

		// Créer un bouton pour la quatrième colonne "Modifier"
		QPushButton* modifyButton = new QPushButton("Modifier");
		QObject::connect(modifyButton, &QPushButton::clicked, this, [this, currentIdEquipement, nomEquipement, adresseEquipement, nbCanalEquipement]() {
			handleModifyButtonClicked(currentIdEquipement, nomEquipement, adresseEquipement, nbCanalEquipement);
			}); // Connecter le signal clicked du bouton à une lambda fonction
		model->setItem(row, 3, new QStandardItem(""));
		tableView->setIndexWidget(model->index(row, 3), modifyButton);

		// Créer un bouton pour la cinquième colonne "Supprimer"
		QPushButton* deleteButton = new QPushButton("Supprimer");
		deleteButton->setObjectName(QString::number(currentIdEquipement)); // Utiliser l'ID de l'équipement comme nom d'objet
		QObject::connect(deleteButton, &QPushButton::clicked, this, &modulesTestDMXProject::handleDeleteButtonClicked); // Connecter le signal clicked du bouton à une slot
		model->setItem(row, 4, new QStandardItem(""));
		tableView->setIndexWidget(model->index(row, 4), deleteButton);

		// Ajouter les autres données de l'équipement à la même ligne
		for (int i = 0; i < rowData.size(); ++i) {
			model->setItem(row, i, rowData[i]);
		}

		++row; // Incrémenter le numéro de ligne
	}

	// Redimensionner les colonnes pour qu'elles s'adaptent au contenu
	tableView->resizeColumnsToContents();

	// Définir les politiques de taille pour le tableau et le widget parent
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->verticalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// Ajouter le QTableView au layout verticalLayout_3 en prenant tout l'espace disponible
	ui.verticalLayout_3->addWidget(tableView);

	// Réinitialiser l'ID de l'équipement à supprimer
	m_idEquipementASupprimer = -1;



}

void modulesTestDMXProject::supprimerEquipement(int idEquipement)
{
	// Début de la transaction SQL
	QSqlDatabase::database().transaction();

	// Supprimer d'abord les enregistrements dans la table 'champ' liés à l'équipement à supprimer
	QSqlQuery queryDeleteChamp;
	queryDeleteChamp.prepare("DELETE FROM champ WHERE idEquip = :id");
	queryDeleteChamp.bindValue(":id", idEquipement);
	if (!queryDeleteChamp.exec()) {
		qDebug() << "Erreur lors de la suppression des enregistrements dans la table champ :" << queryDeleteChamp.lastError().text();
		// En cas d'erreur, annuler la transaction et sortir de la fonction
		QSqlDatabase::database().rollback();
		return;
	}

	// Ensuite, supprimer l'équipement de la table 'equipement'
	QSqlQuery queryDeleteEquipement;
	queryDeleteEquipement.prepare("DELETE FROM equipement WHERE id = :id");
	queryDeleteEquipement.bindValue(":id", idEquipement);
	if (!queryDeleteEquipement.exec()) {
		qDebug() << "Erreur lors de la suppression de l'équipement :" << queryDeleteEquipement.lastError().text();
		// En cas d'erreur, annuler la transaction et sortir de la fonction
		QSqlDatabase::database().rollback();
		return;
	}

	// Valider la transaction SQL
	QSqlDatabase::database().commit();

	qDebug() << "Équipement supprimé avec succès de la base de données.";

	// Actualiser l'affichage des équipements
	Gerer_un_equipement();
}


void modulesTestDMXProject::handleDeleteButtonClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender()); // Récupérer le bouton qui a émis le signal
	if (button) {
		int idEquipement = button->objectName().toInt(); // Récupérer l'ID de l'équipement à partir du nom d'objet du bouton
		// Appeler la fonction de suppression en passant l'ID de l'équipement à supprimer
		supprimerEquipement(idEquipement);
		Gerer_un_equipement();
	}
}

void modulesTestDMXProject::handleModifyButtonClicked(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement)
{
	// Création d'une boîte de dialogue pour modifier les informations de l'équipement
	QDialog dialog(this);
	dialog.setWindowTitle("Modifier équipement");

	// Création des widgets pour saisir les nouvelles informations
	QLabel* labelNom = new QLabel("Nom :", &dialog);
	QLineEdit* lineEditNom = new QLineEdit(nomEquipement, &dialog);

	QLabel* labelAdresse = new QLabel("Adresse :", &dialog);
	QLineEdit* lineEditAdresse = new QLineEdit(adresseEquipement, &dialog);

	QLabel* labelNbCanal = new QLabel("Nombre de canaux :", &dialog);
	QSpinBox* spinBoxNbCanal = new QSpinBox(&dialog);
	spinBoxNbCanal->setValue(nbCanalEquipement);

	// Création du bouton de confirmation
	QPushButton* buttonConfirm = new QPushButton("Confirmer", &dialog);
	connect(buttonConfirm, &QPushButton::clicked, &dialog, &QDialog::accept);

	// Création du layout pour organiser les widgets
	QVBoxLayout* layout = new QVBoxLayout(&dialog);
	layout->addWidget(labelNom);
	layout->addWidget(lineEditNom);
	layout->addWidget(labelAdresse);
	layout->addWidget(lineEditAdresse);
	layout->addWidget(labelNbCanal);
	layout->addWidget(spinBoxNbCanal);
	layout->addWidget(buttonConfirm);

	// Affichage de la boîte de dialogue
	if (dialog.exec() == QDialog::Accepted) {
		// Récupérer les nouvelles valeurs saisies par l'utilisateur
		QString nouveauNom = lineEditNom->text();
		QString nouvelleAdresse = lineEditAdresse->text();
		int nouveauNbCanal = spinBoxNbCanal->value();

		// Appeler la fonction de modification avec les nouvelles valeurs
		modifierEquipement(idEquipement, nouveauNom, nouvelleAdresse, nouveauNbCanal);
		// Mettre à jour l'affichage des équipements
		afficherEquipements();
		Gerer_un_equipement();

	}
}


void modulesTestDMXProject::modifierEquipement(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement)
{
	// Connexion à la base de données
	QSqlDatabase db = QSqlDatabase::database();
	if (!db.isValid()) {
		qDebug() << "La base de données n'est pas valide.";
		return;
	}

	if (!db.isOpen()) {
		qDebug() << "La base de données n'est pas ouverte.";
		return;
	}

	// Préparation de la requête SQL pour mettre à jour l'équipement
	QSqlQuery query(db);
	query.prepare("UPDATE `equipement` SET `nom` = :nom, `adresse` = :adresse, `nbCanal` = :nbCanal WHERE `id` = :id");
	query.bindValue(":nom", nomEquipement);
	query.bindValue(":adresse", adresseEquipement);
	query.bindValue(":nbCanal", nbCanalEquipement);
	query.bindValue(":id", idEquipement);

	// Exécution de la requête
	if (query.exec()) {
		db.commit(); // Valider les modifications dans la base de données
		qDebug() << "Équipement mis à jour avec succès dans la base de données.";
	}
	else {
		qDebug() << "Erreur lors de la mise à jour de l'équipement :" << query.lastError().text();
	}
}

QListWidget* modulesTestDMXProject::getSceneListWidget() const
{
	return ui.listWidget; // Assurez-vous que 'listWidget' est le nom correct du QListWidget dans votre UI
}