/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * my_linker_utils.c
 * Linker Submission
 *****************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lib/tables.h"
#include "linker-src/linker_utils.h"

/*
 * Builds the symbol table and relocation data for a single file.
 * Read the .data, .text, .symbol, .relocation segments in that order.
 * The size of the .data and .text segments are read and saved in the
 * relocation table of the current file. For the .symbol and .relocation
 * segments, save the symbols in them in the corresponding locations.
 *
 * Return value:
 * 0 if no errors, -1 if error.
 *
 * Arguments:
 * input:            file pointer.
 * symtbl:           symbol table.
 * reldt:            pointer to a Relocdata struct.
 * base_text_offset: base text offset.
 * base_data_offset: base data offset.
 */
int fill_data(FILE *input, SymbolTable *symtbl, RelocData *reldt, uint32_t base_text_offset, uint32_t base_data_offset) {
    char buf[LINKER_BUF_SIZE];
    char *token=NULL;
    while(fgets(buf,LINKER_BUF_SIZE,input))
    {
        token=strtok(buf,LINKER_IGNORE_CHARS);
        //Attention no space '\t' or '\n'  should turn up!
        //Read '\t' may cause error when reading next!
        if(strcmp(token,".data")==0)
        {
            reldt->data_size=calc_data_size(input);
        }
        if(strcmp(token,".text")==0)
        {
            reldt->text_size=calc_text_size(input);
        }
        if(strcmp(token,".symbol")==0)
        {
            if(add_to_symbol_table(input,symtbl,base_text_offset,base_data_offset)==-1)
            return -1;
        }
        if(strcmp(token,".relocation")==0)
        {
            if(add_to_symbol_table(input,reldt->table,0,0)==-1)
            return -1;
        }
    }
    return 0;
}

/*
 * Detect whether the given instruction with offset OFFSET needs relocation.
 *
 * Return value:
 * 1 if the instruction needs relocation, 0 otherwise.
 *
 * Arguments:
 * offset: Address offset of the instruction in the current file.
 * reltbl: relocation table corresponding to the current file.
 */
int inst_needs_relocation(SymbolTable *reltbl, uint32_t offset) {
    if(get_symbol_for_addr(reltbl,offset)!=NULL)
    {
        return 1;
    }
    return 0;
}
