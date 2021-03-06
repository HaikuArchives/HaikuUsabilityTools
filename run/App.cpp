#include "App.h"
#include "MainWindow.h"

#define APP_SIGNATURE "application/x-vnd.usability-Run"

App::App(void)
	:	BApplication(APP_SIGNATURE)
{
	MainWindow *mainwin = new MainWindow();
	mainwin->Show();
}


int
main(void)
{
	App *app = new App();
	app->Run();
	delete app;
	return 0;
}
