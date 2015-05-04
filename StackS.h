#ifndef STACKS_H
#define STACKS_H
#include <string.h>
#include "StringNode.h"
#include <iostream>

class StackS
{
    public:
        StringNode* head;
        int stackSSize;
        std::string peek();
        std::string pop();
        void push(std::string str);
        int size();
        bool isEmpty();
        StackS();
        ~StackS();
    protected:
    private:
};

#endif // STACKS_H
