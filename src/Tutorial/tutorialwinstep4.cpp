#include "tutorialwinstep4.h"
#include "ui_tutorialwinstep4.h"
#include <QtGui/QDesktopServices>
#include "Util/AppStrings.h"
#include "SettingsUI/SettingsWidget.h"

namespace Drive
{

TutorialWinStep4::TutorialWinStep4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TutorialWinStep4),
    step(new TutorialStepInterface(this))
{
    ui->setupUi(this);
    step->init(0, 0, ui->nextButton);

    connect(ui->settingsButton, &QPushButton::clicked, this, &TutorialWinStep4::pushedSettings);
}

TutorialWinStep4::~TutorialWinStep4()
{
    delete ui;
}

void TutorialWinStep4::pushedSettings()
{
    SettingsWidget::instance().show();
}

}
