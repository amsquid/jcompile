#define TOKEN_LEN 1024

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

#include "instructions.h" 

char* boilerplate_asm = "SECTION .data\n\
  fmt_num: db \"%d\", 0\n\
  fmt_ch: db \"%c\", 0\n\
SECTION .bss\n\
  array resb 512\n\
SECTION .text\n\
  global main\n\
  extern printf\n\
main:\n\
  push rbp\n\
  mov rsi, 0\n\
  mov rax, array\n\
";

char* left = "\
  sub rsi, 1\n\
";

char* right = "\
  add rsi, 1\n\
";

char* add = "\
  add byte [rax + rsi], 1\n\
";

char* sub = "\
  sub byte[rax + rsi], 1\n\
";

char* print_num = "\
  mov rdi, fmt_num\n\
  movzx esi, byte[rax + rsi]\n\
  mov rax, 0\n\
  call printf\n\
  mov rax, array\n\
";

char* print_ch = "\
  mov rdi, fmt_ch\n\
  movzx esi, byte[rax + rsi]\n\
  mov rax, 0\n\
  call printf\n\
  mov rax, array\n\
";

char* exit_asm = "\
  pop rbp\n\
  mov rax, 0\n\
  ret\n\
";

char* out;

int tokens[TOKEN_LEN];

int lex(char in[TOKEN_LEN]) {
  char c = in[0];
  int i = 0;
  while(c != '\0' && i < TOKEN_LEN) {
    c = in[i];
    int instruction = -1;

    switch(c) {
    case '+':
      instruction = ADD;
      break;
    case '-':
      instruction = SUB;
      break;
    case '>':
      instruction = RIGHT;
      break;
    case '<':
      instruction = LEFT;
      break;
    case '@':
      instruction = PRINT_CHAR;
      break;
    case '#':
      instruction = PRINT_NUM;
      break;
    }

    i++;
    if(instruction == -1) continue;
    tokens[i-1] = instruction;
  }

  tokens[i] = INSTRUCTIONS_END;
  return 0;
}

int addInst(char* out, char* inst, int currSize) {
  int newSize = currSize + (strlen(inst) * sizeof(char));
  out = realloc(out, newSize);
  strcat(out, inst);
  return newSize;
}

int main(int argc, char *argv[]) {
  if(argc == 1) {
    printf("Please input a .txc code file\n");
    return -1;
  }

  int ret;

  FILE *inFile = fopen(argv[1], "r");
  char contents[TOKEN_LEN];
  fgets(contents, TOKEN_LEN, inFile);
  fclose(inFile);
  lex(contents);

  int size = strlen(boilerplate_asm) * sizeof(char);
  out = malloc(size);
  strcpy(out, boilerplate_asm);

  for(int i = 0; i < TOKEN_LEN; i++) {
    int token = tokens[i];
    int end = 0;
    switch(token) {
      case ADD:
        size = addInst(out, add, size);
        break;
      case SUB:
        size = addInst(out, sub, size);
        break;
      case LEFT:
        size = addInst(out, left, size);
        break;
      case RIGHT:
        size = addInst(out, right, size);
        break;
      case PRINT_CHAR:
        size = addInst(out, print_ch, size);
        break;
      case PRINT_NUM:
        size = addInst(out, print_num, size);
        break;
      case INSTRUCTIONS_END:
        end = 1;
        break;
    }

    if(end == 1) break;
  }

  size = addInst(out, add, size);

  size += strlen(exit_asm) * sizeof(char);
  out = realloc(out, size);
  strcat(out, exit_asm);

  FILE *outFile = fopen("out.asm", "w");
  if(outFile == NULL) {
    printf("Error opening out file\n");
    return -1;
  }

  fprintf(outFile, "%s", out);
  fclose(outFile);

  ret = system("nasm -f elf64 out.asm -o out.o");
  if(ret != 0) {
    printf("Failed to compile assembly code using NASM\n");
    return -1;
  }
  ret = system("gcc out.o -no-pie -o out");
  if(ret != 0) {
    printf("Failed to create executable file using GCC\n");
    return -1;
  }
  system("rm out.o && rm out.asm");



  return 0;
}
