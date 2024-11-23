#pragma once
#include "loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

class LoaderJSON : public Loader {
	public :
		void loadQuilles(App* app, CameraComponent *camera) const override;
		void loadEntities(App* app) const override;
};