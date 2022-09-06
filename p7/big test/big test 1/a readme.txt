本测试只是暴力堆标签数，有其它好的测试思路建议集思广益

共100个汇编文件，每个文件.data段创建100个标签，.text段创建100个标签、100个lui引用、100个j引用

把assembler.exe和linker.exe复制到目录下，双击action.bat运行

后缀.src的都是源文件，汇编、链接的结果为ok.o

ok example.o为我生成的结果，可用来比对

generate all.cpp是生成测试文件的程序