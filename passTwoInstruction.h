#ifndef PASSTWOINSTRUCTION_H
#define PASSTWOINSTRUCTION_H

#include <string>
using namespace std;
class passTwoInstruction
{
    public:
        string code;
        string lc;
        string label;
        string opcode;
        string operand;
        string comment;
        passTwoInstruction(string oLc, string oCode, string oLabel, string oOpcode, string oOperand, string oComment);
        passTwoInstruction();
        virtual ~passTwoInstruction();
        string getCode();
        string getLabel();
        string getOpcode();
        string getLc();
        string getOperand();
        string getComment();
    protected:
    private:
};

#endif // PASSTWOINSTRUCTION_H
