#include "passTwoInstruction.h"
#include <string>
using namespace std;
passTwoInstruction::passTwoInstruction(string oLc, string oCode, string oLabel, string oOpcode, string oOperand, string oComment)
{
    code=oCode;
    lc = oLc;
    label = oLabel;
    opcode = oOpcode;
    operand = oOperand;
    comment = oComment;
}
passTwoInstruction::passTwoInstruction()
{
    code="";
    lc = "";
    label="";
    opcode = "";
    operand = "";
    comment = "";
}

passTwoInstruction::~passTwoInstruction()
{
    //dtor
}
string passTwoInstruction::getCode()
{
    return code;
}
string passTwoInstruction::getLc()
{
    return lc;
}
string passTwoInstruction::getLabel()
{
    return label;
}
string passTwoInstruction::getOpcode()
{
    return opcode;
}
string passTwoInstruction::getOperand()
{
    return operand;
}
string passTwoInstruction::getComment()
{
    return comment;
}
