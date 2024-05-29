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
#include <QListWidgetItem>
#include <QScrollArea>
#include <QStringList>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QWizard>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSqlQuery>
#include <QPixmap>


modulesTestDMXProject::modulesTestDMXProject(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// Connexion à la base de données MySQL
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.213");
	db.setDatabaseName("testCodeDMX");
	db.setUserName("root");
	db.setPassword("root");

	if (!db.open()) {
		qDebug() << "Échec de la connexion à la base de données.";
		return;
	}

	consoleMaterielle = new ConsoleMaterielle(ui.verticalSlider, this);

	// Connexion au serveur TCP
	QTcpSocket* socket = new QTcpSocket(this);
	socket->connectToHost("192.168.64.170", 12345);

	// Connexion à l'Arduino
	consoleController = new ConsoleController(this);
	if (!consoleController->connectToArduino("COM8")) {
		qDebug() << "Échec de la connexion à l'Arduino.";
	}

	// Connecter les signaux de ConsoleController aux slots de modulesTestDMXProject
	
	



	// Afficher toutes les scènes existantes
	Scene scene;
	QList<Scene> scenes = scene.getAllScenes();
	foreach(const Scene & s, scenes) {
		ui.listWidget->addItem(s.getNom());
	}

	// Afficher les équipements
	equipement->afficherEquipements(ui.verticalLayoutEquipements);

	afficherScenesCheckbox();
	Gerer_un_equipement();
	fillSceneComboBox2();
	fillEquipComboBox();
	validateSceneEquipment();

	connect(ui.testSceneButton, &QPushButton::clicked, this, &modulesTestDMXProject::testScene);

	// Envoyer les noms des scènes à l'Arduino
	QStringList sceneNames;
	foreach(const Scene & s, scenes) {
		sceneNames.append(s.getNom());
	}
	consoleController->sendSceneNames(sceneNames);

	int nouvelleLargeur = 400; // Par exemple, vous pouvez définir la largeur souhaitée
	int nouvelleHauteur = 300; // Par exemple, vous pouvez définir la hauteur souhaitée

	// Insérer une image dans le label_10
	QString path = QDir::currentPath() + "/image/page.png";
	QPixmap image(path);

	// Redimensionner l'image avec les nouvelles dimensions
	image = image.scaled(nouvelleLargeur, nouvelleHauteur, Qt::KeepAspectRatio); // Redimensionner l'image avec les dimensions spécifiées

	// Redimensionner le QLabel pour qu'il puisse afficher l'image entière
	ui.label_10->resize(image.size());

	// Afficher l'image dans le QLabel
	ui.label_10->setPixmap(image);

	// Mettre à jour le layout si nécessaire
	ui.label_10->updateGeometry();
}

modulesTestDMXProject::~modulesTestDMXProject()
{
}

void modulesTestDMXProject::sendData(const QByteArray& data)
{
	quint16 blockSize = data.size();
	out << blockSize << data;
	tcpSocket->flush(); // Appeler flush() sur l'objet QTcpSocket

	QByteArray response;
	in >> response;

	// Traiter la réponse du serveur
}

void modulesTestDMXProject::sendSceneNamesToArduino(const QStringList& scenes)
{
	if (!consoleController->isConnected()) {
		qDebug() << "Not connected to Arduino";
		return;
	}

	foreach(const QString & scene, scenes) {
		QByteArray data = scene.toUtf8() + "\n";
		qDebug() << "Sending scene:" << scene;
		consoleController->sendData(data);
		// Ajout d'un délai pour s'assurer que chaque ligne est bien envoyée
	}
}

void modulesTestDMXProject::on_pushButtonValider_clicked()
{
	// Récupérer le contenu de lineEditNomScene
	QString nomScene = ui.lineEditNomScene->text();

	// Exécuter une requête SQL pour insérer les données dans la table scene
	scene->insertScene(nomScene);

	ui.listWidget->clear();

	scene->afficherScenes(ui.listWidget);


}

