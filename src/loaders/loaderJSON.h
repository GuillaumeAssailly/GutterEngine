#pragma once
#include "loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class LoaderJSON : public Loader {
	private :
		json jsonData;
		void loadQuilles(App* app, CameraComponent* camera) const override;
		void loadBall(App* app, CameraComponent* camera) const override;
	public :
		LoaderJSON();
};