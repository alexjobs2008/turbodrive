#ifndef EXTRA_LABELS_H
#define EXTRA_LABELS_H

#include <QtWidgets/QLabel>

namespace AuxWidgets
{

class LinkLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool hovered READ getHovered())

public:
    LinkLabel(QString text, QString link, QWidget *parent);

    bool getHovered() { return m_hovered; };

protected:
    bool event(QEvent *e);
    void mousePressEvent(QMouseEvent *event);

private:
    bool m_hovered;
    QString m_link;
};


class ButtonLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString state READ getStateString())

    enum State
    {
        sNormal,
        sHovered,
        sPressed,
        sDisabled
    };

public:
    ButtonLabel(QWidget *parent = 0);
    QString getStateString();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void changeEvent(QEvent *event);

    State m_state;

signals:
    void doubleClicked();
    void clicked();
};

class ElidedLabel : public QLabel
{
    Q_OBJECT

public:
    ElidedLabel(QWidget *parent = 0);

    void setText(const QString &text);
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *event);

private:
    void updateFullSize();

    QString m_originalText;
    QSize m_fullSize;
};

}
#endif // EXTRA_LABELS_H
