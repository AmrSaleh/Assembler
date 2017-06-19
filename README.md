# Assembler
Assembling SIC/XE code and generating the Object file to be used by the loader program

# Team members by alphabetical order
+ Ahmed Abd El-Wahed El-Khatib
+ Amr Saleh Mohamed Aly
+ Mina Makram Louis

# Specifications
1. The assembler executes by entering assemble <source-file-name>
2. The source file for the main program for this phase is named assemble.cpp
3. The output of the assembler includes:
  * Object-code file
  * A report at the end of pass2. Pass1 and Pass2 errors are included as part of the assembler report, exhibiting both the      offending line of source code and the error.
4. The assembler supports:
  * EQU and ORG statements.
  * Simple expression evaluation. A simple expression includes simple (A<op> B) operand arithmetic, where <op> is one of +,-,*,/ and no spacessurround the operation, eg. A+B.
  
# Design
![assemblerdesign](https://user-images.githubusercontent.com/5616594/27295759-589b3476-551e-11e7-95fd-5f6e44ba9648.PNG)

# Main Assembling Algorithm
![assemblingalgorithm1](https://user-images.githubusercontent.com/5616594/27295998-582bd1e8-551f-11e7-91cd-2f50cc2dfb6e.JPG)
![assemblingalgorithm2](https://user-images.githubusercontent.com/5616594/27296006-5d85312a-551f-11e7-8044-937b1ea4ebcf.JPG)

# Assumptions
1.	The code file should be placed in the same folder of the program.
2.	The source file can be free formatted.
3.	Single quotation(') is only allowed in operand field
4.	Opcodes and directives and also the word “!current!” are key wordsthat cannot be used as labels.

# Sample Runs
## Reference samples
![ref1](https://user-images.githubusercontent.com/5616594/27296062-a3e34706-551f-11e7-8e9f-38bebbf359b7.JPG) 
![ref2](https://user-images.githubusercontent.com/5616594/27296059-a3d81b2e-551f-11e7-8ff5-21146e6fd04d.JPG)

## Program execution results
![sample1](https://user-images.githubusercontent.com/5616594/27296060-a3da676c-551f-11e7-8e0e-be8b852e7f0c.JPG)
![sample2](https://user-images.githubusercontent.com/5616594/27296058-a3cff200-551f-11e7-990f-18cbcbca2f60.JPG)
![listfile](https://user-images.githubusercontent.com/5616594/27296153-09dcbfe2-5520-11e7-91ad-109c22f3acef.jpg)
