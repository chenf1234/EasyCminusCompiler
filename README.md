# Introduction

**基于X86平台的简单CMinus语言的编译器**

该编译器基于X86架构，使用了IA-32指令集，在汇编阶段借助了GNU as汇编器，最后的链接阶段借助了glibc库和GNU ld链接器。整个代码我是在64位windows下的vscode中编写完成的，使用的是GBK编码格式，能够编译运行。经测试，代码可以在32位的Ubuntu16.04中编译运行并生成测试源程序的可执行代码。

## CMnius语言介绍

CMinus语言类似C语言，只是减少了很多C语言的特性

在CMinus语言中，数据类型支持整型、整型数组，void

变量的定义和初始化要分开，不能定义的时候进行初始化；并且，所有的变量定义都得放在一个作用域的开始处

支持函数调用，if条件判断语句，while循环语句

详细介绍请参见**doc**文件夹中的**C-语言的详细定义.pdf**

# 文件夹结构

## doc文件夹

编译流程图：FlowGraph.png

CMinus语言的详细定义：**C-语言的详细定义.pdf**

CMinus语言词法分析：DFA.png

详细开发文档：DevelopingDocument.md

## src文件夹

**Code文件夹**存放编译器源码,包括：

​	关键数据结构：Globals.h

​	词法分析：Lexical_Analysis.h，Lexical_Analysis.cpp

​	语法分析：Syntax_Analysis.h，Syntax_Analysis.cpp

​	语义分析：Semantic_Analysis.h，Semantic_Analysis.cpp

​	中间代码生成：Mid_Code.h，Mid_Code.cpp

​	汇编代码生成：Assembly_code.h，Assembly_code.cpp

​	主函数：main.cpp

**OutputFile文件夹**存放编译过程中的中间输出文件，包括：

​	词法分析记号流：Tokens.txt

​	抽象语法树：Tree.txt

​	符号表：symbol_table.txt

​	中间表示--三地址码：Mid_code.txt

**Release文件夹**存放编译器可执行程序以及测试文件的汇编代码和测试文件的可执行程序，包括：

​	编译器可执行程序：CmCompiler

​	测试文件的汇编代码：main.s

​	测试文件的可执行程序：final

**TestFile文件夹**包括了CMinus语言的测试样例

# How to run

cd Code  进入源码所在文件夹

make  编译

cd ../Release/  进入编译器可执行程序文件夹

./CmCompiler   ../TestFile/AllTest.cm     使用Cminus编译器编译CMinus测试文件，生成测试文件的可执行程序

./final   运行
