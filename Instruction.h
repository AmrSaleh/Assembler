#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <iostream>
using namespace std;
class Instruction
{
    public:
        string lc;
        string label;
        string opcode;
        string operand;
        string comment;
        Instruction(string oLc, string oLabel, string oOpcode, string oOperand, string oComment);
        Instruction();
        virtual ~Instruction();
        string getLabel();
        string getOpcode();
        string getLc();
        string getOperand();
        string getComment();
    protected:
    private:

};

#endif // INSTRUCTION_H
