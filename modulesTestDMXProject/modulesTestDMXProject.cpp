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
#include <QWizard>


modulesTestDMXProject::modulesTestDMXProject(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.213");
	db.setDatabaseName("testCodeDMX");
	db.setUserName("root");
	db.setPassword("root");

	if (!db.open()) {
		qDebug() << "�chec de la connexion � la base de donn�es.";
		// G�rer les erreurs de connexion
		return;
	}

	QTcpSocket* socket = new QTcpSocket(this);
	socket->connectToHost("192.168.64.170", 12345); // Remplacez 1234 par le num�ro de port de votre serveur



	in.setDevice(tcpSocket);
	out.setDevice(tcpSocket);
	out.setVersion(QDataStream::Qt_5_0);


	// Afficher toutes les sc�nes existantes
	scene->afficherScenes(ui.listWidget);
	equipement->afficherEquipements(ui.verticalLayoutEquipements);

	afficherScenesCheckbox();
	Gerer_un_equipement();
	fillSceneComboBox();

	connect(ui.testSceneButton, &QPushButton::clicked, this, &modulesTestDMXProject::testScene);

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
	QScrollArea* scrollArea = new QScrollArea;
	QWidget* scrollWidget = new QWidget;
	QVBoxLayout* layoutScenes = new QVBoxLayout(scrollWidget);
	scrollWidget->setLayout(layoutScenes);
	scrollArea->setWidget(scrollWidget);
	scrollArea->setWidgetResizable(true);

	// R�cup�rer le layout pour les sc�nes
	QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui.verticalLayoutScenes->layout());
	if (!mainLayout) {
		qDebug() << "Erreur : le layout des sc�nes est invalide.";
		return;
	}

	// Effacer le contenu existant du layout principal
	QLayoutItem* child;
	while ((child = mainLayout->takeAt(0)) != nullptr) {
		delete child->widget();
		delete child;
	}

	// Garder une liste de toutes les cases � cocher
	QList<QCheckBox*> checkBoxes;

	// Parcourir les r�sultats de la requ�te et ajouter chaque sc�ne en tant que case � cocher
	while (query.next()) {
		QString nomScene = query.value(0).toString();

		// Cr�er une case � cocher pour la sc�ne
		QCheckBox* checkBox = new QCheckBox(nomScene);
		// Ajouter la case � cocher au layout des sc�nes
		layoutScenes->addWidget(checkBox);
		checkBoxes.append(checkBox); // Ajouter la case � cocher � la liste
	}

	// D�finir une largeur fixe pour le widget de d�filement
	scrollArea->setFixedWidth(200); // Vous pouvez ajuster cette valeur selon vos besoins

	// Ajouter le widget de d�filement au layout principal
	mainLayout->addWidget(scrollArea);

	// Connecter le signal toggled() de chaque case � cocher � une fonction de gestion
	for (QCheckBox* checkBox : checkBoxes) {
		connect(checkBox, &QCheckBox::toggled, [this, checkBoxes, checkBox](bool checked) {
			if (checked) {
				// D�s�lectionner toutes les autres cases � cocher sauf celle qui vient d'�tre coch�e
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
	QLayoutItem* child;
	while ((child = ui.verticalLayout_17->takeAt(0)) != nullptr) {
		QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(child->widget());
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
	QWidget* widgetEquipements = ui.verticalLayoutEquipements->itemAt(0)->widget();
	QWidget* widgetScenes = ui.verticalLayoutScenes->itemAt(0)->widget();

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

	// Cr�er un QWizard pour configurer les canaux de chaque �quipement dans la sc�ne
	QWizard* wizard = new QWizard(this);

	// Cr�er une page pour chaque �quipement s�lectionn�
	for (const QString& equipementName : selectedEquipements)
	{
		QWizardPage* page = new QWizardPage(this);
		QVBoxLayout* layout = new QVBoxLayout(page);
		layout->addWidget(new QLabel(QString("Configurer les canaux pour %1 :").arg(equipementName), page));

		// R�cup�rer les informations sur les canaux pour cet �quipement
		QSqlQuery query(QString("SELECT idNumCanal, nom FROM champ WHERE idEquip = (SELECT id FROM equipement WHERE nom = '%1') ORDER BY idNumCanal").arg(equipementName));

		// Ajouter des widgets pour chaque canal de cet �quipement
		QList<QPair<int, int>> channelData; // Ajouter une liste de paires pour stocker les valeurs des QSpinBox
		while (query.next())
		{
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
	connect(wizard, SIGNAL(accepted()), this, SLOT(saveSettings()));

	// Afficher le QWizard
	wizard->show();
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


// Mettre � jour la m�thode Supprimer_un_equipement pour utiliser la variable membre m_idEquipementASupprimer
void modulesTestDMXProject::Gerer_un_equipement()
{
	// V�rifier si un �quipement doit �tre supprim�

		// Effacer tous les widgets pr�c�dents du layout
	QLayoutItem* child;

	while ((child = ui.verticalLayout_3->takeAt(0)) != nullptr)
	{
		delete child->widget();
		delete child;
	}

	// Vider la liste des pointeurs de QLineEdit
	m_lineEdits.clear();

	// Cr�er un nouveau QTableView
	QTableView* tableView = new QTableView;

	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

	// Cr�er un mod�le de tableau vide avec cinq colonnes
	QStandardItemModel* model = new QStandardItemModel(0, 5);
	tableView->setModel(model);

	// D�finir les en-t�tes de colonne
	model->setHorizontalHeaderLabels(QStringList() << "Nom de l'�quipement" << "Adresse" << "Nombre de Canal" << "Modifier" << "Supprimer");

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

	// S�lectionner les valeurs des canaux correspondant � la sc�ne s�lectionn�e dans la base de donn�es
	QSqlQuery query;
	query.prepare("SELECT numCanal, valeur FROM canaux WHERE idScene = (SELECT id FROM scene WHERE nom = :sceneName)");
	query.bindValue(":sceneName", sceneName);
	query.exec();

	// Construire la trame DMX
	// Construire la trame DMX
	QByteArray dmxFrame(512, 0);
	while (query.next()) {
		int numCanal = query.value(0).toInt();
		int valeur = query.value(1).toInt();
		dmxFrame[numCanal - 1] = valeur;
	}

	// Afficher les donn�es de la trame DMX dans la console de d�bogage
	qDebug() << "Trame DMX :" << dmxFrame.toHex(' ');

	// Envoyer la trame DMX au serveur Linux en utilisant une connexion TCP
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
