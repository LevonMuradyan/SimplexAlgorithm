#include "calc.h"
#include <cmath>

void Calc::setConstraintFunctions(QList<Function>cf)
{
    constraintFunctions=cf;
    relinkFunctions();
}

void Calc::addConstraintFunction(Function f)
{
    constraintFunctions.append(f);
    relinkFunctions();
}

void Calc::setObjectiveFunction(Function f)
{
    objectiveFunction=f;
    relinkFunctions();
}

void Calc::removeConstraintFunction(int at)
{
    if(constraintFunctions.size()<=at)
        return;

    constraintFunctions.removeAt(at);
    relinkFunctions();
}

Answer Calc::execute()
{
    //prepare
    invertObjective();
    createSlacks();
    addZeros();

    //iterate
    while(nextTableu())
    {
        // do nothing
    }

    return calcAnswer();
}

Answer Calc::calcAnswer()
{
    Answer answer;

    for(int col=0,n=objectiveFunction.variables.size();col<n;col++)
    {
        // check for single non-zero variable
        int numValues=0;
        Variable var;
        double value;

        for(int row=0,m=allFunctions.size();row<m;row++)
        {
            if(allFunctions.at(row)->variables.at(col).multiplier != 0)
            {
                var=allFunctions.at(row)->variables.at(col);
                value=allFunctions.at(row)->value;
                numValues++;
            }
        }

        // if single non zero calculate variable
        if(numValues==1)
        {
            Variable ansVar;
            ansVar.letter=var.letter;
            ansVar.multiplier=value/var.multiplier;
            answer.variables.append(ansVar);
        }
    }

    // overall value is the value of objective functions value
    answer.value=objectiveFunction.value;

    return answer;
}

bool Calc::nextTableu()
{
    // find pivot column
    int scolumn=0;
    for(int i=1,n=objectiveFunction.variables.size();i<n;i++)
    {
        if(objectiveFunction.variables.at(i).multiplier < objectiveFunction.variables.at(scolumn).multiplier)
            scolumn=i;
    }

    // check if a negative value
    if(objectiveFunction.variables.at(scolumn).multiplier > 0 || isEqual(objectiveFunction.variables.at(scolumn).multiplier,0.0))
        return false;

    // find pivot row
    int srow=0;
    int minKoef;
    bool minKoefSet=false;
    //get smallest koef in rows with positive pivots
    for(int i=0,n=allFunctions.size();i<n;i++)
    {
        if(allFunctions.at(i)->variables.at(scolumn).multiplier > 0)
        {
            if(minKoefSet)
            {
                if( (allFunctions.at(i)->value / allFunctions.at(i)->variables.at(scolumn).multiplier) < minKoef )
                {
                    minKoef=allFunctions.at(i)->value / allFunctions.at(i)->variables.at(scolumn).multiplier;
                    srow=i;
                }
            }
            else
            {
                minKoef = allFunctions.at(i)->value / allFunctions.at(i)->variables.at(scolumn).multiplier;
                minKoefSet=true;
            }
        }
    }

    //if we haven't found pivot return false
    if(!minKoefSet)
        return false;

    // use the pivot to clear the pivot column in the normal manner, this gives the next tableau
    for(int i=0,n=allFunctions.size();i<n;i++)
    {
        if(i!=srow)
        {
            // check if it isnt zero
            if(allFunctions.at(i)->variables.at(scolumn).multiplier!=0)
            {
                //get koef
                double koef = allFunctions.at(i)->variables.at(scolumn).multiplier / allFunctions.at(srow)->variables.at(scolumn).multiplier;

                //change variables
                for(int j=0,m=objectiveFunction.variables.size();j<m;j++)
                {
                    allFunctions.at(i)->variables[j].multiplier -= koef * allFunctions.at(srow)->variables.at(j).multiplier;

                    //if variable very near zero , make it zero
                    if(isEqual(allFunctions.at(i)->variables.at(j).multiplier,0.0))
                        allFunctions.at(i)->variables[j].multiplier = 0.0;
                }
                //change value
                allFunctions.at(i)->value -= koef * allFunctions.at(srow)->value;
            }
        }
    }

    return true;
}

int Calc::createSlacks()
{
    // create slack variables
    Variable slack;
    QString slackNumber;
    int numSlacks=constraintFunctions.size()+1;

    for(int i=0;i<constraintFunctions.size();i++)
    {
        for(int j=0;j<numSlacks;j++)
        {
            slackNumber.setNum(j);
            slack.letter="s"+slackNumber;

            if(j==i)
                slack.multiplier=1;
            else
                slack.multiplier=0;

            constraintFunctions[i].variables.append(slack);
        }
    }

    // add slacks to objective function
    slack.multiplier=0;
    for(int i=0;i<numSlacks-1;i++)
    {
        slackNumber.setNum(i);
        slack.letter="s"+slackNumber;
        objectiveFunction.variables.append(slack);
    }
    slackNumber.setNum(numSlacks-1);
    slack.letter="s"+slackNumber;
    slack.multiplier=1;
    objectiveFunction.variables.append(slack);

    return numSlacks;
}

void Calc::relinkFunctions()
{
    //clear old pointers
    allFunctions.clear();
    //get new constraints
    for(int i=0,n=constraintFunctions.size();i<n;i++)
        allFunctions.append(&constraintFunctions[i]);
    //get objectiveFunction
    allFunctions.append(&objectiveFunction);
}

void Calc::addZeros()
{
    // scan constraint functions and add zero variables
    for(int i=0,n=constraintFunctions.size();i<n;i++)
    {
        for(int j=0,m=objectiveFunction.variables.size();j<m;j++)
        {
            bool found=false;
            for(int k=0,n=constraintFunctions.at(i).variables.size();k<n;k++)
            {
                if(objectiveFunction.variables.at(j).letter == constraintFunctions.at(i).variables.at(k).letter)
                {
                    found=true;
                    break;
                }
            }
            // if variable not found add it
            if(!found)
            {
                Variable var;
                var.letter=objectiveFunction.variables.at(j).letter;
                var.multiplier=0;

                constraintFunctions[i].variables.append(var);
            }
        }
    }
}

void Calc::invertObjective()
{
    for(int i=0,n=objectiveFunction.variables.size();i<n;i++)
    {
        objectiveFunction.variables[i].multiplier*=-1;
    }
}

bool Calc::isEqual(double x, double y, double eps)
{
    if(x==y)
        return true;

    if( fabs(x - y) < eps)
        return true;
    else
        return false;
}
