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

	// Connexion � la base de donn�es MySQL
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.213");
	db.setDatabaseName("testCodeDMX");
	db.setUserName("root");
	db.setPassword("root");

	if (!db.open()) {
		qDebug() << "�chec de la connexion � la base de donn�es.";
		return;
	}

	consoleMaterielle = new ConsoleMaterielle(ui.verticalSlider, this);

	// Connexion au serveur TCP
	QTcpSocket* socket = new QTcpSocket(this);
	socket->connectToHost("192.168.64.170", 12345);

	// Connexion � l'Arduino
	consoleController = new ConsoleController(this);
	if (!consoleController->connectToArduino("COM8")) {
		qDebug() << "�chec de la connexion � l'Arduino.";
	}

	// Connecter les signaux de ConsoleController aux slots de modulesTestDMXProject
	
	



	// Afficher toutes les sc�nes existantes
	Scene scene;
	QList<Scene> scenes = scene.getAllScenes();
	foreach(const Scene & s, scenes) {
		ui.listWidget->addItem(s.getNom());
	}

	// Afficher les �quipements
	equipement->afficherEquipements(ui.verticalLayoutEquipements);

	afficherScenesCheckbox();
	Gerer_un_equipement();
	fillSceneComboBox2();
	fillEquipComboBox();
	validateSceneEquipment();

	connect(ui.testSceneButton, &QPushButton::clicked, this, &modulesTestDMXProject::testScene);

	// Envoyer les noms des sc�nes � l'Arduino
	QStringList sceneNames;
	foreach(const Scene & s, scenes) {
		sceneNames.append(s.getNom());
	}
	consoleController->sendSceneNames(sceneNames);

	int nouvelleLargeur = 400; // Par exemple, vous pouvez d�finir la largeur souhait�e
	int nouvelleHauteur = 300; // Par exemple, vous pouvez d�finir la hauteur souhait�e

	// Ins�rer une image dans le label_10
	QString path = QDir::currentPath() + "/image/page.png";
	QPixmap image(path);

	// Redimensionner l'image avec les nouvelles dimensions
	image = image.scaled(nouvelleLargeur, nouvelleHauteur, Qt::KeepAspectRatio); // Redimensionner l'image avec les dimensions sp�cifi�es

	// Redimensionner le QLabel pour qu'il puisse afficher l'image enti�re
	ui.label_10->resize(image.size());

	// Afficher l'image dans le QLabel
	ui.label_10->setPixmap(image);

	// Mettre � jour le layout si n�cessaire
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

	// Traiter la r�ponse du serveur
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
		// Ajout d'un d�lai pour s'assurer que chaque ligne est bien envoy�e
	}
}

