#include "widget.h"
#include "ui_widget.h"
#include <QLineEdit>
#include "calc.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("Simplex method");
    ui->variables_plus->setIcon(QIcon(":/images/Add.png"));
    ui->functions_plus->setIcon(QIcon(":/images/Add.png"));
    ui->variable_minus->setIcon(QIcon(":/images/Delete.png"));
    ui->functions_minus->setIcon(QIcon(":/images/Delete.png"));
    ui->calc_button->setIcon(QIcon(":/images/calc.png"));

    numFunctions=0;
    numVariables=0;


    variablesPlus();
    variablesPlus();
    functionsPlus();
    functionsPlus();


    connect(ui->variables_plus,SIGNAL(clicked()),this,SLOT(variablesPlus()));
    connect(ui->variable_minus,SIGNAL(clicked()),this,SLOT(variablesMinus()));
    connect(ui->functions_plus,SIGNAL(clicked()),this,SLOT(functionsPlus()));
    connect(ui->functions_minus,SIGNAL(clicked()),this,SLOT(functionsMinus()));
    connect(ui->calc_button,SIGNAL(clicked()),this,SLOT(calculate()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::variablesPlus()
{

    if(numVariables==0)
    {
        QLabel *firstLabel=new QLabel("<=");
        ui->constraint_grid->addWidget(firstLabel,0,0);
        lastColumn.append(firstLabel);
    }

    QString labelNum;
    labelNum.setNum(numVariables+1);
    QLabel *labelOpt=new QLabel("x"+labelNum);
    labelOpt->setAlignment(Qt::AlignHCenter);
    ui->objective_grid->addWidget(labelOpt,0,numVariables);
    optimizationLabels.append(labelOpt);
    QLabel *labelRestr=new QLabel("x"+labelNum);
    labelRestr->setAlignment(Qt::AlignHCenter);
    ui->constraint_grid->addWidget(labelRestr,0,numVariables);
    restrictionLabels.append(labelRestr);


    QLineEdit *optLineEdit=new QLineEdit();
    optLineEdit->setMaximumWidth(40);
    ui->objective_grid->addWidget(optLineEdit,1,numVariables);
    optimizationLineEdits.append(optLineEdit);


    for(int i=0,n=restrictionLineEdits.size();i<n;i++)
    {
        QLineEdit *restrLineEdit=new QLineEdit();
        restrLineEdit->setMaximumWidth(40);
        ui->constraint_grid->addWidget(restrLineEdit,i+1,numVariables);
        restrictionLineEdits[i]->append(restrLineEdit);
    }

    numVariables++;


    moveLastColumn();

    this->adjustSize();
}

void Widget::moveLastColumn()
{
    int col=numVariables;
    for(int i=0,n=lastColumn.size();i<n;i++)
    {
        ui->constraint_grid->removeWidget(lastColumn.at(i));
        ui->constraint_grid->addWidget(lastColumn.at(i),i,col);
    }
}

void Widget::variablesMinus()
{
    if(numVariables<=1)
        return;


    ui->objective_grid->removeWidget(optimizationLabels.last());
    delete optimizationLabels.takeLast();
    ui->constraint_grid->removeWidget(restrictionLabels.last());
    delete restrictionLabels.takeLast();


    ui->objective_grid->removeWidget(optimizationLineEdits.last());
    delete optimizationLineEdits.takeLast();

    for(int i=0,n=restrictionLineEdits.size();i<n;i++)
    {
        ui->constraint_grid->removeWidget(restrictionLineEdits.at(i)->last());
        delete restrictionLineEdits[i]->takeLast();
    }

    numVariables--;

    this->adjustSize();
}

void Widget::functionsPlus()
{

    QList<QLineEdit*> *list= new QList<QLineEdit*>;
    for(int i=0;i<numVariables;i++)
    {
        QLineEdit *lineEdit=new QLineEdit();
        lineEdit->setMaximumWidth(40);
        ui->constraint_grid->addWidget(lineEdit,numFunctions+1,i);
        list->append(lineEdit);
    }
    restrictionLineEdits.append(list);


    QLineEdit *lastLineEdit=new QLineEdit();
    lastLineEdit->setMaximumWidth(40);
    ui->constraint_grid->addWidget(lastLineEdit,numFunctions+1,numVariables);
    lastColumn.append(lastLineEdit);

    numFunctions++;


    moveLastColumn();

    this->adjustSize();
}

void Widget::functionsMinus()
{
    if(numFunctions<=1)
        return;

    QList<QLineEdit*> *list=restrictionLineEdits.takeLast();
    while(!list->isEmpty())
    {
        ui->constraint_grid->removeWidget(list->last());
        delete list->takeLast();
    }
    delete list;
    //panaikinam paskutini stulpeli
    ui->constraint_grid->removeWidget(lastColumn.last());
    delete lastColumn.takeLast();

    numFunctions--;

    this->adjustSize();
}

void Widget::calculate()
{

    Function optFunction;
    for(int i=0;i<numVariables;i++)
    {
        Variable var;
        var.letter=optimizationLabels.at(i)->text();
        var.multiplier=optimizationLineEdits.at(i)->text().toDouble();
        optFunction.variables.append(var);
    }

    QList<Function>restrictionFunctions;
    for(int i=0;i<numFunctions;i++)
    {
        Function restrFunction;
        for(int j=0;j<numVariables;j++)
        {
            Variable var;
            var.letter=restrictionLabels.at(j)->text();
            var.multiplier=restrictionLineEdits.at(i)->at(j)->text().toDouble();
            restrFunction.variables.append(var);
        }
        restrFunction.value=static_cast<QLineEdit*>(lastColumn.at(i+1))->text().toDouble();
        restrictionFunctions.append(restrFunction);
    }


    Calc calc;
    calc.setObjectiveFunction(optFunction);
    calc.setConstraintFunctions(restrictionFunctions);
    Answer answer=calc.execute();

    //atvaizduojame
    displayAnswer(answer);
}

void Widget::displayAnswer(Answer &answer)
{

    for(int i=0,n=answerLabels.size();i<n;i++)
        ui->result_layout->removeWidget(answerLabels.at(i));
    while(!answerLabels.isEmpty())
        delete answerLabels.takeLast();


    QString num;
    num.setNum(answer.value);
    QLabel *answerLabel=new QLabel("Functions max = "+num);
    answerLabels.append(answerLabel);
    ui->result_layout->insertWidget(0,answerLabel);

    for(int i=0,n=answer.variables.size();i<n;i++)
    {
        num.setNum(answer.variables.at(i).multiplier);
        QLabel *label=new QLabel(answer.variables.at(i).letter+" = "+num);
        answerLabels.append(label);
        ui->result_layout->insertWidget(0,label);
    }
}

void Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
