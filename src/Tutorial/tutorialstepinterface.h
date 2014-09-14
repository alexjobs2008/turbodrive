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
    explicit TutorialStepInterface(QDialog *parent);
    void init(
            QPushButton* cancelButton,
            QPushButton* backButton,
            QPushButton* nextButton);
    inline QDialog* dialog() { return (QDialog*) this->parent(); }

signals:
    void cancel();
    void back();
    void next();
};

}

#endif // TUTORIALSTEPINTERFACE_H
