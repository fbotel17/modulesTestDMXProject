#pragma once

#include <QtWidgets/QMainWindow>
#include <QTcpSocket>
#include <QDataStream>
#include "ui_modulesTestDMXProject.h"
#include "Scene.h"
#include "Equipement.h"
#include "Canal.h"
#include "Champ.h"

class modulesTestDMXProject : public QMainWindow
{
	Q_OBJECT

public:
	modulesTestDMXProject(QWidget* parent = nullptr);
	~modulesTestDMXProject();
	
	void afficherScenesCheckbox();
	void createChannelLabelsAndLineEdits(int channelCount, int numCanal);
	void createFormForSelectedEquipements(const QList<QString>& selectedEquipements, const QString& selectedScene);

	void sendData(const QByteArray& data);
	void sendDMXFrame();
	void fillSceneComboBox();
	void testScene();


	void clearForm();
	void Gerer_un_equipement();

public slots:
	void on_actionCreer_une_sc_ne_triggered();
	void on_actionConfigurer_une_sc_ne_2_triggered();
	void on_actionAjouter_un_equipement_triggered();
	void on_actionSupprimer_un_equipement_triggered();
	void on_actionTester_une_scene_triggered();

	void on_pushButtonValider_clicked();
	void on_buttonEquip_clicked();
	void on_validateButtonEquip_clicked();
	void on_pushButton_clicked();
	//void on_ValidateButtonCanal_clicked();
	void handleDeleteButtonClicked();
	void handleModifyButtonClicked(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement);

	//void updateUi(int);
	void saveSettings();

private:
	Ui::modulesTestDMXProjectClass ui;
	QList<QPair<QString, QList<QPair<int, QString>>>> m_selectedEquipementsData;
	int m_currentEquipementIndex;
	QString m_selectedScene;
	QList<QLineEdit*> m_lineEdits;
	int numCanal;
	int m_idEquipementASupprimer = -1; // Initialisé à -1 pour indiquer qu'aucun équipement n'est sélectionné initialement
	QList<QString> m_selectedEquipements;
	Scene* scene;
	Equipement* equipement;
	Canal* canal;
	Champ* champ;

	QTcpSocket* tcpSocket;
	QDataStream in;
	QDataStream out;
};
