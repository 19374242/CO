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
    //Attention! The mips code to test shouldn't have any space '\t' and other special symbol!!!
    //or the int file will be wrong 
    //Such as la can't be translate to lui and ori
    int offset_of_data=0;
    char buf_rds[ASSEMBLER_BUF_SIZE];
    uint32_t line_rds=0;
    char *name=NULL;
    long tmpl=0;
    char *token=NULL;
    while(fgets(buf_rds,ASSEMBLER_BUF_SIZE,input))
    {
        skip_comment(buf_rds);
        ++line_rds;
        //This line is "\n":return !
        if(buf_rds[0]=='\n')
        {
            return offset_of_data;
        }
        token = strtok(buf_rds, ASSEMBLER_IGNORE_CHARS);
        if(token==NULL)
        {
        	continue;
		}
        if(strcmp(token,".data")==0)
        {
            continue;
        } 
        name=create_copy_of_str(token);
        //If Label doesn't end by ':'    error!
        if(name[strlen(name)-1]!=':')
        {
            raise_label_error(line_rds,name);
            return -1;
        }
        name[strlen(name)-1]='\0';
        token[strlen(token)-1]='\0';
        if(!is_valid_label(name))
        {
            raise_label_error(line_rds,name);
            return -1;
        }
        name[0]='%';
        strcpy(name+1,token);
        //read ".space"
        token=strtok(NULL, ASSEMBLER_IGNORE_CHARS);
        //read the number
        token=strtok(NULL, ASSEMBLER_IGNORE_CHARS);
        add_to_table(symtbl,name,offset_of_data);
        if((translate_num(&tmpl,token,0,0x7fffffff))==0)    
        {
            if(tmpl%4==0)
            offset_of_data+=tmpl;
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
    return offset_of_data;
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
    //Reapeat
    if(table->mode==SYMTBL_UNIQUE_NAME&&get_addr_for_symbol(table,name)!=-1)
    {
        name_already_exists(name);
        return -1;
    }
    //Full
    if(table->len==table->cap)
    {
        Symbol *ptr=NULL;
        if((ptr=(Symbol *)realloc(table->tbl,table->cap*SCALING_FACTOR*sizeof(Symbol)))==NULL)
        {
            allocation_failed();
            return -1;
        }
        table->cap*=SCALING_FACTOR;
        table->tbl=ptr;
    }
    //Insert
    char *in_name=create_copy_of_str(name);
    (table->tbl)[table->len].name=in_name;
    (table->tbl)[table->len].addr=addr;
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
    int matchcode_lui=0;
    char ignore_reg[2]="@";
    char *token=NULL;
    char *backup_label=NULL;
    backup_label=create_copy_of_str(args[1]);
    token=strtok(args[1],ignore_reg);
    int op_int=(int)opcode;
    matchcode_lui=(op_int<<26);
    int rt=0;
    rt=translate_reg(args[0]);
    if(rt==-1)
    {
        //error
        return -1;
    }
    long imm=0;
    int flag=0;
    if(translate_num(&imm,args[1],0,0x7fffffff)==-1)
    {
        imm=0;
        flag=1;
    }
    matchcode_lui|=(rt<<16);
    matchcode_lui|=imm;
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
    write_inst_hex(output,matchcode_lui);
    return 0;   
}

