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
		Loader* loader = new LoaderJSON("dataSave/test.json", app->getEntityManager());
		Saver* saver = new SaverJSON("dataSave/test.json", app->getEntityManager());

		//app->set_up_opengl();
		//app->make_systems();
		
		app->loadModelsAndTextures();
		app->loadEntities();
		loader->loadEntities();


		app->run();

		saver->saveEntities();
		delete saver;
		delete loader;
		delete app;
		return 0;
}