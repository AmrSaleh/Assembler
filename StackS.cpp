#include <iostream>
#include <stdio.h>
#include "StackS.h"
#include <string.h>
#include <iostream>

using namespace std;
StackS::StackS()
{
    head=NULL;
    stackSSize=0;
}
StackS::~StackS()
{
    int i;
    for(i=0; i<stackSSize; i++)
    {
        pop();
    }
}
string StackS::peek()
{
    string wordAtTop;
    if (size()==0)
    {
        std::cout<<"Error"<<std::endl;
        return NULL;
    }
    wordAtTop=head->word;
    return wordAtTop;
}
string StackS::pop()
{
    if(size()==0)
    {
        std::cout<<"Error"<<std::endl;
        return NULL;
    }
    StringNode* temp = head;
    string poppedValue=temp->word;
    if(size()==1)
    {
        head=NULL;
        stackSSize--;
        delete temp;
        return poppedValue;
    }
    head=head->next;
    stackSSize--;
    delete temp;
    return poppedValue;
}
void StackS::push(string word)
{
    StringNode* pushedWord=new StringNode(word);
    if(head==NULL)
    {
        head=pushedWord;
        stackSSize++;
        return;
    }
    pushedWord->next=head;
    head=pushedWord;
    stackSSize++;
    return;
}
int StackS::size()
{
    return stackSSize;
}
bool StackS::isEmpty()
{
    if(size()==0)
        return true;
    return false;
}
