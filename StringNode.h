#ifndef STRINGNODE_H
#define STRINGNODE_H
#include <string.h>
#include <iostream>

class StringNode
{
    public:
        std::string word;
        StringNode* next;
        StringNode(std::string value);
        ~StringNode();
    protected:
    private:
};

#endif // STRINGNODE_H
