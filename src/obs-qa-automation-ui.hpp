#ifndef OBS_QA_AUTOMATION_UI_H
#define OBS_QA_AUTOMATION_UI_H

#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QMessageBox>

#include <obs-data.h>

class OBSQAAutomation : public QFrame 
{
	Q_OBJECT

	QWidget *parent = nullptr;

	QLabel* scene_name_lb = nullptr;
	QLabel* source_name_lb = nullptr;
	QLabel* steam_gameid_lb = nullptr;
	QLabel* exe_name_lb = nullptr;
	QLabel* window_name_lb = nullptr;
	QLabel* window_class_lb = nullptr;
	QLabel* crash_window_name_lb = nullptr;
	QLabel* crash_window_class_lb = nullptr;

	QPlainTextEdit* scene_name_te = nullptr;
	QPlainTextEdit* source_name_te = nullptr;
	QPlainTextEdit* steam_gameid_te = nullptr;
	QPlainTextEdit* exe_name_te = nullptr;
	QPlainTextEdit* window_name_te = nullptr;
	QPlainTextEdit* window_class_te = nullptr;
	QPlainTextEdit* crash_window_name_te = nullptr;
	QPlainTextEdit* crash_window_class_te = nullptr;

	QPushButton* start_qa_btn = nullptr;

	obs_data_t* config_data = nullptr;

	QTimer timer;

public:
	OBSQAAutomation(QWidget *parent = nullptr, bool closable = true);
	~OBSQAAutomation();

	static void InitializeValues();
	void SetSettings(obs_data_t* settings);
	obs_data_t* GetSettings();

private:
	void buttonClicked();
	void Update();

private slots:
	void ButtonClicked();

public slots:
	void Reset();

};

#endif //OBS_QA_AUTOMATION_UI_H