#include <iostream>
#include <string.h>
#include <locale>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include "Instruction.h"
#include "passTwoInstruction.h"
#include "StackS.h"
#include "StackF.h"
#include "IntermediateObject.h"
using namespace std;
string filename;
ifstream inFile;
ifstream inFile2;
bool rel = false;
string errorStatement = "" ;
string line;
string opCodes[]= {"add","addf","addr","and","clear","comp","compf","compr",
                   "div","divf","divr","fix","float","hio","j","jeq","jgt","jlt","jsub","lda",
                   "ldb","ldch","ldf","ldl","lds","ldt","ldx","lps","mul","mulf","mulr","norm",
                   "or","rd","rmo","rsub","shiftl","shiftr","sio","ssk","sta","stb","stch","stf",
                   "sti","stl","sts","stsw","stt","stx","sub","subf","subr","svc","td","tio",
                   "tix","tixr","wd"
                  };
string objectCodes[]= {"18","58","90","40","B4","28","88","A0",
                       "24","64","9C","C4","C0","F4","3C","30","34","38","48","00",
                       "68","50","70","08","6C","74","04","D0","20","60","98","C8",
                       "44","D8","AC","4C","A4","A8","F0","EC","0C","78","54","80",
                       "D4","14","7C","E8","84","10","1C","5C","94","B0","E0","F8",
                       "2C","B8","DC"
                      };
int opCodesFormat[]= {3,3,2,3,2,3,3,2,
                      3,3,2,1,1,1,3,3,3,3,3,3,
                      3,3,3,3,3,3,3,3,3,3,2,1,
                      3,3,2,3,2,2,1,3,3,3,3,3,
                      3,3,3,3,3,3,3,3,2,2,3,1,
                      3,2,3
                     };
bool ReadLine();
bool isOPcode(string value);
bool ReadLineInternally(string line);
bool isDirective(string word);
bool ReadLineFromIntFile();
string decimalToHex(int decimal_value);
int hexToDecimal(string hex_value);
void stringToUpper(string &s);
int getRregisterNumber(string reg);
string generateObjCode(string addressOfThisLine, string opCode, string operand);
class Symbol
{
private:
    string sym;
    string loc;
    bool isRelocatable;
public:
    bool getType()
    {
        return isRelocatable;
    }

    string getSym()
    {
        return sym;
    }
    string getLoc()
    {
        return loc;
    }
    void setType(bool s)
    {
        this->isRelocatable = s;
    }
    void setSym(string s)
    {
        this->sym = s;
    }
    void setLoc(string l)
    {
        this->loc = l;
    }
};

int startingAddress = 0;
int LOCCTR = 0;
Symbol symTab[100];
int sizeOfSym = 0;
int progLength = 0;
string label;
string opCode;
string operand;
string comment;
string lineType;
string lineAddress;
Symbol* searchSymTab(string sym)
{
    int var;
    for (var = 0; var < sizeOfSym; ++var)
    {
        if (symTab[var].getSym() == sym)
        {
            return &symTab[var];
        }
    }
    return NULL;
}

void insert(string s, int l, bool isRel)
{
    Symbol sym;
    char buffer [33];
    string loc = itoa(l,buffer,16);
    sym.setLoc(loc);
    sym.setSym(s);
    sym.setType(isRel);
    symTab[sizeOfSym] = sym;
    sizeOfSym++;
}
string adjustSpaces(string toAdjust, int maximumSize)
{
    int difference = maximumSize-toAdjust.length();
    {
        for(int i = 0; i < difference; i++)
        {
            toAdjust.append(" ");
        }
    }
    return toAdjust;
}
void write(string output)
{
    ofstream myfile;
    myfile.open ("LISTFILE.txt", std::ios_base::app);
    output = output + "\n";
    myfile << output;
}
void writeInt(string output)
{
    ofstream myfile;
    myfile.open ("INTFILE.txt", std::ios_base::app);
    output = output + "\n";
    myfile << output;
}
void writeObjectFile(string output)
{
    ofstream myfile;
    myfile.open ("OBJFILE.txt", std::ios_base::app);
    output = output + "\n";
    myfile << output;
}
void open()
{
    ofstream myfile;
    myfile.open("LISTFILE.txt");
    string output = ">>  Source Program statements with value of LC indicated\n";
    write(output);
}
void openInt()
{
    ofstream myfile;
    myfile.open("INTFILE.txt");
}
void openObjectFile()
{
    ofstream myfile;
    myfile.open("OBJFILE.txt");
}

void writeSymbolTable(Symbol symbols [], int sizeOfArray)
{
    string output = "\n>>    e n d    o f   p a s s   1 \n\n";
    output.append(">>   ********************************************************\n");
    output.append(">>    s y m b o l     t a b l e   (values in hexadecimal)\n\n");
    output.append("       name           value    Absol/Reloc\n");
    output.append("       ----------------------------------------");
    write(output);
    for(int i = 0; i < sizeOfArray; i++)
    {
        string type = "";
        if(symbols[i].getType())
        {
            type="Relocatable";
        }
        else
        {
            type="Absolute";
        }
        output = "       "+adjustSpaces(symbols[i].getSym(),14)+" "+adjustSpaces(symbols[i].getLoc(),8)+" "+adjustSpaces(type,12);
        write(output);
    }
    output = ">>   ********************************************************";
    write(output);
}
void writeLine(Instruction* instruction)
{
    string output = "";
    if(instruction->getOpcode() == "\0")
    {
        output = "      ."+adjustSpaces(instruction->getComment(),40);
    }
    else
    {
        output = adjustSpaces(instruction->getLc(),6)+" "+
                 adjustSpaces(instruction->getLabel(),8)+" "+adjustSpaces(instruction->getOpcode(),7)+" "+
                 adjustSpaces(instruction->getOperand(),12)+" "+adjustSpaces(instruction->getComment(),20);
    }
    write(output);
}
void writePassTwoLine(passTwoInstruction* instruction)
{
    string output = "";
    if(instruction->getOpcode() == "\0")
    {
        output = "              ."+adjustSpaces(instruction->getComment(),40);
    }
    else
    {
        output = adjustSpaces(instruction->getLc(),6)+" "+
                 adjustSpaces(instruction->getCode(),8)+" "+
                 adjustSpaces(instruction->getLabel(),8)+" "+
                 adjustSpaces(instruction->getOpcode(),7)+" "+
                 adjustSpaces(instruction->getOperand(),12)+" "+
                 adjustSpaces(instruction->getComment(),20);
    }
    write(output);
}
void writeIntLine(IntermediateObject* intermediate)
{
    string output = "";
    if(intermediate->getOpcode() == "\0")
    {
        output = adjustSpaces(intermediate->getType(),1)+
                 "  "+adjustSpaces(intermediate->getLc(),6)+
                 "  ."+adjustSpaces(intermediate->getComment(),40);
    }
    else
    {
        output = adjustSpaces(intermediate->getType(),1)+"  "+
                 adjustSpaces(intermediate->getLc(),6)+"  "+
                 adjustSpaces(intermediate->getLabel(),8)+"  "+
                 adjustSpaces(intermediate->getOpcode(),7)+"  "+
                 adjustSpaces(intermediate->getOperand(),12)+"  "+
                 adjustSpaces(intermediate->getComment(),20);
    }
    writeInt(output);
}
void startPassTwoLine()
{
    string output = ">>   S t a r t   o f    P a s s   I I \n\n";
    output=output+">>   A s s e m b l e d    p r o g r a m     l i s t i n g\n\n";
    output=output+"LC     Code     Source   Statement\n\n";

    write(output);
}
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
string registerOpCodes[]= {"addr","compr","divr","mulr","rmo","subr","tixr"};
string registers[]= {"a","x","l","b","s","t","f","pc","sw"};
bool isRegister(string reg)
{
    for (int i=0 ; i<9 ; i++)
    {
        if(strcasecmp(reg.c_str(), registers[i].c_str())==0)
        {
            return true;
        }
    }
    return false;
}

