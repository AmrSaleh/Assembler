#ifndef STACKF_H
#define STACKF_H

#include "FloatNode.h"

class StackF
{
    public:
        FloatNode* head;
        int stackFSize;
        float peek();
        float pop();
        void push(float value);
        int size();
        bool isEmpty();
        StackF();
        ~StackF();
    protected:
    private:
};

#endif // STACKF_H
