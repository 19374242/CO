/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * my_assembler_utils.c
 * Assembler Submission
 *****************************************************************/
#include "my_assembler_utils.h"
#include "assembler-src/assembler_utils.h"
#include "lib/translate_utils.h"

#include <string.h>
#include <stdlib.h>

/*
 * This function reads .data symbol from INPUT and add them to the SYMTBL
 * Note that in order to distinguish in the symbol table whether a symbol
 * comes from the .data segment or the .text segment, we append a % to the
 * symbol name when adding the .data segment symbol. Since only underscores and
 * letters will appear in legal symbols, distinguishing them by adding % will
 * not cause a conflict between the new symbol and the symbol in the assembly file.
 *
 * Return value:
 * Return the number of bytes in the .data segment.
 */
int read_data_segment(FILE *input, SymbolTable *symtbl) {
	long int data_sz=0;
    char buf_rds[ASSEMBLER_BUF_SIZE];
    uint32_t line_rds=0;
    while(fgets(buf_rds,ASSEMBLER_BUF_SIZE,input))
    {
    	line_rds++;
        skip_comment(buf_rds);//去掉# 
        if(buf_rds[0]=='\0') continue;
        if(buf_rds[0]=='\n') return data_sz;//遇到换行符结束 
        char *token = strtok(buf_rds, ASSEMBLER_IGNORE_CHARS);
        if(strcmp(token,".data")==0) continue;
        char *name=create_copy_of_str(token);
        if(name[strlen(name)-1]!=':')//不是冒号 
        {
            raise_label_error(line_rds,name);
            return -1;
        }
        token[strlen(name)-1]='\0';
        name[strlen(name)-1]='\0';
        if(!is_valid_label(name))
        {
            raise_label_error(line_rds,name);
            return -1;
        }
        strcpy(name+1,token);
        name[0]='%';
        token=strtok(NULL, ASSEMBLER_IGNORE_CHARS);//read ".space"
        token=strtok(NULL, ASSEMBLER_IGNORE_CHARS);//read the number
        long tmpl=0;
        add_to_table(symtbl,name,data_sz);
        if((translate_num(&tmpl,token,0,0x7fffffff))==0)    
        {
            if(tmpl%4==0) data_sz=data_sz+tmpl;
            else
            {
                //align error
                addr_alignment_incorrect();
                return -1;
            }
        }
        else
            return -1;
    }
    return data_sz;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE.
 * ADDR is given as the byte offset from the first instruction. The SymbolTable
 * must be able to resize itself as more elements are added.
 *
 * Note that NAME may point to a temporary array, so it is not safe to simply
 * store the NAME pointer. You must store a copy of the given string.
 *
 * If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
 * return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists
 * in the table, you should call name_already_exists() and return -1. If memory
 * allocation fails, you should call allocation_failed().And alloction_failed()
 * will print error message and exit with error code 1.
 *
 * Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr) {
    if(get_addr_for_symbol(table,name)!=-1&&table->mode==SYMTBL_UNIQUE_NAME)//第一个查符号表里的地址 
    {
        name_already_exists(name);
        return -1;
    }
    if(addr%4!=0)
    {
        addr_alignment_incorrect();
        return -1;
    }
    if(table->len==table->cap)
    {
        Symbol *ptr=NULL;
        if((ptr=(Symbol *)realloc(table->tbl,table->cap*SCALING_FACTOR*sizeof(Symbol)))==NULL)
        {
            allocation_failed();
            return -1;
        }
        table->tbl=ptr;
        table->cap=SCALING_FACTOR*table->cap;
    }
    //Insert
    (table->tbl)[table->len]=(Symbol){create_copy_of_str(name),addr};
    (table->len)++;
    return 0;
}

/*
 * Convert lui instructions to machine code. Note that for the imm field of lui,
 * it may be an immediate number or a symbol and needs to be handled separately.
 * Output the instruction to the **OUTPUT**.(Consider using write_inst_hex()).
 * 
 * Return value:
 * 0 on success, -1 otherwise.
 * 
 * Arguments:
 * opcode:     op segment in MIPS machine code
 * args:       args[0] is the $rt register, and args[1] can be either an imm field or 
 *             a .data segment label. The other cases are illegal and need not be considered
 * num_args:   length of args array
 * addr:       Address offset of the current instruction in the file
 */
int write_lui(uint8_t opcode, FILE *output, char **args, size_t num_args, uint32_t addr, SymbolTable *reltbl) {
    char ignore_reg[2]="@";
    int lui_code=0;
    char *backup_label=create_copy_of_str(args[1]);
    char *token=strtok(args[1],ignore_reg);//去掉@ 
    int op_int=(int)opcode;
    long imm=0;
    int flag=0;
    lui_code=(op_int)<<26;//op来到最高位 
    int rt=translate_reg(args[0]);//目标寄存器 
    if(rt==-1) return -1;
    if(translate_num(&imm,args[1],0xffffff,0x7fffffff)==-1)//是不是立即数 
    {
    	flag=1;
        imm=0;
    }
    lui_code|=(rt<<16);
    lui_code|=imm;
    if(flag==1)
    {
        if(!is_valid_label(token))
        {
            //error
            raise_label_error(addr,backup_label);
            return -1;
        }
        add_to_table(reltbl,backup_label,addr);
    }
    write_inst_hex(output,lui_code);
    return 0; 
}