bool isCorrectOperand(string opCode,string operand)
{

    if(strcasecmp(opCode.c_str(), "tixr")==0)
    {
        for (int i=0 ; i<9 ; i++)
        {
            if(strcasecmp(operand.c_str(), registers[i].c_str())==0)
            {
                return true;
            }
        }
        return false;
    }
    else if((strcasecmp(opCode.c_str(), "addr")==0)||
            (strcasecmp(opCode.c_str(), "compr")==0)||
            (strcasecmp(opCode.c_str(), "divr")==0)||
            (strcasecmp(opCode.c_str(), "mulr")==0)||
            (strcasecmp(opCode.c_str(), "rmo")==0)||
            (strcasecmp(opCode.c_str(), "subr")==0))
    {

        string firstReg="";
        string secondReg="";
        unsigned int k =0;
        while(k<operand.length())
        {
            if(operand[k]==',')
            {
                k++;
                break;
            }
            firstReg=firstReg+operand[k];
            k++;
        }
        while(k<operand.length())
        {
            secondReg=secondReg+operand[k];
            k++;
        }
        if((isRegister(firstReg))&&(isRegister(secondReg)))
        {
            return true;
        }



        return false;
    }

    return true;
}
string evalPostfix(string postfix, int relocatable)
{
    StackS result;
    string semi;
    istringstream part(postfix);
    while (part >>semi)
    {
        cout<<semi<<endl;
        if(semi=="+"||semi=="-"||semi=="*"||semi=="/")
        {
            string first = result.pop();
            string second = result.pop();

            if(semi=="+")
            {

                if(first=="absolute"&&second=="absolute")
                {
                    result.push("absolute");
                }
                else if(first.find("relocatable")!=string::npos&&second=="absolute")
                {
                    result.push(first);
                }
                else if(first=="absolute"&&second.find("relocatable")!=string::npos)
                {
                    result.push(second);
                }
                else
                {
                    ostringstream num;
                    string temp1 = first.substr(11,first.length()-11);
                    string temp2 = second.substr(11,second.length()-11);
                    char* firstAsCharPointer = &temp1[0];
                    char* secondAsCharPointer = &temp2[0];
                    num << (atoi(firstAsCharPointer)+atoi(secondAsCharPointer));
                    result.push("relocatable"+num.str());
                }
            }
            else if(semi=="-")
            {
                ostringstream num;
                if(first=="absolute"&&second=="absolute")
                {
                    result.push("absolute");
                }
                else if(first.find("relocatable")!=string::npos&&second=="absolute")
                {
                    string temp1 = first.substr(11,first.length()-11);
                    char* firstAsCharPointer = &temp1[0];
                    num << (-1*atoi(firstAsCharPointer));
                    result.push("relocatable"+num.str());
                }
                else if(first=="absolute"&&second.find("relocatable")!=string::npos)
                {
                    result.push(second);
                }
                else
                {
                    string temp1 = first.substr(11,first.length()-11);
                    string temp2 = second.substr(11,second.length()-11);
                    char* firstAsCharPointer = &temp1[0];
                    char* secondAsCharPointer = &temp2[0];
                    num << (-1*atoi(firstAsCharPointer)+atoi(secondAsCharPointer));
                    result.push("relocatable"+num.str());
                }
            }
            else if(semi=="*")
            {
                if((first=="absolute"||first=="relocatable0")&&(second=="absolute"||second=="relocatable0"))
                {
                    result.push("absolute");
                }
//                else if((first=="relocatable"&&second=="absolute")
//                        || (first=="absolute"&&second=="relocatable"))
//                {
//                      return false;
//                }
                else
                {
                    errorStatement = "Operand has multiplication involving relocatable symbols";
                    return "";
                }
            }
            else
            {
                if((first=="absolute"||first=="relocatable0")&&(second=="absolute"||second=="relocatable0"))
                {
                    result.push("absolute");
                }
//                else if((first=="relocatable"&&second=="absolute")
//                        || (first=="absolute"&&second=="relocatable"))
//                {
//                    result.push("relocatable");
//                }
                else
                {
                    errorStatement = "Operand has division involving relocatable symbols";
                    return "";
                }
            }
        }
        else
        {
            result.push(semi);
        }
    }
    string finalValue = result.pop();
    cout<<"finalValue: "<<finalValue<<" "<<"relocatable integer: "<<relocatable<<endl;
    if((finalValue=="absolute"||finalValue=="relocatable0")&&(relocatable==2||relocatable==0))
    {
        return finalValue;
    }
    else if(finalValue=="relocatable1"&&(relocatable==1||relocatable==0))
    {
        return finalValue;
    }
    errorStatement = "Operand has an invalid expression.";
    return "";

}
string turnToPostfixAndEvaluate(string toPostfix, int relocatable) //write this method pls
{
    cout<<toPostfix<<endl;
    string postFix="";
    StackS postFixer;
    string semi;
    istringstream part(toPostfix);
    while (part >>semi)
    {
        while((semi=="+"||semi=="-")&&!postFixer.isEmpty()&&(postFixer.peek()=="*"||postFixer.peek()=="/"))
        {
            postFix+=postFixer.pop()+" ";
        }
        if((semi=="+"||semi=="-")&&!postFixer.isEmpty()&&(postFixer.peek()=="+"||postFixer.peek()=="-"))
        {
            postFix+=postFixer.pop()+" ";
            postFixer.push(semi);
        }
        else if((semi=="*"||semi=="/")&&!postFixer.isEmpty()&&(postFixer.peek()=="*"||postFixer.peek()=="/"))
        {
            postFix+=postFixer.pop()+" ";
            postFixer.push(semi);
        }
        else if(semi=="*"||semi=="/"||semi=="+"||semi=="-"||semi=="(")
        {
            postFixer.push(semi);
        }
        else if(semi==")")
        {
            while(postFixer.peek()!="(")
            {
                postFix+=postFixer.pop()+" ";
            }
            postFixer.pop();
        }
        else
        {
            postFix+=semi+" ";
        }
    }
    while(!postFixer.isEmpty())
    {
        postFix+=postFixer.pop()+" ";
    }
    return evalPostfix(postFix,relocatable);
}
int checkInSymTab(string symbol) //write this method pls
{
    Symbol* toBeChecked = searchSymTab(symbol);
    if(toBeChecked==NULL)
    {
        return -1;
    }
    if(toBeChecked->getType())
    {
        return 1;
    }
    else
    {
        return 2;
    }
}
string boolToString(bool b)
{
    return b ? "true" : "false";
}
float addressPostfixResult(string postfix)
{
    StackF result;
    string semi;
    istringstream part(postfix);
    while (part >>semi)
    {
        if(semi=="+"||semi=="-"||semi=="*"||semi=="/")
        {
            if(semi=="+")
            {
                result.push(result.pop()+result.pop());
            }
            else if(semi=="-")
            {
                float value=(result.pop()-result.pop());
                value=-value;
                result.push(value);
            }
            else if(semi=="*")
            {
                result.push(result.pop()*result.pop());
            }
            else
            {
                float value=((result.pop())/result.pop());
                value=1/value;
                int integerValue = value;
                result.push(integerValue);
            }
        }
        else
        {
            float value;
            stringstream convert(semi);
            if (!(convert >> value))
            {
                value=0;
            }
            result.push(value);
        }
    }
    return result.pop();
}
int evaluateAddress(string operand)
{
    string postFix="";
    StackS postFixer;
    string semi;
    istringstream part(operand);
    while (part >>semi)
    {
        while((semi=="+"||semi=="-")&&!postFixer.isEmpty()&&(postFixer.peek()=="*"||postFixer.peek()=="/"))
        {
            postFix+=postFixer.pop()+" ";
        }
        if((semi=="+"||semi=="-")&&!postFixer.isEmpty()&&(postFixer.peek()=="+"||postFixer.peek()=="-"))
        {
            postFix+=postFixer.pop()+" ";
            postFixer.push(semi);
        }
        else if((semi=="*"||semi=="/")&&!postFixer.isEmpty()&&(postFixer.peek()=="*"||postFixer.peek()=="/"))
        {
            postFix+=postFixer.pop()+" ";
            postFixer.push(semi);
        }
        else if(semi=="*"||semi=="/"||semi=="+"||semi=="-"||semi=="(")
        {
            postFixer.push(semi);
        }
        else if(semi==")")
        {
            while(postFixer.peek()!="(")
            {
                postFix+=postFixer.pop()+" ";
            }
            postFixer.pop();
        }
        else
        {
            ostringstream number;
            if(isInteger(semi))
            {
                postFix+=semi+" ";
            }
            else
            {
                number<<hexToDecimal(searchSymTab(semi)->getLoc());
                postFix+=number.str()+" ";
            }

        }
    }
    while(!postFixer.isEmpty())
    {
        postFix+=postFixer.pop()+" ";
    }
    return addressPostfixResult(postFix);
}
string validNotation(string operand, int relocatable, string currentLocation)
{
    //relocatable cases: 0 if it could be relocatable/absolute
    //                    1 if it must be relocatable
    //                      2 if it must be absolute
    string output = "";
    string absReloc = "";
    if(operand=="")
    {
        output = "##FALSE##";
        errorStatement = "Operand is empty";
        return output;
    }
    if(operand=="*")
    {
        return currentLocation;
    }
    string operandTemp = operand.substr(0, operand.length());
    char *cstr = &operandTemp[0];
    //strcpy(cstr, operand.c_str());
    int i = 0;
    char *pch = strtok (cstr,"+-*/");
    string processedOperand = operand.substr(0,operand.length());
    //cout<<"Error check, processedOperand: "<<processedOperand<<endl;
    //bool immediate = false;
    //bool indirect = false;
    bool indexed = false;
    int bracketValidation = 0;
    //cout<<"Output:    "<<output<<endl;
    while(pch != NULL)
    {
        //cout<<"loop"<<endl;
        int closedBracketCount = 0;
        int index = processedOperand.find_first_of("+-*/");
        cout<<"Current expression: "<<processedOperand<<" index: "<<index<<endl;
        if(indexed)
        {
            output = "##FALSE##";
            errorStatement = "Operand has an invalid indexed format";
            return output;
        }
        string token = std::string (pch);
        if(processedOperand.length()>(index+1)&&index==0&&(processedOperand.substr(index+1,1)=="+"||processedOperand.substr(index+1,1)=="-"||
             processedOperand.substr(index+1,1)=="*"||processedOperand.substr(index+1,1)=="/"
            ||processedOperand.substr(index+1,1)==")")&&
            (processedOperand.substr(index,1)=="*"))
        {
            string temp="";
            if(index>0)
            {
                temp=temp+processedOperand.substr(0,index-1);
            }

            temp=temp+"!current!"+processedOperand.substr(index+1,processedOperand.length()-index-1);
            processedOperand=temp;
            //cout<<"Current expression: "<<processedOperand<<" index is: "<<index<<endl;
            index = processedOperand.find_first_of("+-*/");
            //cout<<"Current expression2: "<<processedOperand<<" index is: "<<index<<endl;
            token=processedOperand.substr(0,index);
            //cout<<"Current expression3: "<<processedOperand<<" index is: "<<index<<endl;
        }
        else if(index==0&&processedOperand.length()>(index)&&(processedOperand.substr(index,1)=="*")&&
                (processedOperand.length()==(index+1)||(processedOperand.length()>(index+1)&&processedOperand.substr(index+1,processedOperand.length()-index-1).find_first_of("+-*/")==string::npos)))
        {
            cout<<"Trying to fix"<<endl;
            string temp="";
            if(index>0)
            {
                temp=temp+processedOperand.substr(0,index-1);
            }

            temp=temp+"!current!";
            if(processedOperand.length()>index)
            {
                temp=temp+processedOperand.substr(index+1,processedOperand.length()-index-1);
            }
            processedOperand=temp;
            index = processedOperand.find_first_of("+-*/");
            string temp2 = processedOperand.substr(0,index);
            token= temp2;
        }
        //cout<<"Current expression4: "<<processedOperand<<" index is: "<<index<<endl;
        //cout<<"Current operand: "<<token<<endl;
        if(token.length()==0)
        {
            output = "##FALSE##";
            errorStatement = "Operand has misplaced operators";
            return output;
        }
        if(token.length()>1&&strcasecmp(token.substr(token.length()-2,2).c_str(),",x")==0)
        {
            token = token.substr(0, token.length()-2);
            indexed = true;
        }
        //cout<<token<<endl;

        if(token.length()>0&&i!=0 && (strcasecmp(token.substr(0,1).c_str(),"@")==0
                                      ||strcasecmp(token.substr(0,1).c_str(),"#")==0))
        {
            output = "##FALSE##";
            errorStatement = "Operand has misplaced indirect/immediate operators";
            return output;
        }
        else if(token.length()>0&&i==0 && strcasecmp(token.substr(0,1).c_str(),"@")==0)
        {
       //     indirect = true;
            token=token.substr(1,token.length());
        }
        else if(token.length()>0&&i==0 && strcasecmp(token.substr(0,1).c_str(),"#")==0)
        {
       //     immediate = true;
            token=token.substr(1,token.length());
        }
        while(token.length()>0&&token.substr(0,1)=="(")
        {
            bracketValidation++;
            output = output + " ( ";
            absReloc = absReloc + " ( ";
            token = token.substr(1,token.length());
            if(bracketValidation<0)
            {
                output = "##FALSE##";
                errorStatement = "1- Operand has misplaced brackets";
                return output;
            }
        }

        while(token.length()>0&&token.substr(token.length()-1,1)==")")
        {
            closedBracketCount++;
            //cout<<"bracket validation: "<<bracketValidation<<endl;
            token = token.substr(0,token.length()-1);
            if(bracketValidation<0)
            {
                output = "##FALSE##";
                errorStatement = token+" Operand has misplaced brackets";
                return output;
            }
        }
        if(token.length()>0&&(token.substr(0,1)!=")"&&token.substr(token.length()-1,1)!="("))
        {
            if(token =="")
            {
                output = "##FALSE##";
                errorStatement = "Operand has misplaced operators";
                return output;
            }
            string start = "";
            if(i!=0)
            {
                start = " ";
            }
            if(!isInteger(token))
            {
                if(token=="!current!")
                {
                    absReloc = absReloc +start+ "relocatable1 ";
                }
                else
                {
                    int type = checkInSymTab(token);//--->write this method!
                    if(type==-1)
                    {
                        output = "##FALSE##";
                        errorStatement = "Operand has symbols missing from the symbol table";
                        return output;
                    }
                    else if(type == 2)
                    {
                        absReloc = absReloc +start+ "absolute ";
                    }
                    else //1
                    {
                        absReloc = absReloc +start+ "relocatable1 ";
                    }
                }



            }
            else if(isInteger(token))
            {
                absReloc = absReloc +start+ "absolute ";
            }
            if(token=="!current!")
            {
                    ostringstream num;
                    num<<hexToDecimal(currentLocation);
                    output = output + start+num.str()+" ";
            }
            else
            {
                output = output + start+token+" ";
            }

        }
        else
        {
            output = "##FALSE##";
            errorStatement = "Operand has misplaced brackets";
            return output;
        }
        while(closedBracketCount>0)
        {
            output = output + " ) ";
            absReloc = absReloc + " ) ";
            bracketValidation--;
            closedBracketCount--;
        }
        //cout<<"Token is: "<<token<<endl;
        if(index != -1)
        {
            //cout<<"processedOperand:   " <<processedOperand<<endl;
            output = output + " "+processedOperand.at(index)+" ";
            absReloc = absReloc + " "+processedOperand.at(index)+" ";
            //if(i==0&&(immediate||indirect))
            {
           //     index=index+1;
            }
            //cout<<"index is: "<<index<<endl;
            processedOperand=processedOperand.substr(index+1,processedOperand.length()-index);
            //cout<<"second check processedOperand:   " <<processedOperand<<endl;

        }
        i++;
        if(index==-1)
        {
            break;
        }
        string processedOperandTemp = processedOperand.substr(0,processedOperand.length());
        if(processedOperandTemp=="*")
        {
            processedOperandTemp="!current!";
        }
        cstr=&processedOperandTemp[0];

        pch = strtok (cstr,"+-*/");
    }
    if(bracketValidation!=0)
    {
        output = "##FALSE##";
        errorStatement = "Operand has misplaced brackets";
        return output;
    }

    string expression = turnToPostfixAndEvaluate(absReloc,relocatable);
    if(expression=="")
    {
        output = "##FALSE##";
        return output;
    }
    else if(expression!="" && currentLocation=="")
    {
        return expression;
    }
    //cout<<output<<endl;
    output = decimalToHex(evaluateAddress(output));
    //output = output + " " + boolToString(indirect) + " " + boolToString(immediate) + " " + boolToString(indexed);
    cout<<output<<endl;
    return output;
}
bool checkDirectives(string label, string opcode, string operand)
{
    const char *cstr = opcode.c_str();
    if(strcasecmp(cstr, "BYTE")==0||operand.substr(0,1)=="=")
    {
        if(operand.substr(0,1)=="=")
        {
            operand=operand.substr(1,operand.length()-1);
        }
        if(operand.size()==0)
        {
            return false;
        }

        if(strcasecmp(operand.substr(0,1).c_str(),"C")==0)
        {
            if(operand.size()>18||strcasecmp(operand.substr(1,1).c_str(),"'")!=0||strcasecmp(operand.substr(operand.size()-1,1).c_str(),"'")!=0)
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
            if(opcode.size()>17||strcasecmp(operand.substr(1,1).c_str(),"'")!=0||strcasecmp(operand.substr(operand.size()-1,1).c_str(),"'")!=0)
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
        else if(operand.size()<1)
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
        string expressionResult = validNotation(operand,1,"");
        if(!label.empty()||strcasecmp(expressionResult.c_str(),"##FALSE##")==0)
        {
            return false;
        }
        return true;
    }
    else if(strcasecmp(cstr, "EQU")==0)
    {
        string expressionResult = validNotation(operand,0,"");
        if(label.empty()||strcasecmp(expressionResult.c_str(),"##FALSE##")==0)
        {
            return false;
        }
        if(expressionResult=="relocatable1")
        {
            rel = true;
        }
        else
        {
            rel = false;
        }
        return true;
    }
    else if(strcasecmp(cstr, "LTORG")==0)
    {
        if(!label.empty())
        {
            return false;
        }
        return true;
    }
    cout<<"Not a directive!"<<endl;
    return true;
}

int indexOfOpCode = 0;
string  literals[100][2];
int numOfLiterals = 0;
string getAddressFromLiterals(string literal)
{
    for (int i =0; i < numOfLiterals ; i++ )
    {
        string l = literals[i][0];
        if (strcasecmp(l.c_str(), literal.c_str()) == 0 )
        {
            return literals[i][1];
        }
    }
    return NULL;
}

bool thereIsLtorg = false;
void pass2(int start, string startString, string nameProg, string pl);
int main(int argc, char** fileInput)
{
    if(argc<2)
    {
        cout<<"please add file name parameter, Program terminated"<<endl;
        return 0;
    }
    cout<<" parameter is "<<argc<<"  "<<fileInput[1]<<endl;
    filename= fileInput[1];
    inFile.open( filename.c_str());
    if( !inFile )
    {
        cout << "Unable to open file: " << filename << endl;
        return -1;
    }
    openInt();
    int lo = 0;
    bool canRead = ReadLine();
    open();
    string e = "";
    bool isError = false;
    bool ThereIsError = false;
    while (opCode == "\0" && canRead)
    {
        IntermediateObject* line = new IntermediateObject("f","0",label,opCode,operand,comment);
        writeIntLine(line);
        write("."+comment);
        canRead = ReadLine();
    }
    string nameProg = "";
    string start = "";
    if (strcasecmp(opCode.c_str(), "START") == 0 && canRead)
    {
        nameProg = label;
        bool isValid = checkDirectives(label,opCode,operand);
        if (!isValid)
        {
            if (operand == "\0")
            {
                e = "** missing operand";
            }
            else
            {
                e = "Illegal operand";
            }
            isError = true;
            LOCCTR = 0;
        }
        else
        {
            if (operand == "\0")
            {
                LOCCTR = 0;
            }
            else
            {
                start = operand;
                std::stringstream ss;
                ss << std::hex << operand;
                ss >> startingAddress;
                LOCCTR = startingAddress;
                lo = LOCCTR;
            }
        }
        // write line;
        char buffer [33];
        string loc = itoa(lo,buffer,16);
        Instruction* p = new Instruction(loc,label,opCode,operand,comment);
        writeLine(p);
        IntermediateObject* line = new IntermediateObject("t",loc,label,opCode,operand,comment);
        writeIntLine(line);
        if (isError)
        {
            ThereIsError = true;
            write(e);
        }
        lo = LOCCTR;
        canRead = ReadLine();
    }
    isError = false;
    while (strcasecmp(opCode.c_str(), "end") != 0 && canRead)
    {
        isError = false;
        if (opCode != "\0")
        {
            // this is not a comment
            if (label != "\0")
            {
                // there is a symbol in the label field
                if (searchSymTab(label) != NULL)
                {
                    ThereIsError = true;
                    isError = true;
                    e = "**Error; Symbol " + label + " already defined";
                }
                else
                {
                    if (strcasecmp(opCode.c_str(), "equ") != 0){
                        insert(label, LOCCTR,true);
                    }
                }
            }
            bool found = isOPcode(opCode); // search for opCode
            bool isValidOperand = true;
            if (found)
            {
                if (operand[0] == '=')
                {
                    if (!checkDirectives(label,opCode,operand))
                    {
                        e = "**Error; Illegal operand.";
                        isError = true;
                    }
                    else
                    {
                        int x = opCodesFormat[indexOfOpCode];
                        if (x == 3 && opCode[0] == '+')
                        {
                            x = 4;
                        }
                        LOCCTR += x;
                        // add literal to array
                        bool founded = false;
                        for (int i = 0; i < numOfLiterals ; i++ )
                        {
                            string l = literals[i][0];
                            if (strcasecmp(l.c_str(), operand.c_str()) == 0 )
                            {
                                founded = true;
                                i = numOfLiterals;
                            }
                        }
                        if (!founded)
                        {
                            literals[numOfLiterals][0] = operand;
                            literals[numOfLiterals][1] = "";
                            numOfLiterals++;
                        }
                    }
                }
                else
                {
                    isValidOperand = isCorrectOperand(opCode,operand);
                    if (isValidOperand)
                    {
                        int x = opCodesFormat[indexOfOpCode];
                        if (x == 3 && opCode[0] == '+')
                        {
                            x = 4;
                        }
                        LOCCTR += x;
                    }
                }

            }
            else if (strcasecmp(opCode.c_str(), "word") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    LOCCTR += 3;
                }
            }
            else if (strcasecmp(opCode.c_str(), "resw") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    int x = atoi(operand.c_str());
                    LOCCTR += 3 * x;
                }
            }
            else if (strcasecmp(opCode.c_str(), "resb") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    int x = atoi(operand.c_str());
                    LOCCTR += x;
                }
            }
            else if (strcasecmp(opCode.c_str(), "byte") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    int x = strlen(operand.c_str()) - 3;
                    if (operand[0] == 'X' || operand[0] == 'x')
                    {
                        if (x % 2 == 1)
                        {
                            LOCCTR += x/2 + 1;
                        }
                        else
                        {
                            LOCCTR += x/2;
                        }
                    }
                    else
                    {
                        LOCCTR += x;
                    }
                }
            }
            else if (strcasecmp(opCode.c_str(), "equ") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    char buffer [33];
                    string loc = itoa(lo,buffer,16);
                    string result = validNotation(operand,0,loc);
                    if (result == "##FALSE##")
                    {
                        isError = true;
                        e = errorStatement;
                    }
                    else
                    {
                        // evaluate the expression and return the location counter
                        int r = hexToDecimal(result);
                        insert(label,r,rel);
                    }
                }
            }
            else if (strcasecmp(opCode.c_str(), "org") == 0)
            {
                isValidOperand = checkDirectives(label,opCode,operand);
                if (isValidOperand)
                {
                    // evaluate the expression and return the location counter
                    char buffer [33];
                    string loc = itoa(lo,buffer,16);
                    string result = validNotation(operand,1,loc);
                    if (result == "##FALSE##")
                    {
                        isError = true;
                        e = errorStatement;
                    }
                    else
                    {
                        LOCCTR = hexToDecimal(result);
                    }
                }
            }
            else if (strcasecmp(opCode.c_str(), "ltorg") == 0)
            {
                if (!checkDirectives(label,opCode,operand))
                {
                    e = "**Error; missplaced label.";
                    isError = true;
                }
                else
                {
                    thereIsLtorg =true;
                }
            }
            else
            {
                ThereIsError = true;
                isError = true;
                e =  "**Error; invalid operation code";
            }
            if (!isValidOperand)
            {
                ThereIsError = true;
                isError = true;
                e =  "**Illegal operand";
            }
        }
        char buffer [33];
        string loc = itoa(lo,buffer,16);
        stringToUpper(loc);
        Instruction* p = new Instruction(loc,label,opCode,operand,comment);
        writeLine(p);
        string type = "";
        if (opCode == "\0")
        {
            type = "f";
        }
        else
        {
            type = "t";
        }
        IntermediateObject* line = new IntermediateObject(type,loc,label,opCode,operand,comment);
        writeIntLine(line);
        if (thereIsLtorg)
        {
            for (int i = 0; i < numOfLiterals ; i++ )
            {
                label = "*";
                operand = "";
                opCode = literals[i][0];
                literals[i][1] = decimalToHex(LOCCTR);
                Instruction* p = new Instruction(literals[i][1],label,opCode,operand,comment);
                writeLine(p);
                IntermediateObject* line = new IntermediateObject("t",literals[i][1],label,opCode,operand,comment);
                writeIntLine(line);
                int x = strlen(opCode.c_str()) - 4;
                if (opCode[0] == 'X' || opCode[0] == 'x')
                {
                    if (x % 2 == 1)
                    {
                        LOCCTR += x/2 + 1;
                    }
                    else
                    {
                        LOCCTR += x/2;
                    }
                }
                else
                {
                    LOCCTR += x;
                }
            }
            thereIsLtorg = false;
        }
        if (isError)
        {
            write(e);
        }
        lo = LOCCTR;
        canRead = ReadLine();
    }
    char buffer [33];
    string loc = itoa(LOCCTR,buffer,16);
    string LABEL;
    stringToUpper(loc);
    Instruction* p = new Instruction(loc,label,opCode,operand,comment);
    writeLine(p);
    string type = "";
    if (opCode == "\0")
    {
        type = "f";
    }
    else
    {
        type = "t";
    }
    IntermediateObject* line = new IntermediateObject(type,loc,label,opCode,operand,comment);
    writeIntLine(line);
    if (strcasecmp(opCode.c_str(), "end") != 0)
    {
        ThereIsError = true;
    }

    for (int i = 0; i < numOfLiterals; i++ )
    {
        if (literals[i][1] == "\0")
        {
            label = "*";
            operand = "";
            opCode = literals[i][0];
            literals[i][1] = decimalToHex(LOCCTR);
            stringToUpper(literals[i][1]);
            Instruction* p = new Instruction(literals[i][1],label,opCode,operand,comment);
            writeLine(p);
            IntermediateObject* line = new IntermediateObject("t",literals[i][1],label,opCode,operand,comment);
            writeIntLine(line);
            int x = strlen(opCode.c_str()) - 4;
            if (opCode[0] == 'X' || opCode[0] == 'x')
            {
                if (x % 2 == 1)
                {
                    LOCCTR += x/2 + 1;
                }
                else
                {
                    LOCCTR += x/2;
                }
            }
            else
            {
                LOCCTR += x;
            }

        }
        if (i == numOfLiterals-1)
        {
            string l = decimalToHex(LOCCTR);
            stringToUpper(l);
            IntermediateObject* line = new IntermediateObject("t",l,"","End","","");
            writeIntLine(line);
        }
    }
    progLength = LOCCTR - startingAddress;

    if (!ThereIsError)
    {
        writeSymbolTable(symTab,sizeOfSym);
        char buffer [33];
        string pl = itoa(progLength-1,buffer,16);
        stringToUpper(pl);
        pass2(startingAddress,start,nameProg,pl);
    }
    else
    {
        write("Incomplete assembly");
    }
    inFile.close();
}



