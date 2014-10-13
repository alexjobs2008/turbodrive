#ifndef TUTORIALSTEPINTERFACE_H
#define TUTORIALSTEPINTERFACE_H

#include <QObject>
#include <QDialog>
#include <QPushButton>

namespace Drive
{

class TutorialStepInterface : public QObject
{
    Q_OBJECT
public:
    explicit TutorialStepInterface(QWidget *parent);
    void init(QPushButton* cancelButton, QPushButton* backButton,
            QPushButton* nextButton);
    inline QWidget* widget() { return (QWidget*) this->parent(); }

signals:
    void cancel();
    void back();
    void next();
};

}

#endif // TUTORIALSTEPINTERFACE_H
