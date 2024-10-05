#include <obs.h>
#include <plugin-support.h>
#include "obs-qa-automation-ui.hpp"

#define TIMER_INTERVAL 2000

OBSQAAutomation::OBSQAAutomation(QWidget *parent, bool closable) 
		: QFrame(parent),
		timer(this)
{
	this->parent = parent;

	QVBoxLayout *mainLayout = new QVBoxLayout();

	{//Labels
		{//Scene Name
			QHBoxLayout* hor_layout = new QHBoxLayout();
			
			this->scene_name_lb = new QLabel("Scene Name");
			hor_layout->addWidget(this->scene_name_lb);

			this->scene_name_te = new QPlainTextEdit("");
			hor_layout->addWidget(this->scene_name_te);

			mainLayout->addLayout(hor_layout);
		}

		{//Source Name
			QHBoxLayout* hor_layout = new QHBoxLayout();

			this->source_name_lb = new QLabel("Source Name");
			hor_layout->addWidget(this->source_name_lb);
			
			this->source_name_te = new QPlainTextEdit("");
			hor_layout->addWidget(this->source_name_te);
			
			mainLayout->addLayout(hor_layout);
		}

		{//Steam GameID
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->steam_gameid_lb = new QLabel("Steam GameID");
			hor_layout->addWidget(this->steam_gameid_lb);
			mainLayout->addLayout(hor_layout);
		}

		{//Exe Name
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->exe_name_lb = new QLabel("Executable Name");
			hor_layout->addWidget(this->exe_name_lb);
			mainLayout->addLayout(hor_layout);
		}

		{//Window Name
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->window_name_lb = new QLabel("Window Name");
			hor_layout->addWidget(this->window_name_lb);
			mainLayout->addLayout(hor_layout);
		}

		{//Window Class
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->window_class_lb = new QLabel("Window Class");
			hor_layout->addWidget(this->window_class_lb);
			mainLayout->addLayout(hor_layout);
		}

		{//Crash Window Name
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->crash_window_name_lb = new QLabel("Crash Window Name");
			hor_layout->addWidget(this->crash_window_name_lb);
			mainLayout->addLayout(hor_layout);
		}

		{//Crash Window Class
			QHBoxLayout* hor_layout = new QHBoxLayout();
			this->crash_window_class_lb = new QLabel("Crash Window Class");
			hor_layout->addWidget(this->crash_window_class_lb);
			mainLayout->addLayout(hor_layout);
		}
	}

	//StartQA button
	this->start_qa_btn = new QPushButton();
	this->start_qa_btn->setText("Start QA");
	mainLayout->addWidget(this->start_qa_btn);

	setLayout(mainLayout);
	resize(300, 300);
    setWindowTitle("QA Automation");

	QObject::connect(this->start_qa_btn, SIGNAL(clicked()), SLOT(ButtonClicked()));

	QObject::connect(&timer, &QTimer::timeout, this,
			 &OBSQAAutomation::Update);
	timer.setInterval(TIMER_INTERVAL);

	if (isVisible())
		timer.start();

	Update();
}

OBSQAAutomation::~OBSQAAutomation() {

}

void OBSQAAutomation::ButtonClicked() 
{
	QMessageBox::information(this, "Info", "You just clicked on the box version 2!");
}

void OBSQAAutomation::Reset()
{
	if(config_data != nullptr){
		obs_data_release(config_data);
		config_data = nullptr;
	}

	timer.start();
}

void OBSQAAutomation::Update()
{
	if(config_data = nullptr)
		return;

	// obs_data_set_string(config_data, "scene_name", scene_name_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "source_name", source_name_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "steam_gameid", steam_gameid_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "exe_name", exe_name_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "window_name", window_name_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "window_class", window_class_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "class_window_name", crash_window_name_te->toPlainText().toStdString().c_str());
	// obs_data_set_string(config_data, "class_window_class", crash_window_class_te->toPlainText().toStdString().c_str());
}

void OBSQAAutomation::SetSettings(obs_data_t* settings){

	if(config_data != nullptr){
		obs_data_release(config_data);
		config_data = nullptr;
	}

	obs_data_addref(settings);
	config_data = settings;

	if(config_data == nullptr){
		obs_log(LOG_INFO, "ConfigData is null");
		return;
	}

	const char* scene_name = obs_data_get_string(config_data, "scene_name");
	const char* source_name = obs_data_get_string(config_data, "source_name");
	const char* steam_gameid = obs_data_get_string(config_data, "steam_gameid");
	const char* exe_name = obs_data_get_string(config_data, "exe_name");
	const char* window_name = obs_data_get_string(config_data, "window_name");
	const char* window_class = obs_data_get_string(config_data, "window_class");
	const char* crash_window_name = obs_data_get_string(config_data, "class_window_name");
	const char* crash_window_class = obs_data_get_string(config_data, "class_window_class");

	obs_log(LOG_INFO, scene_name);
	obs_log(LOG_INFO, source_name);
	obs_log(LOG_INFO, steam_gameid);
	obs_log(LOG_INFO, exe_name);
	obs_log(LOG_INFO, window_name);
	obs_log(LOG_INFO, window_class);
	obs_log(LOG_INFO, crash_window_name);
	obs_log(LOG_INFO, crash_window_class);

	scene_name_te->setPlainText(scene_name);
	source_name_te->setPlainText(source_name);
	steam_gameid_te->setPlainText(steam_gameid);
	exe_name_te->setPlainText(exe_name);
	window_name_te->setPlainText(window_name);
	window_class_te->setPlainText(window_class);
	window_name_te->setPlainText(window_name);
	window_class_te->setPlainText(window_class);
	crash_window_name_te->setPlainText(crash_window_name);
	crash_window_class_te->setPlainText(crash_window_class);
}

obs_data_t* OBSQAAutomation::GetSettings(){
	return nullptr;
}