void modulesTestDMXProject::on_pushButtonValider_clicked()
{
	// R�cup�rer le contenu de lineEditNomScene
	QString nomScene = ui.lineEditNomScene->text();

	// Ex�cuter une requ�te SQL pour ins�rer les donn�es dans la table scene
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

	// Ex�cuter une requ�te pour r�cup�rer tous les �quipements existants
	QSqlQuery query("SELECT nom FROM scene");

	// Cr�er un widget de d�filement pour contenir le layout des sc�nes
	QScrollArea *scrollArea = new QScrollArea;
	QWidget *scrollWidget = new QWidget;
	QVBoxLayout *layoutScenes = new QVBoxLayout(scrollWidget);
	scrollWidget->setLayout(layoutScenes);
	scrollArea->setWidget(scrollWidget);
	scrollArea->setWidgetResizable(true);

	// R�cup�rer le layout pour les sc�nes
	QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui.verticalLayoutScenes->layout());
	if (!mainLayout) {
		qDebug() << "Erreur : le layout des sc�nes est invalide.";
		return;
	}

	// Effacer le contenu existant du layout principal
	QLayoutItem *child;
	while ((child = mainLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Garder une liste de toutes les cases � cocher
	QList<QCheckBox *> checkBoxes;

	// Parcourir les r�sultats de la requ�te et ajouter chaque sc�ne en tant que case � cocher
	while (query.next()) {
		QString nomScene = query.value(0).toString();

		// Cr�er une case � cocher pour la sc�ne
		QCheckBox *checkBox = new QCheckBox(nomScene);
		// Ajouter la case � cocher au layout des sc�nes
		layoutScenes->addWidget(checkBox);
		checkBoxes.append(checkBox); // Ajouter la case � cocher � la liste
	}

	// D�finir une largeur fixe pour le widget de d�filement
	scrollArea->setFixedWidth(200); // Vous pouvez ajuster cette valeur selon vos besoins

	// Ajouter le widget de d�filement au layout principal
	mainLayout->addWidget(scrollArea);

	// Connecter le signal toggled() de chaque case � cocher � une fonction de gestion
	for (QCheckBox *checkBox : checkBoxes) {
		connect(checkBox, &QCheckBox::toggled, [this, checkBoxes, checkBox](bool checked) {
			if (checked) {
				// D�s�lectionner toutes les autres cases � cocher sauf celle qui vient d'�tre coch�e
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
	// Supprimez les widgets pr�c�demment ajout�s s'il y en a
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
		startChannelAddress++; // Incr�menter l'adresse de d�part pour le prochain �quipement
	}
}


void modulesTestDMXProject::on_validateButtonEquip_clicked() {
	// R�cup�rer l'ID du dernier �quipement ins�r�
	QSqlQuery query;
	query.exec("SELECT LAST_INSERT_ID()");
	int idEquip = query.next() ? query.value(0).toInt() : -1;

	if (idEquip == -1) {
		qDebug() << "Erreur : impossible de r�cup�rer l'ID du dernier �quipement ins�r�.";
		return;
	}

	// R�cup�rer la valeur de numCanal
	int idNumCanal = ui.adresseEquipEdit->text().toInt();

	// Parcourir tous les QLineEdit et ins�rer les donn�es dans la table "champ"
	QLayoutItem *child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr) {
		QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(child->widget());
		if (lineEdit) {
			QString nom = lineEdit->text();
			champ->insertChamp(idEquip, idNumCanal, nom);


			idNumCanal++; // Incr�menter idNumCanal pour le prochain enregistrement
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

	// R�cup�rer les �quipements s�lectionn�s
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
				// R�cup�rer le nom de l'�quipement � partir du texte de la QCheckBox
				QString equipementName = checkBox->text();
				selectedEquipements.append(equipementName);
			}
		}
	}

	// R�cup�rer la sc�ne s�lectionn�e
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
				// R�cup�rer le nom de la sc�ne � partir du texte de la QCheckBox
				QString sceneName = checkBox->text();
				selectedScene = sceneName;
				m_selectedScene = sceneName;
				break;
			}
		}
	}

	if (!selectedEquipements.isEmpty() && !selectedScene.isEmpty())
	{

		// Cr�er le formulaire dynamique
		createFormForSelectedEquipements(selectedEquipements, selectedScene);

		// Passer � la page d'index 4
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

	// R�cup�rer l'ID de la sc�ne s�lectionn�e
	QSqlQuery sceneQuery;
	sceneQuery.prepare("SELECT id FROM scene WHERE nom = :nom");
	sceneQuery.bindValue(":nom", selectedScene);
	if (!sceneQuery.exec() || !sceneQuery.next()) {
		qDebug() << "Erreur lors de l'ex�cution de la requ�te sceneQuery:" << sceneQuery.lastError().text();
		ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
		return;
	}
	int idScene = sceneQuery.value(0).toInt();

	// V�rifier si des canaux existent d�j� pour cette sc�ne
	QSqlQuery checkCanauxQuery;
	checkCanauxQuery.prepare("SELECT COUNT(*) FROM canaux WHERE idScene = :idScene");
	checkCanauxQuery.bindValue(":idScene", idScene);
	if (!checkCanauxQuery.exec() || !checkCanauxQuery.next()) {
		qDebug() << "Erreur lors de l'ex�cution de la requ�te checkCanauxQuery:" << checkCanauxQuery.lastError().text();
		ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
		return;
	}
	int countCanaux = checkCanauxQuery.value(0).toInt();

	// Si des canaux existent d�j�, demander confirmation � l'utilisateur
	if (countCanaux > 0) {
		QMessageBox msgBox;
		msgBox.setText("La sc�ne s�lectionn�e poss�de d�j� une configuration. Voulez-vous la supprimer ?");
		msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok) {
			// Supprimer les canaux existants
			QSqlQuery deleteQuery;
			deleteQuery.prepare("DELETE FROM canaux WHERE idScene = :idScene");
			deleteQuery.bindValue(":idScene", idScene);
			if (!deleteQuery.exec()) {
				qDebug() << "Erreur lors de l'ex�cution de la requ�te deleteQuery:" << deleteQuery.lastError().text();
				ui.stackedWidget->setCurrentIndex(1); // Rediriger en cas d'erreur
				return;
			}
		}
		else {
			// Annuler l'op�ration
			ui.stackedWidget->setCurrentIndex(1); // Rediriger apr�s annulation
			return;
		}
	}

	// Cr�er un QWizard pour configurer les canaux de chaque �quipement dans la sc�ne
	QWizard* wizard = new QWizard(this);

	// Cr�er une page pour chaque �quipement s�lectionn�
	for (const QString& equipementName : selectedEquipements) {
		QWizardPage* page = new QWizardPage(this);
		QVBoxLayout* layout = new QVBoxLayout(page);
		layout->addWidget(new QLabel(QString("Configurer les canaux pour %1 :").arg(equipementName), page));

		// R�cup�rer les informations sur les canaux pour cet �quipement
		QSqlQuery query(QString("SELECT idNumCanal, nom FROM champ WHERE idEquip = (SELECT id FROM equipement WHERE nom = '%1') ORDER BY idNumCanal").arg(equipementName));

		// Ajouter des widgets pour chaque canal de cet �quipement
		QList<QPair<int, int>> channelData; // Ajouter une liste de paires pour stocker les valeurs des QSpinBox
		while (query.next()) {
			QString canalName = query.value(1).toString();
			int canalNumber = query.value(0).toInt();
			QLabel* label = new QLabel(QString("%1 :").arg(canalName), page);
			QSpinBox* spinBox = new QSpinBox(page);
			spinBox->setObjectName(QString("spinBox_%1").arg(canalNumber));
			spinBox->setMaximum(255); // D�finir la valeur maximale du QSpinBox � 255
			layout->addWidget(label);
			layout->addWidget(spinBox);

			// Ajouter la valeur du QSpinBox � la liste channelData
			channelData.append(qMakePair(canalNumber, spinBox->value()));
		}

		// Ajouter la liste channelData � la page du QWizard
		page->setProperty("channelData", QVariant::fromValue(channelData));

		wizard->addPage(page);
	}

	// Connecter le signal accepted() du QWizard � un slot pour enregistrer les param�tres de canal dans la base de donn�es
	connect(wizard, &QWizard::accepted, this, &modulesTestDMXProject::saveSettings);

	// Connecter les signaux finished et rejected pour g�rer la redirection
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

	// Cr�er un QLabel avec du texte en grand au milieu
	QLabel* gererEquipementLabel = new QLabel("Gerer un equipement");
	gererEquipementLabel->setAlignment(Qt::AlignCenter);
	QFont font = gererEquipementLabel->font();
	font.setPointSize(24); // D�finir la taille de la police
	gererEquipementLabel->setFont(font);

	// Utiliser un QHBoxLayout pour centrer le QLabel horizontalement
	QHBoxLayout* labelLayout = new QHBoxLayout;
	labelLayout->addWidget(gererEquipementLabel);
	ui.verticalLayout_3->addLayout(labelLayout);

	// Cr�er un bouton "Nouvel �quipement" plus petit mais plus �pais
	QPushButton* newEquipmentButton = new QPushButton("Nouvel equipement");
	newEquipmentButton->setFixedSize(150, 50); // D�finir une taille fixe pour le bouton
	newEquipmentButton->setStyleSheet("QPushButton { padding: 10px; font-weight: bold; }"); // D�finir du padding et un texte en gras

	QObject::connect(newEquipmentButton, &QPushButton::clicked, this, &modulesTestDMXProject::handleNewEquipmentButtonClicked);
	ui.verticalLayout_3->addWidget(newEquipmentButton);

	// Cr�er un nouveau QTableView
	QTableView* tableView = new QTableView;

	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

	// Cr�er un mod�le de tableau vide avec cinq colonnes
	QStandardItemModel* model = new QStandardItemModel(0, 5);
	tableView->setModel(model);

	// D�finir les en-t�tes de colonne
	model->setHorizontalHeaderLabels(QStringList() << "Nom de l'equipement" << "Adresse" << "Nombre de Canal" << "Modifier" << "Supprimer");

	// Ex�cuter une requ�te pour r�cup�rer tous les �quipements existants
	QSqlQuery query("SELECT id, nom, adresse, nbCanal FROM equipement");

	int row = 0; // Variable pour garder une trace du num�ro de ligne

	// Parcourir les r�sultats de la requ�te et ajouter chaque �quipement en tant que ligne dans le mod�le de tableau
	while (query.next()) {
		int currentIdEquipement = query.value(0).toInt(); // R�cup�rer l'ID de l'�quipement actuel

		// V�rifier si l'ID de l'�quipement actuel correspond � l'ID de l'�quipement � supprimer
		if (currentIdEquipement == m_idEquipementASupprimer) {
			continue; // Ignorer cet �quipement car il doit �tre supprim�
		}

		QString nomEquipement = query.value(1).toString();
		QString adresseEquipement = query.value(2).toString();
		int nbCanalEquipement = query.value(3).toInt();

		QList<QStandardItem*> rowData;
		rowData << new QStandardItem(nomEquipement);
		rowData << new QStandardItem(adresseEquipement);
		rowData << new QStandardItem(QString::number(nbCanalEquipement));

		// Cr�er un bouton pour la quatri�me colonne "Modifier"
		QPushButton* modifyButton = new QPushButton("Modifier");
		QObject::connect(modifyButton, &QPushButton::clicked, this, [this, currentIdEquipement, nomEquipement, adresseEquipement, nbCanalEquipement]() {
			handleModifyButtonClicked(currentIdEquipement, nomEquipement, adresseEquipement, nbCanalEquipement);
		}); // Connecter le signal clicked du bouton � une lambda fonction
		model->setItem(row, 3, new QStandardItem(""));
		tableView->setIndexWidget(model->index(row, 3), modifyButton);

		// Cr�er un bouton pour la cinqui�me colonne "Supprimer"
		QPushButton* deleteButton = new QPushButton("Supprimer");
		deleteButton->setObjectName(QString::number(currentIdEquipement)); // Utiliser l'ID de l'�quipement comme nom d'objet
		QObject::connect(deleteButton, &QPushButton::clicked, this, &modulesTestDMXProject::handleDeleteButtonClicked); // Connecter le signal clicked du bouton � une slot
		model->setItem(row, 4, new QStandardItem(""));
		tableView->setIndexWidget(model->index(row, 4), deleteButton);

		// Ajouter les autres donn�es de l'�quipement � la m�me ligne
		for (int i = 0; i < rowData.size(); ++i) {
			model->setItem(row, i, rowData[i]);
		}

		++row; // Incr�menter le num�ro de ligne
	}

	// Redimensionner les colonnes pour qu'elles s'adaptent au contenu
	tableView->resizeColumnsToContents();

	// D�finir les politiques de taille pour le tableau et le widget parent
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->verticalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// Ajouter le QTableView au layout verticalLayout_3 en prenant tout l'espace disponible
	ui.verticalLayout_3->addWidget(tableView);

	// R�initialiser l'ID de l'�quipement � supprimer
	m_idEquipementASupprimer = -1;
}


