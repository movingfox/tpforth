/*
 * program.c
 *
 *  Created on: Sep 29, 2024
 *      Author: Jan Atle Ramsli
 *
 */

// #define DEBUG

#include "program.h"
#include "logg.h"
#include "variable.h"
#include <string.h>

#include "smtok.h"

/**
Loop through the program, calling func for each p-code
*/
void program_loop(program_p prog, void (*func)(program_p, smtok_p *, ftask_p),
                  ftask_p task) {
  for (int i = 0; i < prog->npcp_array; i++) {
    func(prog, prog->pcp_array + i, task);
  }
}

void program_dump_cb(program_p prog, smtok_p *pcpp, ftask_p task) {
  var_p v;

  if (pcpp == task->pcp) {
    printf(" ==>");
  }
  if (pcpp >= prog->pcp_array + prog->npcp_array){
	printf("\nPROGRAM COUNTER BEYOND END!\n");
  }
  if (pcpp < prog->pcp_array){
	printf("\nPROGRAM COUNTER BEFORE START!\n");
  }

  smtok_p pcp = *pcpp;
  switch (pcp->jtidx) {
  case SMTOK_NUMBER:
    printf("NUM:%ld ", pcp->val.l);
    break;
  case SMTOK_BUILTIN:
    printf("BIN:%s ", pcp->name);
    break;
  case SMTOK_VARIABLE:
    v = variable_get(pcp->val.var_idx);
    printf("VAR:(%s=%s) ", pcp->name, vartype_string(v));
    break;
  case SMTOK_DICT_ENTRY:
    printf("PROG:%s ", pcp->name);
    break;
  case SMTOK_IF:
    printf("IF:%ld ", pcp->val.l);
    return;
  case SMTOK_ELSE:
    printf("ELSE:%ld ", pcp->val.l);
    break;
  case SMTOK_THEN:
    printf("THEN:%ld ", pcp->val.l);
    return;
  case SMTOK_LOOP_DO:
    printf("DO:%ld ", pcp->val.l);
    break;
  case SMTOK_LOOP_END:
    printf("LOOP:%ld ", pcp->val.l);
    break;
  case SMTOK_I:
    printf("I:%s ", pcp->name);
    break;
  case SMTOK_EXIT:
    printf("EXIT:%s ", pcp->name);
    break;
  case SMTOK_LAST:
    printf("END ");
    break;
  case SMTOK_DEFER:
    printf("QUOT:");
    break;
  case SMTOK_EXEC:
    printf("EXEC:");
    break;
  case SMTOK_SPAWN:
    printf("SPWN:");
    break;
  case SMTOK_STRING:
    printf("STR:%s", pcp->val.s);
    break;
  default:
    printf("  ? ");
    break;
  }
  printf("\n");
}

void program_dump(program_p progp, ftask_p task) {
  printf("----------------| %s |-----------------\n", progp->name);
  program_loop(progp, program_dump_cb, task);
  printf("----------------------------------------");
}

void program_delete(program_p prog) {
  prog->npcp_array = 0;
  free(prog->pcp_array);
  prog->pcp_array = 0;
  free(prog->name);
  prog->name = 0;
  free(prog);
}

program_p program_create(char *name) {
  logg("", name);
  program_p prog = malloc(sizeof(program_p));
  prog->name = malloc(strlen(name));
  strcpy(prog->name, name);
  prog->npcp_array = 0;
  prog->pcp_array = 0;
  return prog;
}

void program_add_smtok(program_p prog, smtok_p pcp) {
  logg(prog->name, pcp->name);
  prog->npcp_array++;
  if (!prog->pcp_array) {
    prog->pcp_array = malloc(sizeof(smtok_p) * (prog->npcp_array + 1));
  } else {
    prog->pcp_array =
        realloc(prog->pcp_array, sizeof(smtok_p) * (prog->npcp_array + 1));
  }
  prog->pcp_array[prog->npcp_array - 1] = pcp;
  prog->pcp_array[prog->npcp_array] = 0;
}

void program_add(program_p prog, char *src) {
  smtok_p pc = smtok_compile_word(src);
  if (!pc) {
    printf("\nprogram_add(%s) failed \n", src);
    return;
  }
  program_add_smtok(prog, pc);
}