void pass2(int start, string startString, string nameProg, string pl)
{
    startPassTwoLine();
    openObjectFile();
    string e = "";
    bool isError = false;
    bool canRead = ReadLineFromIntFile();
    string objCode1 = "";
    while (opCode == "\0" && canRead)
    {
        write("."+comment);
        canRead = ReadLineFromIntFile();
    }
    if (strcasecmp(opCode.c_str(), "START") == 0 && canRead)
    {
        // write in listing file
        passTwoInstruction* instruction = new passTwoInstruction(lineAddress,"",label,opCode,operand,comment);
        writePassTwoLine(instruction);
        // write header record in to object program
        if(nameProg == "\0")
        {
            nameProg = "      ";
        }
        else if(nameProg.length() < 6)
        {
            int s = 6 - nameProg.length();
            for(int i = 0; i < s; i++)
            {
                nameProg += " ";
            }
        }
        if (startString.length() < 6)
        {
            int x = 6 - startString.length();
            for (int i = 0; i < x; i++)
            {
                startString = "0" + startString;
            }
        }
        if (pl.length() < 6)
        {
            int x = 6 - pl.length();
            for (int i = 0; i < x; i++)
            {
                pl = "0" + pl;
            }
        }
        nameProg += "^";
        string headerCode = "H^" + nameProg + startString + "^" + pl;
        writeObjectFile(headerCode);
        canRead = ReadLineFromIntFile();
    }
    int startAddress = start;
    string objCode = "";
    bool ThereIsError = false;
    string format4[100];
    int numOfForm4 = 0;
    bool res = false;
    string tr = "";
    string finalAddress = "";
    int current = start;
    int prev = 0;
    string tempObjectCode = "";
    bool isEnd = false;
    while (canRead)
    {
        isError = false;
        // this is not a comment line
        if (opCode != "\0")
        {
            // search opTab for opCode;
            bool found = isOPcode(opCode);

            if (found)
            {
                if (opCode[0] == '+')
                {
                    int ad =  hexToDecimal(lineAddress) + 1;
                    format4[numOfForm4] = decimalToHex(ad);
                    numOfForm4++;
                }
                string o = generateObjCode(lineAddress,opCode,operand);
                if (o == "\0")
                {
                    isError = true;
                    e = errorStatement;
                }
                else
                {
                    if (o.length() < 6)
                    {
                        int x = 6 - o.length();
                        for (int i = 0; i < x; i++)
                        {
                            o = "0" + o;
                        }
                    }
                    objCode =  "^" + o;
                    objCode1 = o;
                }
            }
            else if (strcasecmp(opCode.c_str(), "byte") == 0 || opCode[0] == '=')
            {
                if (opCode[0] == '=')
                {
                    operand = opCode.substr(1,opCode.length()-1);
                }
                if (operand[0] == 'x' || operand[0] == 'X')
                {
                    objCode = operand.substr(2,operand.length()-3);
                    if (objCode.length() < 6)
                    {
                        int x = 6 - objCode.length();
                        for (int i = 0; i < x; i++)
                        {
                            objCode = "0" + objCode;
                        }
                    }
                    objCode1 = objCode;
                    objCode = "^" + objCode;
                }
                else if (operand[0] == 'c' || operand[0] == 'C')
                {

                    string v = operand.substr(2,operand.length()-3);
                    unsigned int i;
                    objCode = "^";
                    for (i = 0; i < v.length() ; i++)
                    {
                        int x = (int)v[i];
                        char buffer [33];
                        string s = itoa(x,buffer,16);
                        objCode += s;
                        objCode1 += s;
                    }
                    if (objCode1.length() < 6)
                    {
                        int x = 6 - objCode1.length();
                        for (int i = 0; i < x; i++)
                        {
                            objCode1 = "0" + objCode1;
                        }
                    }

                }
                if (opCode[0] == '=')
                {
                    operand = "";
                }
            }
            else if (strcasecmp(opCode.c_str(), "word") == 0)
            {
                int x = atoi(operand.c_str());
                char buffer [33];
                string o = std::string(itoa(x,buffer,16));
                if (o.length() < 6)
                {
                    int x = 6 - o.length();
                    for (int i = 0; i < x; i++)
                    {
                        o = "0" + o;
                    }
                }
                objCode = "^" + o;
                objCode1 = o;
            }
            else
            {
                objCode = "";
            }
            //old address
            prev = current;
            std::stringstream ss;
            ss << std::hex << lineAddress;
            ss >> current;
            if (res)
            {
                //  startAddress = current;
                res = false;
            }
            int lengthRecord = prev - startAddress;
            int diff = current - prev;
            if (lengthRecord  + diff > 30 || strcasecmp(opCode.c_str(), "resw") == 0 || strcasecmp(opCode.c_str(), "resb") == 0)
            {
                // not fit
                // write text record to object program
                char buffer [33];
                string st = itoa(startAddress,buffer,16);
                if (st.length() < 6)
                {
                    int x = 6 - st.length();
                    for (int i = 0; i < x; i++)
                    {
                        st = "0" + st;
                    }
                }
                string length = itoa(prev - startAddress,buffer,16);
                if (length.length() < 2)
                {
                    int x = 2 - length.length();
                    for (int i = 0; i < x; i++)
                    {
                        length = "0" + length;
                    }
                }
                cout << "object code" << tr;
                if (tr != "\0")
                {
                    string temp = tr.substr(tr.length()-7,tr.length()-1);
                    if (!(strcasecmp(opCode.c_str(), "resw") == 0 || strcasecmp(opCode.c_str(), "resb") == 0))
                    {
                        tr = tr.substr(0,tr.length()-7);
                    }
                    else
                    {
                        length = itoa(current - startAddress,buffer,16);
                        if (length.length() < 2)
                        {
                            int x = 2 - length.length();
                            for (int i = 0; i < x; i++)
                            {
                                length = "0" + length;
                            }
                        }
                    }
                    stringToUpper(st);
                    stringToUpper(length);
                    stringToUpper(tempObjectCode);
                    stringToUpper(tr);
                    string textRecord = "T^" + st + "^" + length + tempObjectCode + tr;
                    tempObjectCode = temp;
                    writeObjectFile(textRecord);
                }
                // initialize new text record
                if (strcasecmp(opCode.c_str(), "resw") == 0 || strcasecmp(opCode.c_str(), "resb") == 0)
                {
                    tempObjectCode = "";
                    tr = "";
                    objCode = "";
                    res = true;
                }
                else
                {
                    tr = objCode;
                    objCode = "";
                    startAddress = prev;
                }
            }
            else
            {
                tr += objCode;
            }
        }
        // write in listing file

        if (strcasecmp(opCode.c_str(), "end") != 0 ){
            passTwoInstruction* instruction = new passTwoInstruction(lineAddress,objCode1,label,opCode,operand,comment);
            writePassTwoLine(instruction);
        }else{
            if (!isEnd){
                passTwoInstruction* instruction = new passTwoInstruction(lineAddress,objCode1,label,opCode,operand,comment);
                writePassTwoLine(instruction);
            }
        }
        if (strcasecmp(opCode.c_str(), "end") == 0){
            isEnd = true;
        }
        objCode1 = "";
        if (isError)
        {
            ThereIsError = true;
            write(e);
        }
        canRead = ReadLineFromIntFile();
    }

    if (tr != "\0")
    {
        // write last record
        prev = current;
        std::stringstream ss;
        ss << std::hex << lineAddress;
        ss >> current;
        int lengthRecord = prev - startAddress;
        char buffer [33];

        string st = itoa(startAddress,buffer,16);
        if (st.length() < 6)
        {
            int x = 6 - st.length();
            for (int i = 0; i < x; i++)
            {
                st = "0" + st;
            }
        }

        string length = itoa(lengthRecord,buffer,16);
        if (length.length() < 2)
        {

            int x = 2 - length.length();
            for (int i = 0; i < x; i++)
            {
                length = "0" + length;
            }

        }
       stringToUpper(st);
        stringToUpper(length);
        stringToUpper(tempObjectCode);
        stringToUpper(tr);
        string textRecord = "T^" + st + "^" + length + tempObjectCode + tr;
        writeObjectFile(textRecord);

    }
    for (int i = 0; i < numOfForm4; i++ )
    {
        string f = format4[i];
        if (f.length() < 6)
        {
            int x = 6 - f.length();
            for (int i = 0; i < x; i++)
            {
                f = "0" + f;
            }
        }
        string modified = "M^" + f + "^05";
        writeObjectFile(modified);
    }
    // write end record
    string text = "E^" + startString;
    writeObjectFile(text);
    if (!ThereIsError)
    {
        write("Successful assembly");
    }
    else
    {
        write("Incomplete assembly");
    }
}
bool ReadLine()
{
    cout<<"\n=========== Reading from Source File ===========\n"<<endl;
    if(!getline (inFile,line) )
    {
        cout << "cannot read, file ended" << '\n';
        return false;
    }
    cout<<line<<endl;
    line = line + "  ";

    bool flag=false;
    string results[4];
    unsigned int i =0;

    for(unsigned int j=0; j<5; j++)
    {
        flag=false;
        while( i<line.length()  )
        {
            if(j==1)
            {
                if(isOPcode(results[0])|| isDirective(results[0]))
                {
                    results[1]=results[0];
                    results[0]="";
                    break;
                }
            }
            if(line[i]=='\t')
            {
                line[i]=' ';
            }
            if(j==4)
            {
                if(flag==false)
                {
                    results[j-1]=results[j-1]+line[i-1];
                }
                results[j-1]=results[j-1]+line[i];
                i++;
                flag=true;
                continue;
            }
            if(j==0 && flag==false && line[i]=='.')
            {
                j=3;
                i++;
                i++;
                break;
            }
            if(line[i]==' ' )
            {
                if(flag==true)
                {
                    i++;
                    break;
                }
                else
                {
                    i++;
                    continue;
                }

            }
            else
            {
                if(j==3)
                {
                    if(line[i]!=',' && line[i]!='+' &&line[i]!='-' &&line[i]!='*' &&line[i]!='/' && line[i]!=')')
                    {
                        i++;
                        break;
                    }
                }
                else
                {
                    // j= 0 or 1 or 2
                    flag=true;
                }
            }

            if(j==3) // then there is a connecting operator
            {
                if( line[i]!=')')
                {
//                    cout<<" reduced j "<<j<<" "<<line[i]<<endl;
                    j=j-1;

                    flag=false;
                }
                else
                {
//                    cout<<" reduced j "<<j<<" "<<line[i]<<endl;
                    j=j-1;
                    flag=true;
                }

            }
            if(j==2)
            {
                if(line[i]==',' || line[i]=='+' || line[i]=='-' || line[i]=='*' || line[i]=='/' || line[i]=='(')
                {
                    flag=false;
                }

                if(line[i]=='\'')
                {
                    results[j]=results[j]+line[i];
                    i++;
                    while(line[i]!='\'')
                    {
                        results[j]=results[j]+line[i];
                        i++;
                    }
                    results[j]=results[j]+line[i];
                    i++;
                    j=3;
                    flag=false;
                    continue;
                }
            }
            results[j]=results[j]+line[i];
            i++;
        }

    }



//    if(isOPcode(results[0]))
//    {
//        results[3]=results[2];
//        results[2]=results[1];
//        results[1]=results[0];
//        results[0]="";
//    }
//
    if(strcasecmp(results[3].c_str(), ""))
    {
//        cout<<"comment not empty"<<endl;
        unsigned int place = line.find(results[3]);
        results[3]="";
        while(place<line.length()  )
        {
            results[3]=results[3]+line[place];
            place++;
        }
    }

//    cout<<"results of 0 ="<<results[0]<<endl;
//    if(strcasecmp(results[0].c_str(), "start")==0 ||strcasecmp(results[0].c_str(), "end")==0  )
//    {
//        cout<<"here start"<<endl;
//        if(!isOPcode(results[1]))
//        {
//
//            results[3]=results[2];
//            results[2]=results[1];
//            results[1]=results[0];
//            results[0]="";
//        }
//
//    }


    if(strcasecmp(results[1].c_str(), "rsub")==0 ||strcasecmp(results[1].c_str(), "LTORG")==0 )
    {
//        cout<<"rsub detected"<<endl;
        results[3]=results[2];
        results[2]="";

        if(strcasecmp(results[3].c_str(), ""))
        {
//            cout<<"comment not empty"<<endl;
            unsigned int place = line.find(results[3]);
            results[3]="";
            while(place<line.length()  )
            {
                results[3]=results[3]+line[place];
                place++;
            }
        }
    }


    label=results[0];
    opCode=results[1];
    operand=results[2];
    comment=results[3];

    if(strcasecmp(comment.c_str(),"")!=0)
    {
//    cout<<">>"<<comment<<"<<"<<endl;
        comment = comment.substr (0,comment.length()-2);
//cout<<">>"<<comment<<"<<"<<endl;
    }

    cout << "label="<<label<<", opCode="<<opCode<<", operand="<<operand<<", comment="<<comment << '\n';
    return true;
}



