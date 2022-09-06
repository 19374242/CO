#include<iostream>
#include<cstdlib>
#include<algorithm>
#include<cstring>
#include<string>
#include<cstdlib>

using namespace std;

const int file_num = 100;
const int label_num = 100;
const int func_num = 100;


// 100个源文件，每个文件的.data段创建100个符号，.text段创建100个符号、200个引用

int main() {
	const string folder_path = "C:/Users/jin/Desktop/计算机硬件基础/选做实验一代码/test/big test 1/";
	const string file_name = "ok";
	
	string bat = folder_path + string("action.bat");
	FILE *file = fopen(bat.data(), "w");
	freopen(bat.data(), "w", stdout);
	for (int i=0; i<file_num; i++) {
		printf("assembler ok%d.src inter ok%d.out\n", i,i);
	}
	printf("linker ");
	for (int i=0; i<file_num; i++) {
		printf("ok%d.out ",i);
	} 
	puts("ok.o");
	puts("pause");
	fclose(file);
	
	for (int i=0; i<file_num; i++) {
		string path = (folder_path+file_name+to_string(i)+string(".src"));
		FILE *file = fopen(path.data(), "w");
		freopen(path.data(), "w", stdout);
		puts(".data");
		for (int j=0; j<label_num; j++) {
			printf("label%d:  ",i*label_num+j);
			printf(".space  ");
			printf("%d\n",(rand()%30+2)*4);
		}
		puts("");
		puts(".text");
		for (int j=0; j<label_num; j++) {
			printf("la $%d, label%d\n", rand()%32, rand()*rand()%(file_num*label_num));
			printf("j func%d\n", rand()%(file_num*func_num));
			printf("func%d:\n", i*func_num+j);
		}
		fclose(file);
	}
//	cout << to_string(123);
} 

