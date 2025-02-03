#include "config.h"
#include "controllers/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"
#include "components/light_component.h"

#include "loaders/loader.h"
#include "loaders/loaderRaw.h"
#include "loaders/loaderJSON.h"

#include "savers/saver.h"
#include "savers/saverJSON.h"


int main() {

		App* app = new App();
		//Loader* loader = new LoaderJSON("dataSave/quilles_test.json");
		//Loader* loader = new LoaderJSON("dataSave/test.json");
		//Saver* saver = new SaverJSON("dataSave/test.json");

		//app->set_up_opengl();
		//app->make_systems();
		
		app->loadModelsAndTextures();
		//loader->loadEntities(app);
		app->loadEntities();

		//saver->saveEntities(app);

		app->run();

		//saver->saveEntities(app);
		//delete loader;
		delete app;
		return 0;
}