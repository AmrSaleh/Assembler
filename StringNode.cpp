#include <stdio.h>
#include "StringNode.h"
#include <string.h>
#include <iostream>
using namespace std;
StringNode::StringNode(string str)
{
    word=str;
    next=NULL;
}
StringNode::~StringNode()
{
    word="";
    next=NULL;
}
