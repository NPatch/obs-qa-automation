/*
obs-qa-automation
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <plugin-support.h>

// #include "../.deps/obs-deps-2024-03-19-x64/include/jansson.h"
#include <util/platform.h>
#include <obs.h>
#include <obs.hpp>
#include <obs-frontend-api.h>

#include "obs-qa-automation-ui.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_AUTHOR("npatch")
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

#define CONFIG_FILE "obs_qa_automation.json"

OBSDataAutoRelease config_settings;

bool obs_module_load(void)
{
	QWidget *main_window = (QWidget *)obs_frontend_get_main_window();
	obs_frontend_push_ui_translation(obs_module_get_string);
	QWidget *testWidget = new OBSQAAutomation(main_window);
	obs_frontend_add_dock_by_id(
		"obs-qa-automation",
		testWidget->windowTitle().toStdString().c_str(), testWidget);
	obs_frontend_pop_ui_translation();

	obs_log(LOG_INFO, "plugin loaded successfully (version %s)",
		PLUGIN_VERSION);

	char* config_dir = obs_module_config_path(NULL);
	if (config_dir) {
		os_mkdirs(config_dir);
	}
	bfree(config_dir);

	char* settings_json = obs_module_config_path(CONFIG_FILE);
	if(!os_file_exists(settings_json)){

		OBSDataAutoRelease data = obs_data_create();
		obs_data_set_string(data, "scene_name", "");
		obs_data_set_string(data, "source_name", "");
		obs_data_set_string(data, "steam_gameid", "");
		obs_data_set_string(data, "exe_name", "");
		obs_data_set_string(data, "window_name", "");
		obs_data_set_string(data, "window_class", "");
		obs_data_set_string(data, "class_window_name", "");
		obs_data_set_string(data, "class_window_class", "");
		obs_data_save_json(data, settings_json);
	}
	
	if(os_file_exists(settings_json)){
		config_settings = obs_data_create_from_json_file(settings_json);
		const char* data_str = obs_data_get_json_pretty(config_settings);
		obs_log(LOG_INFO, "plugin settings: %s", data_str);

		//((OBSQAAutomation*)testWidget)->SetSettings(config_settings);
	}
	bfree(settings_json);

	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
	char* settings_json = obs_module_config_path(CONFIG_FILE);
	obs_data_save_json(config_settings, settings_json);
	bfree(settings_json);
}

const char *obs_module_name(void)
{
	return obs_module_text("QA Automation");
}

const char *obs_module_description(void)
{
	return obs_module_text("Description");
}