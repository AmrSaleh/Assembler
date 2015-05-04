#include "IntermediateObject.h"
#include <string>
using namespace std;
IntermediateObject::IntermediateObject(string oType, string oLc, string oLabel, string oOpcode, string oOperand, string oComment)
{
    type=oType;
    lc = oLc;
    label = oLabel;
    opcode = oOpcode;
    operand = oOperand;
    comment = oComment;
}
IntermediateObject::IntermediateObject()
{
    type="";
    lc = "";
    label="";
    opcode = "";
    operand = "";
    comment = "";
}

IntermediateObject::~IntermediateObject()
{
    //dtor
}
string IntermediateObject::getType()
{
    return type;
}
string IntermediateObject::getLc()
{
    return lc;
}
string IntermediateObject::getLabel()
{
    return label;
}
string IntermediateObject::getOpcode()
{
    return opcode;
}
string IntermediateObject::getOperand()
{
    return operand;
}
string IntermediateObject::getComment()
{
    return comment;
}