bool isOPcode(string value)
{
    locale loc;
    string oldValue="";
    if(value[0]=='+')
    {
        oldValue=value.substr (1);

    }
    else
    {
        oldValue=value;
    }
    string newValue="";
    for(unsigned int j=0; j<oldValue.length(); j++)
    {
        newValue=newValue+tolower(oldValue[j],loc);
    }
    bool found = false;
    for(int i=0 ; i<59; i++)
    {
        if(!strcmp(opCodes[i].c_str(),newValue.c_str()))
        {
            found = true;
            indexOfOpCode=i;
            break;
        }
    }
    return found;
}

bool ReadLineFromIntFile()
{
    cout<<"\n=========== Reading from Intermediate File ===========\n"<<endl;
    string myline;

    if(!inFile2.is_open())
    {
        inFile2.open( "INTFILE.txt");
        if( !inFile2 )
        {
            cout << "Unable to open file: " << "INTFILE.txt" << endl;
            return false;
        }
    }
    if(!getline (inFile2,myline) )
    {
        cout << "cannot read, file ended" << '\n';
        return false;
    }
    cout<<myline<<endl;


    bool flag=false;
    string results[2];
    unsigned int i =0;

    for(unsigned int j=0; j<2; j++)
    {
        flag=false;
        while( i<myline.length()  )
        {
            if(myline[i]=='\t')
            {
                myline[i]=' ';
            }


            if(myline[i]==' ' )
            {
                if(flag==true)
                {
                    i++;
                    break;
                }
                else
                {
                    i++;
                    continue;
                }

            }
            else
            {
                flag=true;
            }
            results[j]=results[j]+myline[i];
            i++;
        }

    }
    string newline = "";
    while( i<myline.length()  )
    {
        newline=newline+myline[i];
        i++;
    }

    lineType=results[0];
    lineAddress=results[1];

    cout << "lineType="<<lineType<<", lineAddress="<<lineAddress<<", "<<'\n';
    if(ReadLineInternally(newline)==false)
    {
        return false;
    }
    return true;
}

