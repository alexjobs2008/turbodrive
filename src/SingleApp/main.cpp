#include "singleapp.h"

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv);

    if(app.shouldContinue())
        return app.exec();

    return 0;
}
