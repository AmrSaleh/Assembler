#include <iostream>
#include <stdio.h>
#include "StackF.h"

StackF::StackF()
{
    head=NULL;
    stackFSize=0;
}
StackF::~StackF()
{
    int i;
    for(i=0; i<stackFSize; i++)
    {
        pop();
    }
}
float StackF::peek()
{
    float elementAtTop;
    if (size()==0)
    {
        std::cout<<"Error"<<std::endl;
        return NULL;
    }
    elementAtTop=head->element;
    return elementAtTop;
}
float StackF::pop()
{
    if(size()==0)
    {
        std::cout<<"Error"<<std::endl;
        return NULL;
    }
    FloatNode* temp = head;
    float poppedValue=temp->element;
    if(size()==1)
    {
        head=NULL;
        stackFSize--;
        delete temp;
        return poppedValue;
    }
    head=head->next;
    stackFSize--;
    delete temp;
    return poppedValue;
}
void StackF::push(float element)
{
    FloatNode* pushedElement=new FloatNode(element);
    if(head==NULL)
    {
        head=pushedElement;
        stackFSize++;
        return;
    }
    pushedElement->next=head;
    head=pushedElement;
    stackFSize++;
    return;
}
int StackF::size()
{
    return stackFSize;
}
bool StackF::isEmpty()
{
    if(size()==0)
        return true;
    return false;
}
