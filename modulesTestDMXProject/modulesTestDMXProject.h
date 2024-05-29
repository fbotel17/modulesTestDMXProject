#pragma once

#include <QtWidgets/QMainWindow>
#include <QTcpSocket>
#include <QDataStream>
#include "ui_modulesTestDMXProject.h"
#include "Scene.h"
#include "Equipement.h"
#include "Canal.h"
#include "Champ.h"
#include "ConsoleController.h"
#include "ConsoleMaterielle.h"

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
	void afficherEmplacementsLibresDansTrame();
	void sendSceneNamesToArduino(const QStringList& scenes);

	void clearForm();
	void Gerer_un_equipement();

	void fillSceneComboBox2();
	void fillEquipComboBox();
	void showEquipmentFields(int equipIndex);




public slots:
	void on_actionCreer_une_sc_ne_triggered();
	void on_actionConfigurer_une_sc_ne_2_triggered();
	void on_actionAjouter_un_equipement_triggered();
	void on_actionSupprimer_un_equipement_triggered();
	void on_actionTester_une_scene_triggered();
	void on_actionArduino_triggered();

	void on_pushButtonValider_clicked();
	void on_buttonEquip_clicked();
	void on_validateButtonEquip_clicked();
	void on_pushButton_clicked();
	//void on_ValidateButtonCanal_clicked();
	void handleDeleteButtonClicked();
	void handleModifyButtonClicked(int idEquipement, const QString& nomEquipement, const QString& adresseEquipement, int nbCanalEquipement);
	void saveSettings();
	void handleNewEquipmentButtonClicked();

	void validateSceneEquipment();
	void updateSliderValue(int value);
	void onConfirmButtonPressed();

private:
	Ui::modulesTestDMXProjectClass ui;
	QList<QPair<QString, QList<QPair<int, QString>>>> m_selectedEquipementsData;
	int m_currentEquipementIndex;
	QString m_selectedScene;
	QList<QLineEdit*> m_lineEdits;
	int numCanal;
	int m_idEquipementASupprimer = -1;
	QList<QString> m_selectedEquipements;
	Scene* scene;
	Equipement* equipement;
	Canal* canal;
	Champ* champ;

	QTcpSocket* tcpSocket;
	QDataStream in;
	QDataStream out;
	ConsoleController *consoleController;
	ConsoleMaterielle* consoleMaterielle;
	QMap<int, QSlider*> channelSliders;
	QList<int> sliderValues; // Variable pour stocker les valeurs des sliders
	int currentFieldIndex; // Index du champ actuel
};
