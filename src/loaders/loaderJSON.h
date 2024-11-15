#pragma once
#include "loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

class LoaderJSON : public Loader {
	void loadQuilles(App* app) const override;
};