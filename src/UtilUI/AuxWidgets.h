#ifndef AUXWIDGETS_H
#define AUXWIDGETS_H

#include <QtWidgets/QFrame>

class QLabel;
class QLineEdit;
class QHBoxLayout;

namespace CommonUI
{

class LabeledEdit : public QFrame
{
	Q_OBJECT
//	Q_PROPERTY(bool isEnabled READ getStateString())
public:

	enum Type
	{
		Numeric,
		Text
	};

	LabeledEdit(QString leftCaption,
		Type type = Type::Text,
		QString rightCaption = QString(),
		uint leftLabelMinWidth = 150,
		QString regexp = QString(),
		uint editMaxWidth = 0,
		QWidget *parent = 0);

	bool getStateString() const;

	void setName(const QString& name);

	QString text() const;
	void setText(QString text);

	int number();

	QHBoxLayout* layout() const;
	QLabel* leftLabel() const;
	QLineEdit* lineEdit() const;
	QLabel* rightLabel() const;

private:
	Type type;
	QHBoxLayout *_layout;
	QLabel *_leftLabel;
	QLabel *_rightLabel;
	QLineEdit *_lineEdit;

};

class LabeledLabel : public QFrame
{
	Q_OBJECT
public:
	LabeledLabel(QString leftCaption, QString rightCaption,
		uint leftLabelMinWidth = 150, QWidget *parent = 0);
};

class MinMaxEdit : public QFrame
{
	Q_OBJECT
public:
	MinMaxEdit(QString defaultMin, QString defaultMax,
		QString leftCaption, QString middleCaption, QString rightCaption,
		uint leftLabelMinWidth = 150, uint middleLabelMinWidth = 48,
		uint editMaxWidth = 40,
		QString regexpMin = QString(), QString regexpMax = QString(),
		QWidget *parent = 0);

	int minInt() const;
	int maxInt() const;
	QString minString() const;
	QString maxString() const;

private:
	QString prepare(QString value) const;

	QLineEdit *leMin;
	QLineEdit *leMax;
};

}

#endif // AUXWIDGETS_H