bool ReadLineInternally(string line)
{

//    cout<<line<<endl;

    line = line +"  ";

    bool flag=false;
    string results[4];
    unsigned int i =0;

    for(unsigned int j=0; j<5; j++)
    {
        flag=false;
        while( i<line.length()  )
        {
            if(j==1)
            {
                if(isOPcode(results[0]) || isDirective(results[0]))
                {
                    results[1]=results[0];
                    results[0]="";
                    break;
                }
            }
            if(line[i]=='\t')
            {
                line[i]=' ';
            }
            if(j==4)
            {
                if(flag==false)
                {
                    results[j-1]=results[j-1]+line[i-1];
                }
                results[j-1]=results[j-1]+line[i];
                i++;
                flag=true;
                continue;
            }
            if(j==0 && flag==false && line[i]=='.')
            {
                j=3;
                i++;
                i++;
                break;
            }
            if(line[i]==' ' )
            {
                if(flag==true)
                {
                    i++;
                    break;
                }
                else
                {
                    i++;
                    continue;
                }

            }
            else
            {
                if(j==3)
                {
                    if(line[i]!=',' && line[i]!='+' &&line[i]!='-' &&line[i]!='*' &&line[i]!='/' && line[i]!=')')
                    {
                        i++;
                        break;
                    }
                }
                else
                {
                    // j= 0 or 1 or 2
                    flag=true;
                }
            }

            if(j==3) // then there is a connecting operator
            {
                if( line[i]!=')')
                {
//                    cout<<" reduced j "<<j<<" "<<line[i]<<endl;
                    j=j-1;

                    flag=false;
                }
                else
                {
//                    cout<<" reduced j "<<j<<" "<<line[i]<<endl;
                    j=j-1;
                    flag=true;
                }

            }
            if(j==2)
            {
                if(line[i]==',' || line[i]=='+' || line[i]=='-' || line[i]=='*' || line[i]=='/' || line[i]=='(')
                {
                    flag=false;
                }

                if(line[i]=='\'')
                {
                    results[j]=results[j]+line[i];
                    i++;
                    while(line[i]!='\'')
                    {
                        results[j]=results[j]+line[i];
                        i++;
                    }
                    results[j]=results[j]+line[i];
                    i++;
                    j=3;
                    flag=false;
                    continue;
                }
            }
            results[j]=results[j]+line[i];
            i++;
        }

    }

    if(strcasecmp(results[3].c_str(), ""))
    {
//        cout<<"comment not empty"<<endl;
        unsigned int place = line.find(results[3]);
        results[3]="";
        while(place<line.length()  )
        {
            results[3]=results[3]+line[place];
            place++;
        }
    }


    if(strcasecmp(results[1].c_str(), "rsub")==0 ||strcasecmp(results[1].c_str(), "LTORG")==0 )
    {
//        cout<<"rsub detected"<<endl;
        results[3]=results[2];
        results[2]="";

        if(strcasecmp(results[3].c_str(), ""))
        {
//            cout<<"comment not empty"<<endl;
            unsigned int place = line.find(results[3]);
            results[3]="";
            while(place<line.length()  )
            {
                results[3]=results[3]+line[place];
                place++;
            }
        }
    }


    label=results[0];
    opCode=results[1];
    operand=results[2];
    comment=results[3];

    if(strcasecmp(comment.c_str(),"")!=0)
    {
//    cout<<">>"<<comment<<"<<"<<endl;
        comment = comment.substr (0,comment.length()-2);
//cout<<">>"<<comment<<"<<"<<endl;
    }
    cout << "label="<<label<<", opCode="<<opCode<<", operand="<<operand<<", comment="<<comment << '\n';
    return true;
}


