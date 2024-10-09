#ifndef OBS_QA_AUTOMATION_UI_H
#define OBS_QA_AUTOMATION_UI_H

#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
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

	QLineEdit* scene_name_te = nullptr;
	QLineEdit* source_name_te = nullptr;
	QLineEdit* steam_gameid_te = nullptr;
	QLineEdit* exe_name_te = nullptr;
	QLineEdit* window_name_te = nullptr;
	QLineEdit* window_class_te = nullptr;
	QLineEdit* crash_window_name_te = nullptr;
	QLineEdit* crash_window_class_te = nullptr;

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
	void createGridStringProperty(QLabel*& lb, const char* lb_text, QLineEdit*& le, QGridLayout*& layout, int row);

private slots:
	void ButtonClicked();
	void Update();

public slots:
	void Reset();

};

#endif //OBS_QA_AUTOMATION_UI_H