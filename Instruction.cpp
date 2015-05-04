#include <string>
#include "Instruction.h"
using namespace std;
Instruction::Instruction(string oLc, string oLabel, string oOpcode, string oOperand, string oComment)
{
    lc = oLc;
    label = oLabel;
    opcode = oOpcode;
    operand = oOperand;
    comment = oComment;
}
Instruction::Instruction()
{
    lc = "";
    label="";
    opcode = "";
    operand = "";
    comment = "";
}

Instruction::~Instruction()
{
    //dtor
}
string Instruction::getLc()
{
    return lc;
}
string Instruction::getLabel()
{
    return label;
}
string Instruction::getOpcode()
{
    return opcode;
}
string Instruction::getOperand()
{
    return operand;
}
string Instruction::getComment()
{
    return comment;
}