bool isDirective(string word)
{

    string directives[]= {"start","end","equ","org","byte","word","resb","resw","ltorg"};
    for (int i=0; i<9 ; i++ )
    {

        if(strcasecmp(word.c_str(), directives[i].c_str())==0 )
        {
//            cout<<"true directive word="<<word<<"."<<endl;
            return true;
        }

    }

//    cout<<"false directive word="<<word<<"."<<endl;
    return false;
}

string generateObjCode(string addressOfThisLine, string opCode, string operand)
{
    string fullObjcode="";

    isOPcode(opCode);
    int opCodeIndex = indexOfOpCode;
    string instructionCode = objectCodes[opCodeIndex];
    int format = opCodesFormat[opCodeIndex];
    if(format==1)
    {
        stringToUpper(instructionCode);
        return instructionCode;
    }
    else if (format==2)
    {
        // 0  1  2   3   4   5   6   8   9
        // A  X  L   B   S   T   F   PC  SW

        if(operand.length() ==1 || operand.length() ==2 )
        {
            // one register
            int regNum = getRregisterNumber(operand);
            string hexRegNum = decimalToHex(regNum);



            fullObjcode = instructionCode + hexRegNum + "0000";


        }
        else
        {
            // 2 registers
            string firstReg="";
            string secondReg="";
            unsigned int k =0;
            while(k<operand.length())
            {
                if(operand[k]==',')
                {
                    k++;
                    break;
                }
                firstReg=firstReg+operand[k];
                k++;
            }
            while(k<operand.length())
            {
                secondReg=secondReg+operand[k];
                k++;
            }
            if(!(isRegister(firstReg))|| !(isRegister(secondReg)))
            {
                // throw exception
                errorStatement= " Wrong Register Name ";
                return "";
            }


            int regNum1 = getRregisterNumber(firstReg);
            string hexRegNum1 = decimalToHex(regNum1);



            int regNum2 = getRregisterNumber(secondReg);
            string hexRegNum2 = decimalToHex(regNum2);



            fullObjcode = instructionCode + hexRegNum1 + hexRegNum2;
        }

        stringToUpper(fullObjcode);
        return fullObjcode;
    }

// format 3 or 4
// n(indirect@) i(immediate#) x(indexed,X) b(base) p(programCounter) e(format3)

// n i
    bool immedOrIndirect =false;
    int middle_2_bits =3;
    // x b p e
    int xbpe = 2;

    if(opCode[0]=='+')
    {

        if(operand[0]=='#')
        {
            immedOrIndirect=true;
            middle_2_bits--;
            middle_2_bits--;
            xbpe=1;
        }
        if(operand[0]=='@')
        {
            immedOrIndirect=true;
            middle_2_bits--;
        }

        fullObjcode = decimalToHex(hexToDecimal(instructionCode)+middle_2_bits);
        xbpe = 1;
        if((operand[operand.length()-1]=='x' ||operand[operand.length()-1]=='X' )&&operand[operand.length()-2]==',' )
        {
            if (immedOrIndirect==true)
            {
                errorStatement="cannot be immediate or Indirect and also indexed";
                return "";
            }
            if( operand[0]=='=')
            {
                errorStatement="cannot be Literal and also indexed";
                return "";
            }

            xbpe= xbpe + 8;
        }
        if(strcasecmp(opCode.c_str(),"rsub")==0)
        {
            xbpe=1;
        }
        fullObjcode=fullObjcode+decimalToHex(xbpe);


        // need address of operand and make sure it is 5 chars ##FALSE##
        string address="";
        if(strcasecmp(opCode.c_str(),"rsub")!=0 && operand[0]!='=' )
        {
            address= validNotation(operand, 0,addressOfThisLine);
        }
        else if(strcasecmp(opCode.c_str(),"rsub")==0)
        {
            address="00000";
        }
        else if( operand[0]=='=')
        {
            // get literal address from mina
            address = getAddressFromLiterals(operand);
        }


        if(strcasecmp(address.c_str(),"##FALSE##")==0)
        {
            // throw exception
//            errorStatement = " Wrong expression in operand";
            return "";
        }

        int temp = hexToDecimal(address);
        if(temp >= 1048575 )
        {
            // throw exception
            errorStatement = " address of operand out of range";
            return "";
        }

        while(address.length()<5)
        {
            address="0"+address;
        }

        fullObjcode=fullObjcode+address;

        while(fullObjcode.length()<8)
        {
            fullObjcode="0"+fullObjcode;
        }

    }
    else
    {
        bool immediate =false;
        bool rsub=false;
        immedOrIndirect = false;
        if(operand[0]=='#')
        {
            immediate=true;
            immedOrIndirect=true;
            middle_2_bits--;
            middle_2_bits--;
            xbpe=0;
        }
        if(operand[0]=='@')
        {
            immedOrIndirect=true;
            middle_2_bits--;
        }
        fullObjcode = decimalToHex(hexToDecimal(instructionCode)+middle_2_bits);


        if((operand[operand.length()-1]=='x' ||operand[operand.length()-1]=='X' )&&operand[operand.length()-2]==',' )
        {
            if (immedOrIndirect==true)
            {
                errorStatement="cannot be immediate or Indirect and also indexed";
                return "";
            }
            if( operand[0]=='=')
            {
                errorStatement="cannot be Literal and also indexed";
                return "";
            }
            xbpe= xbpe + 8;
        }
        if(strcasecmp(opCode.c_str(),"rsub")==0)
        {
            xbpe=0;
        }

        fullObjcode=fullObjcode+decimalToHex(xbpe);

// calculate displacement from start of program and make sure it is in 3 chars
        string address="";
        if(strcasecmp(opCode.c_str(),"rsub")!=0 && operand[0]!='=')
        {
            address= validNotation( operand, 0,addressOfThisLine);
        }
        else if(strcasecmp(opCode.c_str(),"rsub")==0)
        {
            rsub=true;
            address="000";
        }
        else if(operand[0]=='=')
        {
            // get address for literal from mina
            address = getAddressFromLiterals(operand);
        }
        if(strcasecmp(address.c_str(),"##FALSE##")==0)
        {
            // throw exception
//            errorStatement = " Wrong expression in operand";
            return "";
        }

        /////////
        string displacement="";

        int addresofline = hexToDecimal(addressOfThisLine);
        int targetaddress = hexToDecimal(address);
        int disp;
        if(immediate==false && rsub == false)
        {
            disp = targetaddress - (addresofline+3);
        }
        else if(rsub==true)
        {
            disp=0;
        }
        else
        {
            // immediate
            disp = targetaddress;

        }

        if(disp > 2047 || disp< -2048)
        {

            // throw exception
            errorStatement = " displacement of operand out of range";
            return "";
        }
        displacement = decimalToHex(disp);
        if(disp <0)
            cout << "negative  value   ="<< displacement;
        if(displacement.length()>=3)
        {
            displacement=displacement.substr(displacement.length()-3,3);
        }

        while(displacement.length()<3)
        {
            displacement="0"+displacement;
        }


        fullObjcode=fullObjcode+displacement;
    }

    cout<<"full object code ="<<fullObjcode<<endl;
    return fullObjcode;

}

int hexToDecimal(string hex_value)
{
//Hex to decimal :-
    int decimal_value;
    stringstream ss;
    ss  << hex_value ; // std::string hex_value
    ss >> hex >> decimal_value ; //int decimal_value

//std::cout << decimal_value ;

    return decimal_value ;
}

string decimalToHex(int decimal_value)
{
// Decimal to hex :-

    stringstream ss;
    ss<< hex << decimal_value; // int decimal_value
    string res ( ss.str() );

//cout << res;
    return res;
}

int getRregisterNumber(string reg)
{
    for (int i=0 ; i<9 ; i++)
    {
        if(strcasecmp(reg.c_str(), registers[i].c_str())==0)
        {
            if(i>6)
            {
                i++;
            }
            return i;
        }
    }
    return -1;


}

void stringToUpper(string &s)
{
    for(unsigned int l = 0; l < s.length(); l++)
    {
        s[l] = toupper(s[l]);
    }
}
