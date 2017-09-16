#include <string.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

// Remover quebra de linha do fgets
void remove_line_break(char *string) {
  size_t size;
  size = strlen(string);
  if (string[size - 1] == '\n'){
    string[--size] = 0;
  }
}

// Limpar o buffer do teclado
void clean_buffer(){
  int ch;
  do {
    ch = fgetc(stdin);
  } while (ch != EOF && ch != '\n');
}

// Cabeçalho da camada de aplicação do host A.
void application_header() {
  printf("\n*******************\n");
  printf("Camada de aplicação\n");
  printf("*******************\n");
}

// Cabeçalho da camada de transporte do host A.
void transport_header() {
  printf("\n********************\n");
  printf("Camada de transporte\n");
  printf("********************\n");
}
