#include <obs.h>
#include <plugin-support.h>
#include "obs-qa-automation-ui.hpp"

#define TIMER_INTERVAL 2000

void OBSQAAutomation::DebugMessage(QWidget* parent, const char* format)
{
	QMessageBox::information(parent, "Info", format);
}

bool CompareLineEditWithConfig(QLineEdit*& widget, const char* value){
	return strcmp(widget->text().toStdString().c_str(), value) == 0;
}

void SetTextWithoutNotifying(QLineEdit*& widget, const char* value)
{
	const QSignalBlocker blocker(widget);
	widget->setText(value);
	widget->setCursorPosition(0);
}

void OBSQAAutomation::createGridStringProperty(QLabel*& lb, const char* lb_text, QLineEdit*& le, QGridLayout*& layout, int row)
{
	lb = new QLabel(lb_text);
	le = new QLineEdit("");

	layout->addWidget(lb, row, 0);
	layout->addWidget(le, row, 1);

	QObject::connect(le, &QLineEdit::textChanged, this, &OBSQAAutomation::Update);
}

OBSQAAutomation::OBSQAAutomation(QWidget *parent, bool closable) 
		: QFrame(parent),
		timer(this)
{
	this->parent = parent;

	QGridLayout *mainLayout = new QGridLayout();

	{//Properties
		//Scene Name
		createGridStringProperty(scene_name_lb, "Scene Name", scene_name_te, mainLayout, 0);
		//Source Name
		createGridStringProperty(source_name_lb, "Source Name", source_name_te, mainLayout, 1);
		//Steam GameID
		createGridStringProperty(steam_gameid_lb, "Steam GameID", steam_gameid_te, mainLayout, 2);
		//Exe Name
		createGridStringProperty(exe_name_lb, "Executable Name", exe_name_te, mainLayout, 3);
		//Window Name
		createGridStringProperty(window_name_lb, "Window Name", window_name_te, mainLayout, 4);
		//Window Class
		createGridStringProperty(window_class_lb, "Window Class", window_class_te, mainLayout, 5);
		//Crash Window Name
		createGridStringProperty(crash_window_name_lb, "Crash Window Name", crash_window_name_te, mainLayout, 6);
		//Crash Window Class
		createGridStringProperty(crash_window_class_lb, "Crash Window Class", crash_window_class_te, mainLayout, 7);
	}

	//StartQA button
	start_qa_btn = new QPushButton();
	start_qa_btn->setText("Start QA");
	mainLayout->addWidget(start_qa_btn, 8, 0, 1, 2);

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
	DebugMessage(this, "You just clicked on the box version 2!");
	// QMessageBox::information(this, "Info", "You just clicked on the box version 2!");
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
	if(config_data == nullptr)
		return;

	obs_log(LOG_INFO, "Update");

	const char* scene_name = obs_data_get_string(config_data, "scene_name");
	const char* source_name = obs_data_get_string(config_data, "source_name");
	const char* steam_gameid = obs_data_get_string(config_data, "steam_gameid");
	const char* exe_name = obs_data_get_string(config_data, "exe_name");
	const char* window_name = obs_data_get_string(config_data, "window_name");
	const char* window_class = obs_data_get_string(config_data, "window_class");
	const char* crash_window_name = obs_data_get_string(config_data, "crash_window_name");
	const char* crash_window_class = obs_data_get_string(config_data, "crash_window_class");

	if(!CompareLineEditWithConfig(scene_name_te, scene_name))
	{
		obs_data_set_string(config_data, "scene_name", scene_name_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(source_name_te, source_name))
	{
		obs_data_set_string(config_data, "source_name", source_name_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(steam_gameid_te, steam_gameid))
	{
		obs_data_set_string(config_data, "steam_gameid", steam_gameid_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(exe_name_te, exe_name))
	{
		obs_data_set_string(config_data, "exe_name", exe_name_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(window_name_te, window_name))
	{
		obs_data_set_string(config_data, "window_name", window_name_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(window_class_te, window_class))
	{
		obs_data_set_string(config_data, "window_class", window_class_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(crash_window_name_te, crash_window_name))
	{
		obs_data_set_string(config_data, "crash_window_name", crash_window_name_te->text().toStdString().c_str());
	}
	if(!CompareLineEditWithConfig(crash_window_class_te, crash_window_class))
	{
		obs_data_set_string(config_data, "crash_window_class", crash_window_class_te->text().toStdString().c_str());
	}

}

void OBSQAAutomation::SetSettings(obs_data_t* settings){

	if(config_data != nullptr){
		obs_data_release(config_data);
		config_data = nullptr;
	}

	config_data = settings;
	obs_data_addref(config_data);

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
	const char* crash_window_name = obs_data_get_string(config_data, "crash_window_name");
	const char* crash_window_class = obs_data_get_string(config_data, "crash_window_class");

	SetTextWithoutNotifying(scene_name_te, scene_name);
	SetTextWithoutNotifying(source_name_te, source_name);
	SetTextWithoutNotifying(steam_gameid_te, steam_gameid);
	SetTextWithoutNotifying(exe_name_te, exe_name);
	SetTextWithoutNotifying(window_name_te, window_name);
	SetTextWithoutNotifying(window_class_te, window_class);
	SetTextWithoutNotifying(window_name_te, window_name);
	SetTextWithoutNotifying(window_class_te, window_class);
	SetTextWithoutNotifying(crash_window_name_te, crash_window_name);
	SetTextWithoutNotifying(crash_window_class_te, crash_window_class);
}

obs_data_t* OBSQAAutomation::GetSettings(){
	return nullptr;
}