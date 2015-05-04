#include <stdlib.h>     /* strtol */

bool isInteger(const string s)
    {
       if(s.empty() || !isdigit(s[0])) return false ;

       char * p ;
       strtol(s.c_str(), &p, 10) ;

       return (*p == 0) ;
    }
bool isHexDecimal(const string s)
{
   if(s.empty())
   {
       return false;
   }
    for(int i = 0; i < signed(s.size()); i++)
    {
        if(!isxdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}
bool checkDirectives(string label, string opcode, string operand)
{
    const char *cstr = opcode.c_str();
    if(strcasecmp(cstr, "BYTE")==0)
    {
        if(operand.size()==0)
        {
            return false;
        }
        if(strcasecmp(operand.substr(0,1).c_str(),"C")==0)
        {
            if(operand.size()>18||strcasecmp(operand.substr(1,2).c_str(),"'")!=0||strcasecmp(operand.substr(operand.size()-1,operand.size()).c_str(),"'")!=0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else if(strcasecmp(operand.substr(0,1).c_str(),"X")==0)
        {
            if(opcode.size()>17||strcasecmp(operand.substr(1,2).c_str(),"'")!=0||strcasecmp(operand.substr(operand.size()-1,operand.size()).c_str(),"'")!=0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else if(strcasecmp(cstr, "WORD")==0)
    {
        if(operand.size()==0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else if(strcasecmp(cstr, "RESB")==0
        ||strcasecmp(cstr, "RESW")==0)
    {
        if(operand.size()>4)
        {
            return false;
        }
        return isInteger(operand);
    }
    else if(strcasecmp(cstr, "START")==0)
    {
        if(operand.size()>4)
        {
            return false;
        }
        return isHexDecimal(operand);
    }
    else if(strcasecmp(cstr, "END")==0)
    {
        if(!label.empty())
        {
            return false;
        }
        return true;
    }
    else if(strcasecmp(cstr, "ORG")==0)
    {
        if(!label.empty())
        {
            return false;
        }
        return true;
    }
    else if(strcasecmp(cstr, "EQU")==0)
    {
        if(label.empty())
        {
            return false;
        }
        return true;
    }
    cout<<"Not a directive!"<<endl;
    return true;
}