void modulesTestDMXProject::handleNewEquipmentButtonClicked()
{
	// Logique pour ajouter un nouvel �quipement
	QDialog dialog(this);
	dialog.setWindowTitle("Ajouter un nouvel equipement");

	QFormLayout form(&dialog);

	// Ajout des champs d'entr�e
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

	// Afficher la bo�te de dialogue
	if (dialog.exec() == QDialog::Accepted) {
		QString nom = nomEdit->text();
		QString adresse = adresseEdit->text();
		int canaux = canauxEdit->text().toInt();

		// Ajouter l'�quipement � la base de donn�es
		QSqlQuery query;
		query.prepare("INSERT INTO equipement (nom, adresse, nbCanal) VALUES (?, ?, ?)");
		query.addBindValue(nom);
		query.addBindValue(adresse);
		query.addBindValue(canaux);

		if (query.exec()) {
			// R�afficher la liste des �quipements
			Gerer_un_equipement();

			// Cr�er une nouvelle fen�tre pour les �tiquettes de canal et les champs de texte associ�s
			QDialog channelDialog(this);
			channelDialog.setWindowTitle("Noms des canaux");

			QVBoxLayout channelLayout(&channelDialog);

			// R�cup�rer la valeur de l'adresse pour l'incr�mentation
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

			// Ajouter les boutons OK et Annuler � la fen�tre des canaux
			QDialogButtonBox channelButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &channelDialog);
			channelLayout.addWidget(&channelButtonBox);

			// Connexion des boutons
			QObject::connect(&channelButtonBox, &QDialogButtonBox::accepted, [&]() {
				// Ins�rer les noms des canaux dans la base de donn�es
				QSqlQuery channelQuery;

				// Pr�parer la requ�te pour l'insertion des noms des canaux
				channelQuery.prepare("INSERT INTO champ (idEquip, idNumCanal, nom) VALUES (:idEquip, :idNumCanal, :nom)");

				// R�cup�rer l'ID de l'�quipement ins�r�
				int idEquip = query.lastInsertId().toInt();

				// Parcourir les noms des canaux et les ins�rer dans la base de donn�es
				for (int i = 0; i < canaux; ++i) {
					QString channelName = channelLineEditVector[i]->text();

					channelQuery.bindValue(":idEquip", idEquip);
					channelQuery.bindValue(":idNumCanal", adresseInt + i); // Identifiant num�rique du canal
					channelQuery.bindValue(":nom", channelName);

					if (!channelQuery.exec()) {
						qDebug() << "Erreur lors de l'insertion des donn�es dans la table champ:" << channelQuery.lastError().text();
						QMessageBox::warning(this, "Erreur", "�chec de l'ajout des noms des canaux � la base de donn�es: " + channelQuery.lastError().text());
						break;
					}
				}

				// Fermer la fen�tre des canaux une fois que l'insertion est termin�e
				channelDialog.accept();
			});

			QObject::connect(&channelButtonBox, &QDialogButtonBox::rejected, &channelDialog, &QDialog::reject);

			// Afficher la fen�tre des canaux
			channelDialog.exec();
		}
		else {
			QMessageBox::warning(this, "Erreur", "�chec de l'ajout de l'�quipement � la base de donn�es.");
		}
	}
}



