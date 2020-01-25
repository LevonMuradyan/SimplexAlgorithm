#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>


class QLineEdit;
class QLabel;
class Answer;

namespace Ui
{
    class Widget;
}


class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void variablesPlus();
    void variablesMinus();
    void functionsPlus();
    void functionsMinus();
    void calculate();

protected:
    void changeEvent(QEvent *e);

private:
    void displayAnswer(Answer &answer);

    QList<QLabel*>optimizationLabels;
    QList<QLineEdit*>optimizationLineEdits;
    QList<QLabel*>restrictionLabels;
    QList<QList<QLineEdit*>* >restrictionLineEdits;
    QList<QWidget*> lastColumn;
    int numVariables;
    int numFunctions;

    QList<QLabel*>answerLabels;

    void moveLastColumn();

    Ui::Widget *ui;
};

#endif // WIDGET_H
