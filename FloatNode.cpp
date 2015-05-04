#include <stdio.h>
#include "floatnode.h"

FloatNode::FloatNode(float num)
{
    element=num;
    next=NULL;
}
FloatNode::~FloatNode()
{
    element=0;
    next=NULL;
}