void modulesTestDMXProject::handleDeleteButtonClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender()); // R�cup�rer le bouton qui a �mis le signal
	if (button) {
		int idEquipement = button->objectName().toInt(); // R�cup�rer l'ID de l'�quipement � partir du nom d'objet du bouton
		// Appeler la fonction de suppression en passant l'ID de l'�quipement � supprimer
		equipement->supprimerEquipement(idEquipement, this);
		Gerer_un_equipement();
	}
}

void modulesTestDMXProject::handleModifyButtonClicked(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement)
{
	// Cr�ation d'une bo�te de dialogue pour modifier les informations de l'�quipement
	QDialog dialog(this);
	dialog.setWindowTitle("Modifier �quipement");

	// Cr�ation des widgets pour saisir les nouvelles informations
	QLabel* labelNom = new QLabel("Nom :", &dialog);
	QLineEdit* lineEditNom = new QLineEdit(nomEquipement, &dialog);

	QLabel* labelAdresse = new QLabel("Adresse :", &dialog);
	QLineEdit* lineEditAdresse = new QLineEdit(adresseEquipement, &dialog);

	QLabel* labelNbCanal = new QLabel("Nombre de canaux :", &dialog);
	QSpinBox* spinBoxNbCanal = new QSpinBox(&dialog);
	spinBoxNbCanal->setValue(nbCanalEquipement);

	// Cr�ation du bouton de confirmation
	QPushButton* buttonConfirm = new QPushButton("Confirmer", &dialog);
	connect(buttonConfirm, &QPushButton::clicked, &dialog, &QDialog::accept);

	// Cr�ation du layout pour organiser les widgets
	QVBoxLayout* layout = new QVBoxLayout(&dialog);
	layout->addWidget(labelNom);
	layout->addWidget(lineEditNom);
	layout->addWidget(labelAdresse);
	layout->addWidget(lineEditAdresse);
	layout->addWidget(labelNbCanal);
	layout->addWidget(spinBoxNbCanal);
	layout->addWidget(buttonConfirm);

	// Affichage de la bo�te de dialogue
	if (dialog.exec() == QDialog::Accepted) {
		// R�cup�rer les nouvelles valeurs saisies par l'utilisateur
		QString nouveauNom = lineEditNom->text();
		QString nouvelleAdresse = lineEditAdresse->text();
		int nouveauNbCanal = spinBoxNbCanal->value();

		// Appeler la fonction de modification avec les nouvelles valeurs
		equipement->updateEquipement(idEquipement, nouveauNom, nouvelleAdresse, nouveauNbCanal);
		// Mettre � jour l'affichage des �quipements
		equipement->afficherEquipements(ui.verticalLayoutEquipements);

		Gerer_un_equipement();

	}
}