void modulesTestDMXProject::on_actionCreer_une_sc_ne_triggered()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void modulesTestDMXProject::on_actionConfigurer_une_sc_ne_2_triggered()
{
	equipement->afficherEquipements(ui.verticalLayoutEquipements);
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

void modulesTestDMXProject::on_actionTester_une_scene_triggered()
{
	fillSceneComboBox();
	ui.stackedWidget->setCurrentIndex(6);
}

void modulesTestDMXProject::on_actionArduino_triggered()
{
	ui.stackedWidget->setCurrentIndex(8);
}


// Modifiez votre slot on_buttonEquip_clicked pour appeler cette fonction
void modulesTestDMXProject::on_buttonEquip_clicked() {
	QString nomEquipement = ui.nomEquipEdit->text();
	QString adresseEquipement = ui.adresseEquipEdit->text();
	int startChannelAddress = ui.adresseEquipEdit->text().toInt(); // Convertir le texte en entier
	int nbCanaux = ui.nbCannauxEdit->text().toInt(); // Convertir le texte en entier
	createChannelLabelsAndLineEdits(nbCanaux, startChannelAddress);

	equipement->insertEquipement(nomEquipement, adresseEquipement, nbCanaux);
}


void modulesTestDMXProject::afficherScenesCheckbox()
{

	// Exécuter une requête pour récupérer tous les équipements existants
	QSqlQuery query("SELECT nom FROM scene");

	// Créer un widget de défilement pour contenir le layout des scènes
	QScrollArea *scrollArea = new QScrollArea;
	QWidget *scrollWidget = new QWidget;
	QVBoxLayout *layoutScenes = new QVBoxLayout(scrollWidget);
	scrollWidget->setLayout(layoutScenes);
	scrollArea->setWidget(scrollWidget);
	scrollArea->setWidgetResizable(true);

	// Récupérer le layout pour les scènes
	QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui.verticalLayoutScenes->layout());
	if (!mainLayout) {
		qDebug() << "Erreur : le layout des scènes est invalide.";
		return;
	}

	// Effacer le contenu existant du layout principal
	QLayoutItem *child;
	while ((child = mainLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Garder une liste de toutes les cases à cocher
	QList<QCheckBox *> checkBoxes;

	// Parcourir les résultats de la requête et ajouter chaque scène en tant que case à cocher
	while (query.next()) {
		QString nomScene = query.value(0).toString();

		// Créer une case à cocher pour la scène
		QCheckBox *checkBox = new QCheckBox(nomScene);
		// Ajouter la case à cocher au layout des scènes
		layoutScenes->addWidget(checkBox);
		checkBoxes.append(checkBox); // Ajouter la case à cocher à la liste
	}

	// Définir une largeur fixe pour le widget de défilement
	scrollArea->setFixedWidth(200); // Vous pouvez ajuster cette valeur selon vos besoins

	// Ajouter le widget de défilement au layout principal
	mainLayout->addWidget(scrollArea);

	// Connecter le signal toggled() de chaque case à cocher à une fonction de gestion
	for (QCheckBox *checkBox : checkBoxes) {
		connect(checkBox, &QCheckBox::toggled, [this, checkBoxes, checkBox](bool checked) {
			if (checked) {
				// Désélectionner toutes les autres cases à cocher sauf celle qui vient d'être cochée
				for (QCheckBox *otherCheckBox : checkBoxes) {
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
	QLayoutItem *child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr) {
		QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(child->widget());
		if (lineEdit) {
			QString nom = lineEdit->text();
			champ->insertChamp(idEquip, idNumCanal, nom);


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
	QWidget *widgetEquipements = ui.verticalLayoutEquipements->itemAt(0)->widget();
	QWidget *widgetScenes = ui.verticalLayoutScenes->itemAt(0)->widget();

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

	// Récupérer l'ID de la scène sélectionnée
	QSqlQuery sceneQuery;
	sceneQuery.prepare("SELECT id FROM scene WHERE nom = :nom");
	sceneQuery.bindValue(":nom", selectedScene);
	if (!sceneQuery.exec() || !sceneQuery.next()) {
		qDebug() << "Erreur lors de l'exécution de la requête sceneQuery:" << sceneQuery.lastError().text();
		ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
		return;
	}
	int idScene = sceneQuery.value(0).toInt();

	// Vérifier si des canaux existent déjà pour cette scène
	QSqlQuery checkCanauxQuery;
	checkCanauxQuery.prepare("SELECT COUNT(*) FROM canaux WHERE idScene = :idScene");
	checkCanauxQuery.bindValue(":idScene", idScene);
	if (!checkCanauxQuery.exec() || !checkCanauxQuery.next()) {
		qDebug() << "Erreur lors de l'exécution de la requête checkCanauxQuery:" << checkCanauxQuery.lastError().text();
		ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
		return;
	}
	int countCanaux = checkCanauxQuery.value(0).toInt();

	// Si des canaux existent déjà, demander confirmation à l'utilisateur
	if (countCanaux > 0) {
		QMessageBox msgBox;
		msgBox.setText("La scène sélectionnée possède déjà une configuration. Voulez-vous la supprimer ?");
		msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok) {
			// Supprimer les canaux existants
			QSqlQuery deleteQuery;
			deleteQuery.prepare("DELETE FROM canaux WHERE idScene = :idScene");
			deleteQuery.bindValue(":idScene", idScene);
			if (!deleteQuery.exec()) {
				qDebug() << "Erreur lors de l'exécution de la requête deleteQuery:" << deleteQuery.lastError().text();
				ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
				return;
			}
		}
		else {
			// Annuler l'opération
			ui.stackedWidget->setCurrentIndex(1); // Rediriger après annulation
			return;
		}
	}

	// Créer un QWizard pour configurer les canaux de chaque équipement dans la scène
	QWizard* wizard = new QWizard(this);

	// Créer une page pour chaque équipement sélectionné
	for (const QString& equipementName : selectedEquipements) {
		QWizardPage* page = new QWizardPage(this);
		QVBoxLayout* layout = new QVBoxLayout(page);
		layout->addWidget(new QLabel(QString("Configurer les canaux pour %1 :").arg(equipementName), page));

		// Récupérer les informations sur les canaux pour cet équipement
		QSqlQuery query(QString("SELECT idNumCanal, nom FROM champ WHERE idEquip = (SELECT id FROM equipement WHERE nom = '%1') ORDER BY idNumCanal").arg(equipementName));

		// Ajouter des widgets pour chaque canal de cet équipement
		QList<QPair<int, int>> channelData; // Ajouter une liste de paires pour stocker les valeurs des QSpinBox
		while (query.next()) {
			QString canalName = query.value(1).toString();
			int canalNumber = query.value(0).toInt();
			QLabel* label = new QLabel(QString("%1 :").arg(canalName), page);
			QSpinBox* spinBox = new QSpinBox(page);
			spinBox->setObjectName(QString("spinBox_%1").arg(canalNumber));
			spinBox->setMaximum(255); // Définir la valeur maximale du QSpinBox à 255
			layout->addWidget(label);
			layout->addWidget(spinBox);

			// Ajouter la valeur du QSpinBox à la liste channelData
			channelData.append(qMakePair(canalNumber, spinBox->value()));
		}

		// Ajouter la liste channelData à la page du QWizard
		page->setProperty("channelData", QVariant::fromValue(channelData));

		wizard->addPage(page);
	}

	// Connecter le signal accepted() du QWizard à un slot pour enregistrer les paramètres de canal dans la base de données
	connect(wizard, &QWizard::accepted, this, &modulesTestDMXProject::saveSettings);

	// Connecter les signaux finished et rejected pour gérer la redirection
	connect(wizard, &QWizard::rejected, [this, wizard]() {
		ui.stackedWidget->setCurrentIndex(1);
		wizard->deleteLater();
	});
	connect(wizard, &QWizard::finished, [this, wizard]() {
		ui.stackedWidget->setCurrentIndex(1);
		wizard->deleteLater();
	});

	// Afficher le QWizard
	wizard->show();
}





void modulesTestDMXProject::clearForm()
{
	// Effacer le layout existant
	QLayoutItem *child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Vider la liste des pointeurs de QLineEdit
	m_lineEdits.clear();
}


void modulesTestDMXProject::Gerer_un_equipement()
{
	QLayoutItem* child;

	// Supprimer tous les widgets existants dans le layout
	while ((child = ui.verticalLayout_3->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Vider la liste des pointeurs de QLineEdit
	m_lineEdits.clear();

	// Créer un QLabel avec du texte en grand au milieu
	QLabel* gererEquipementLabel = new QLabel("Gerer un equipement");
	gererEquipementLabel->setAlignment(Qt::AlignCenter);
	QFont font = gererEquipementLabel->font();
	font.setPointSize(24); // Définir la taille de la police
	gererEquipementLabel->setFont(font);

	// Utiliser un QHBoxLayout pour centrer le QLabel horizontalement
	QHBoxLayout* labelLayout = new QHBoxLayout;
	labelLayout->addWidget(gererEquipementLabel);
	ui.verticalLayout_3->addLayout(labelLayout);

	// Créer un bouton "Nouvel équipement" plus petit mais plus épais
	QPushButton* newEquipmentButton = new QPushButton("Nouvel equipement");
	newEquipmentButton->setFixedSize(150, 50); // Définir une taille fixe pour le bouton
	newEquipmentButton->setStyleSheet("QPushButton { padding: 10px; font-weight: bold; }"); // Définir du padding et un texte en gras

	QObject::connect(newEquipmentButton, &QPushButton::clicked, this, &modulesTestDMXProject::handleNewEquipmentButtonClicked);
	ui.verticalLayout_3->addWidget(newEquipmentButton);

	// Créer un nouveau QTableView
	QTableView* tableView = new QTableView;

	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

	// Créer un modèle de tableau vide avec cinq colonnes
	QStandardItemModel* model = new QStandardItemModel(0, 5);
	tableView->setModel(model);

	// Définir les en-têtes de colonne
	model->setHorizontalHeaderLabels(QStringList() << "Nom de l'equipement" << "Adresse" << "Nombre de Canal" << "Modifier" << "Supprimer");

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


void modulesTestDMXProject::handleNewEquipmentButtonClicked()
{
	// Logique pour ajouter un nouvel équipement
	QDialog dialog(this);
	dialog.setWindowTitle("Ajouter un nouvel equipement");

	QFormLayout form(&dialog);

	// Ajout des champs d'entrée
	QLineEdit* nomEdit = new QLineEdit(&dialog);
	form.addRow("Nom de l'equipement:", nomEdit);

	QLineEdit* adresseEdit = new QLineEdit(&dialog);
	form.addRow("Adresse:", adresseEdit);

	QLineEdit* canauxEdit = new QLineEdit(&dialog);
	form.addRow("Nombre de canaux:", canauxEdit);

	// Ajout des boutons
	QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
	form.addRow(&buttonBox);

	// Connexion des boutons
	QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

	// Afficher la boîte de dialogue
	if (dialog.exec() == QDialog::Accepted) {
		QString nom = nomEdit->text();
		QString adresse = adresseEdit->text();
		int canaux = canauxEdit->text().toInt();

		// Ajouter l'équipement à la base de données
		QSqlQuery query;
		query.prepare("INSERT INTO equipement (nom, adresse, nbCanal) VALUES (?, ?, ?)");
		query.addBindValue(nom);
		query.addBindValue(adresse);
		query.addBindValue(canaux);

		if (query.exec()) {
			// Réafficher la liste des équipements
			Gerer_un_equipement();

			// Créer une nouvelle fenêtre pour les étiquettes de canal et les champs de texte associés
			QDialog channelDialog(this);
			channelDialog.setWindowTitle("Noms des canaux");

			QVBoxLayout channelLayout(&channelDialog);

			// Récupérer la valeur de l'adresse pour l'incrémentation
			int adresseInt = adresse.toInt();

			// Affichez les QLabel et les QLineEdit pour chaque canal
			QVector<QLineEdit*> channelLineEditVector;
			for (int i = 0; i < canaux; ++i) {
				QLabel* label = new QLabel("Nom du canal " + QString::number(adresseInt + i) + " :");
				QLineEdit* lineEdit = new QLineEdit(&channelDialog);
				channelLayout.addWidget(label);
				channelLayout.addWidget(lineEdit);
				channelLineEditVector.append(lineEdit);
			}

			// Ajouter les boutons OK et Annuler à la fenêtre des canaux
			QDialogButtonBox channelButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &channelDialog);
			channelLayout.addWidget(&channelButtonBox);

			// Connexion des boutons
			QObject::connect(&channelButtonBox, &QDialogButtonBox::accepted, [&]() {
				// Insérer les noms des canaux dans la base de données
				QSqlQuery channelQuery;

				// Préparer la requête pour l'insertion des noms des canaux
				channelQuery.prepare("INSERT INTO champ (idEquip, idNumCanal, nom) VALUES (:idEquip, :idNumCanal, :nom)");

				// Récupérer l'ID de l'équipement inséré
				int idEquip = query.lastInsertId().toInt();

				// Parcourir les noms des canaux et les insérer dans la base de données
				for (int i = 0; i < canaux; ++i) {
					QString channelName = channelLineEditVector[i]->text();

					channelQuery.bindValue(":idEquip", idEquip);
					channelQuery.bindValue(":idNumCanal", adresseInt + i); // Identifiant numérique du canal
					channelQuery.bindValue(":nom", channelName);

					if (!channelQuery.exec()) {
						qDebug() << "Erreur lors de l'insertion des données dans la table champ:" << channelQuery.lastError().text();
						QMessageBox::warning(this, "Erreur", "Échec de l'ajout des noms des canaux à la base de données: " + channelQuery.lastError().text());
						break;
					}
				}

				// Fermer la fenêtre des canaux une fois que l'insertion est terminée
				channelDialog.accept();
			});

			QObject::connect(&channelButtonBox, &QDialogButtonBox::rejected, &channelDialog, &QDialog::reject);

			// Afficher la fenêtre des canaux
			channelDialog.exec();
		}
		else {
			QMessageBox::warning(this, "Erreur", "Échec de l'ajout de l'équipement à la base de données.");
		}
	}
}



void modulesTestDMXProject::handleDeleteButtonClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender()); // Récupérer le bouton qui a émis le signal
	if (button) {
		int idEquipement = button->objectName().toInt(); // Récupérer l'ID de l'équipement à partir du nom d'objet du bouton
		// Appeler la fonction de suppression en passant l'ID de l'équipement à supprimer
		equipement->supprimerEquipement(idEquipement, this);
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
		equipement->updateEquipement(idEquipement, nouveauNom, nouvelleAdresse, nouveauNbCanal);
		// Mettre à jour l'affichage des équipements
		equipement->afficherEquipements(ui.verticalLayoutEquipements);

		Gerer_un_equipement();

	}
}


void modulesTestDMXProject::saveSettings() {
	// Récupérer l'ID de la scène sélectionnée
	int idScene = scene->getSceneId(m_selectedScene); //getSceneId(m_selectedScene);

	QWizard* wizard = qobject_cast<QWizard*>(sender());
	if (wizard) {
		QList<QWizardPage*> pages = wizard->findChildren<QWizardPage*>();
		for (QList<QWizardPage*>::iterator it = pages.begin(); it != pages.end(); ++it) {
			QWizardPage* page = *it;

			// Récupérer les valeurs actuelles des QSpinBox dans la page actuelle
			QList<QPair<int, int>> channelData;
			QList<QSpinBox*> spinBoxes = page->findChildren<QSpinBox*>();
			for (QSpinBox* spinBox : spinBoxes) {
				// Récupérer le numéro de canal à partir du nom d'objet du QSpinBox
				QString objectName = spinBox->objectName();
				int index = objectName.lastIndexOf('_');
				int numCanal = objectName.mid(index + 1).toInt();

				// Récupérer la valeur actuelle du QSpinBox
				int valeur = spinBox->value();

				// Ajouter la paire (numCanal, valeur) à la liste channelData
				channelData.append(qMakePair(numCanal, valeur));
			}

			// Enregistrer les paramètres de canal pour cet équipement dans la base de données
			canal->insertCanal(idScene, channelData);
		}
	}

	// Fermer le QWizard
	qobject_cast<QWizard*>(sender())->close();
}

void modulesTestDMXProject::sendDMXFrame()
{
	// Récupérer l'ID de la scène sélectionnée dans votre interface utilisateur
	int sceneId = 5; //ui->sceneComboBox->currentData().toInt()

	// Récupérer les valeurs des canaux correspondants dans votre base de données
	QSqlQuery query;
	query.prepare("SELECT numCanal, valeur FROM canaux WHERE idScene = :sceneId");
	query.bindValue(":sceneId", sceneId);
	query.exec();

	// Construire la trame DMX
	QByteArray dmxFrame(512, 0);
	while (query.next()) {
		int numCanal = query.value(0).toInt();
		int valeur = query.value(1).toInt();
		dmxFrame[numCanal - 1] = valeur;
	}

	// Envoyer la trame DMX à votre serveur Linux en utilisant une connexion TCP
	QTcpSocket socket;
	socket.connectToHost("192.168.64.170", 12345); // Remplacez par l'adresse IP et le port de votre serveur Linux
	if (socket.waitForConnected()) {
		socket.write(dmxFrame);
		socket.waitForBytesWritten();
		socket.disconnectFromHost();
	}
	else {
		qDebug() << "Erreur : impossible de se connecter au serveur.";
	}
}

void modulesTestDMXProject::fillSceneComboBox()
{
	QSqlQuery query;
	query.prepare("SELECT nom FROM scene");
	query.exec();

	while (query.next()) {
		ui.sceneComboBox->addItem(query.value(0).toString());
	}
}

void modulesTestDMXProject::testScene()
{
	// Récupérer le nom de la scène sélectionnée dans le QComboBox
	QString sceneName = ui.sceneComboBox->currentText();

	// Sélectionner l'ID de la scène correspondant au nom sélectionné dans la base de données
	QSqlQuery query;
	query.prepare("SELECT id FROM scene WHERE nom = :sceneName");
	query.bindValue(":sceneName", sceneName);
	query.exec();

	int sceneId = -1;
	if (query.next()) {
		sceneId = query.value(0).toInt();
	}

	if (sceneId != -1) {
		// Convertir l'ID de la scène en QByteArray
		QByteArray sceneIdData = QByteArray::number(sceneId);

		// Envoyer l'ID de la scène au serveur Linux en utilisant une connexion TCP
		QTcpSocket socket;
		socket.connectToHost("192.168.64.170", 12345); // Remplacez par l'adresse IP et le port de votre serveur Linux
		if (socket.waitForConnected()) {
			socket.write(sceneIdData);
			socket.waitForBytesWritten();
			socket.disconnectFromHost();
		}
		else {
			qDebug() << "Erreur : impossible de se connecter au serveur.";
		}
	}
	else {
		qDebug() << "Erreur : scène non trouvée.";
	}
}


void afficherEmplacementsLibresDansTrame() {
	// Connexion à la base de données et récupération des adresses et du nombre de canaux de chaque équipement
	QSqlQuery query("SELECT adresse, nbCanal FROM equipement ORDER BY adresse ASC;");

	// Structure de données pour stocker les emplacements des équipements dans la trame
	std::map<int, int> emplacements;

	// Assignation des emplacements pour chaque équipement
	while (query.next()) {
		int adresse = query.value(0).toInt();
		int nbCanal = query.value(1).toInt();
		int emplacement = adresse + nbCanal; // Calcul de l'emplacement dans la trame
		emplacements[emplacement] = nbCanal; // Stockage de l'emplacement dans la map
	}

	// Calcul des emplacements libres dans la trame
	int dernierEmplacementUtilise = 0;
	int nombreEmplacementsLibres = 0;
	for (const auto& pair : emplacements) {
		if (pair.first > dernierEmplacementUtilise) {
			nombreEmplacementsLibres += pair.first - dernierEmplacementUtilise;
		}
		dernierEmplacementUtilise = pair.first + pair.second;
	}
	nombreEmplacementsLibres += 255 - dernierEmplacementUtilise;

	// Affichage des emplacements libres dans une boîte de dialogue
	QMessageBox::information(nullptr, "Emplacements libres dans la trame",
		QString("Nombre d'emplacements libres dans la trame : %1").arg(nombreEmplacementsLibres));
}

void modulesTestDMXProject::fillSceneComboBox2() {
	Scene scene;
	QList<Scene> scenes = scene.getAllScenes();
	foreach(const Scene & s, scenes) {
		ui.SceneComboBox->addItem(s.getNom());
	}
}


void modulesTestDMXProject::fillEquipComboBox() {
	Equipement equip;
	QList<Equipement> equips = equip.getAllEquipements();
	foreach(const Equipement & e, equips) {
		ui.EquipComboBox->addItem(e.getNom());
	}
}


void modulesTestDMXProject::validateSceneEquipment()
{
	int sceneIndex = ui.SceneComboBox->currentIndex();
	int equipIndex = ui.EquipComboBox->currentIndex();

	if (sceneIndex != -1 && equipIndex != -1) {
		showEquipmentFields(equipIndex);
	}
}

void modulesTestDMXProject::updateSliderValue(int value)
{
	ui.verticalSlider->setValue(value);

	if (currentFieldIndex < sliderValues.size()) {
		sliderValues[currentFieldIndex] = value;
	}
	else {
		sliderValues.append(value);
	}
}

void modulesTestDMXProject::showEquipmentFields(int equipIndex)
{
	// Logique pour afficher les champs de l'équipement
	qDebug() << "Showing fields for equipment at index:" << equipIndex;

	// Réinitialiser l'index de champ actuel et les valeurs des sliders
	currentFieldIndex = 0;
	sliderValues.clear();
}

void modulesTestDMXProject::onConfirmButtonPressed()
{
	// Enregistrer la valeur actuelle et passer au champ suivant
	int value = ui.verticalSlider->value();
	if (currentFieldIndex < sliderValues.size()) {
		sliderValues[currentFieldIndex] = value;
	}
	else {
		sliderValues.append(value);
	}

	currentFieldIndex++;
	qDebug() << "Current field index:" << currentFieldIndex << "Slider values:" << sliderValues;

	// Logique pour gérer l'affichage du champ suivant
}