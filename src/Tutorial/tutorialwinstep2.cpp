#include "tutorialwinstep2.h"
#include "ui_tutorialwinstep2.h"
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include "Util/AppStrings.h"

namespace Drive
{

TutorialWinStep2::TutorialWinStep2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TutorialWinStep2),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(0, 0, ui->nextButton);

    connect(ui->googlePlayButton, &QPushButton::clicked, this, &TutorialWinStep2::pushedGooglePlay);
    connect(ui->appStoreButton, &QPushButton::clicked, this, &TutorialWinStep2::pushedAppleAppStore);
}

TutorialWinStep2::~TutorialWinStep2()
{
    delete ui;
}

void TutorialWinStep2::pushedGooglePlay()
{
    QDesktopServices::openUrl(
        QUrl(Strings::getAppString(Strings::GooglePlayURL), QUrl::TolerantMode));
}

void TutorialWinStep2::pushedAppleAppStore()
{
    QDesktopServices::openUrl(
        QUrl(Strings::getAppString(Strings::AppleAppStoreURL), QUrl::TolerantMode));
}

}