void modulesTestDMXProject::saveSettings() {
	// R�cup�rer l'ID de la sc�ne s�lectionn�e
	int idScene = scene->getSceneId(m_selectedScene); //getSceneId(m_selectedScene);

	QWizard* wizard = qobject_cast<QWizard*>(sender());
	if (wizard) {
		QList<QWizardPage*> pages = wizard->findChildren<QWizardPage*>();
		for (QList<QWizardPage*>::iterator it = pages.begin(); it != pages.end(); ++it) {
			QWizardPage* page = *it;

			// R�cup�rer les valeurs actuelles des QSpinBox dans la page actuelle
			QList<QPair<int, int>> channelData;
			QList<QSpinBox*> spinBoxes = page->findChildren<QSpinBox*>();
			for (QSpinBox* spinBox : spinBoxes) {
				// R�cup�rer le num�ro de canal � partir du nom d'objet du QSpinBox
				QString objectName = spinBox->objectName();
				int index = objectName.lastIndexOf('_');
				int numCanal = objectName.mid(index + 1).toInt();

				// R�cup�rer la valeur actuelle du QSpinBox
				int valeur = spinBox->value();

				// Ajouter la paire (numCanal, valeur) � la liste channelData
				channelData.append(qMakePair(numCanal, valeur));
			}

			// Enregistrer les param�tres de canal pour cet �quipement dans la base de donn�es
			canal->insertCanal(idScene, channelData);
		}
	}

	// Fermer le QWizard
	qobject_cast<QWizard*>(sender())->close();
}

