#ifndef INTERMEDIATEOBJECT_H
#define INTERMEDIATEOBJECT_H
#include <string>
using namespace std;

class IntermediateObject
{
    public:
        string type;
        string lc;
        string label;
        string opcode;
        string operand;
        string comment;
        IntermediateObject(string oType, string oLc, string oLabel, string oOpcode, string oOperand, string oComment);
        IntermediateObject();
        virtual ~IntermediateObject();
        string getType();
        string getLabel();
        string getOpcode();
        string getLc();
        string getOperand();
        string getComment();
    protected:
    private:
};

#endif // INTERMEDIATEOBJECT_H
