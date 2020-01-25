#ifndef CALC_H
#define CALC_H

#include <QList>
#include <QString>

struct Variable
{
    QString letter;
    double multiplier;
};

enum Sign
{
    LESS,EQUAL,MORE,LESSEQUAL,MOREEQUAL
};

struct Function
{
    QList<Variable> variables;
    Sign sign;
    double value;

    void setSign(QString signStr)
    {
        if(signStr=="<")
            sign=LESS;
        else if(signStr==">")
            sign=MORE;
        else if(signStr=="<=")
            sign=LESSEQUAL;
        else if(signStr==">=")
            sign=MOREEQUAL;
        else
            sign=EQUAL;
    }

    void setSign(Sign sign)
    {
        this->sign=sign;
    }
};

struct Answer
{
    double value;
    QList<Variable> variables;
};

class Calc
{
public:

    Answer execute();

    //setters and getters
    void setObjectiveFunction(Function obj);
    void setConstraintFunctions(QList<Function> cf);
    void addConstraintFunction(Function f);
    void removeConstraintFunction(int at);

private:
    QList<Function*> allFunctions;
    QList<Function> constraintFunctions;
    Function objectiveFunction;

    // helper functions
    void relinkFunctions();
    int createSlacks();
    void addZeros();
    void invertObjective();
    Answer calcAnswer();
    bool nextTableu();
    bool isEqual(double x,double y, double eps=0.0000001);


};

#endif // CALC_H