void modulesTestDMXProject::sendDMXFrame()
{
	// R�cup�rer l'ID de la sc�ne s�lectionn�e dans votre interface utilisateur
	int sceneId = 5; //ui->sceneComboBox->currentData().toInt()

	// R�cup�rer les valeurs des canaux correspondants dans votre base de donn�es
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

	// Envoyer la trame DMX � votre serveur Linux en utilisant une connexion TCP
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
	// R�cup�rer le nom de la sc�ne s�lectionn�e dans le QComboBox
	QString sceneName = ui.sceneComboBox->currentText();

	// S�lectionner l'ID de la sc�ne correspondant au nom s�lectionn� dans la base de donn�es
	QSqlQuery query;
	query.prepare("SELECT id FROM scene WHERE nom = :sceneName");
	query.bindValue(":sceneName", sceneName);
	query.exec();

	int sceneId = -1;
	if (query.next()) {
		sceneId = query.value(0).toInt();
	}

	if (sceneId != -1) {
		// Convertir l'ID de la sc�ne en QByteArray
		QByteArray sceneIdData = QByteArray::number(sceneId);

		// Envoyer l'ID de la sc�ne au serveur Linux en utilisant une connexion TCP
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
		qDebug() << "Erreur : sc�ne non trouv�e.";
	}
}


void afficherEmplacementsLibresDansTrame() {
	// Connexion � la base de donn�es et r�cup�ration des adresses et du nombre de canaux de chaque �quipement
	QSqlQuery query("SELECT adresse, nbCanal FROM equipement ORDER BY adresse ASC;");

	// Structure de donn�es pour stocker les emplacements des �quipements dans la trame
	std::map<int, int> emplacements;

	// Assignation des emplacements pour chaque �quipement
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

	// Affichage des emplacements libres dans une bo�te de dialogue
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
	// Logique pour afficher les champs de l'�quipement
	qDebug() << "Showing fields for equipment at index:" << equipIndex;

	// R�initialiser l'index de champ actuel et les valeurs des sliders
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

	// Logique pour g�rer l'affichage du champ suivant
}