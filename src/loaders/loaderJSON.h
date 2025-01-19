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
		void loadPins(App* app, CameraComponent* camera) const override;
		void loadBall(App* app, CameraComponent* camera) const override;
		void loadLights(App* app, CameraComponent* camera) const override;
		void loadCamera(App* app, CameraComponent* camera) const override;
		void loadLane(App* app, CameraComponent* camera)const;
	public :
		LoaderJSON(std::string fileName);
};