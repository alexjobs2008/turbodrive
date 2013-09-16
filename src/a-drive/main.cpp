#include "single_app/singleapp.h"
#include "mainwindow.h"
//#include <QApplication>

int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);

	if(app.shouldContinue())
	{
		MainWindow w;
		w.show();
		return app.exec();
	}

	return 0;
}
