#ifndef ANIMATED_SYSTEM_TRAY_ICON_H
#define ANIMATED_SYSTEM_TRAY_ICON_H

#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QMap>


class AnimatedSystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:

     struct State
     {
         State() {};
         
         State(const QString& name, const QPixmap& pixmap)
             : name(name)
             , delay(40)
             , currentFrame(0)
		 {			 
             frames << pixmap;
		 };
		 
		 inline bool isAnimated()
		 {
			 return frames.size() > 1;
		 };
		 
		 QString name;
         QVector<QPixmap> frames;
		 int delay; // delay between showing the next frame, in milliseconds
         int currentFrame;
     };

    static AnimatedSystemTrayIcon& instance();
	~AnimatedSystemTrayIcon();

	void appendState(State *state); // takes ownership of state
	bool setState(const QString &stateName);

protected:
	explicit AnimatedSystemTrayIcon(QObject *parent = 0);
    virtual void timerEvent(QTimerEvent * event);

private:
    Q_DISABLE_COPY(AnimatedSystemTrayIcon)    

    QMap<QString, State*> states;
	State *currentState;
	int timerId;

};

#endif ANIMATED_SYSTEM_TRAY_ICON_H