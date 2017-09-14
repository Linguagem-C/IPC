#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
// Headers para socket
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

// Constantes
#define ERROR  -1
#define TRUE  1
#define FALSE  0
#define PORTA 2000
#define NUMBER_OF_CONNECTION 1
#define MSG_LEN 4096

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

// Placa de rede local - 127.0.0.1 ou localhost
struct sockaddr_in local;
// Placa de rede do cliente remoto
struct sockaddr_in remoto;

int create_connection() {
  // Cria a comunicação (endpoint) via protocolo IPv4 da camada de redes
  // (dominio) e protocolo TCP da camada de transporte (tipo)
  int descritor = socket(AF_INET, SOCK_STREAM, 0);

  // Caso ocorra algum erro
  if (descritor == ERROR) {
    perror("socket");
    exit(1);
  } else {
    printf("Socket criado com sucesso!\n");
  }

  // Pega o protocolo IPv4
  local.sin_family = AF_INET;
  // Transforma a porta em números de rede
  local.sin_port = htons(PORTA);

  // Zera a memória da estrutura de dados local
  memset(local.sin_zero, 0x0, 8);

  // Atribui o endereço local ao socket descritor. Ou seja atribui um nome ao socket
  if(bind(descritor, (struct sockaddr*) &local, sizeof(local)) == ERROR) {
    perror("bind");
    exit(1);
  }

  // Colocar a porta em escuta para alguma conexão de um socket.
  listen(descritor, NUMBER_OF_CONNECTION);

  return descritor;
}

void accept_connection(int *descritor, int *cliente) {
    int len = sizeof(remoto); // Armazena o tamanho da placa de rede do cliente

    // Receber e aceitar as conexões dos clientes
    if((*cliente = accept(*descritor, (struct sockaddr*) &remoto, &len)) == ERROR) {
      perror("accept");
      exit(1);
    } else {
      printf("Conexão estabelecida com sucesso!\n");
    }
}

void welcome_message(int *cliente, char *message) {

    // Enviar a mensagem de bem vindo para o cliente
    strcpy(message, "Bem vindo ao servidor!\n\0");
    send(*cliente, message, strlen(message), 0);
}


int main() {

  pid_t process_son;        // Processo filho
  int descritor;            // Usado para escrever e ler dados no socket
  int cliente;              // Armazena o descritor do cliente.
  int string_len;           // Armazena o tamanho da string de buffer
  char message[MSG_LEN];    // Cria a string de buffer da mensagem

  // Cria um processo filho exatamente igual ao processo pai
  process_son = fork();

  if(process_son == 0) { // PROCESSO FILHO (CAMADA DE TRANSPORTE)

    transport_header();
    descritor = create_connection();
    accept_connection(&descritor, &cliente);
    welcome_message(&cliente, message);

    // Aguarda resposta do cliente
    printf("Aguardando resposta do cliente!\n");
    while(TRUE) {
      // Limpar o buffer
      memset(message, 0x0, MSG_LEN);

      // Receber a mensagem do cliente.
      if((string_len = recv(cliente, message, MSG_LEN, 0)) > 0) {
        // Insere o valor nulo no final e tira a quebra de linha
        message[string_len - 1] = '\0';

        // TODO: Aqui iremos retirar essa mensagem e criar a fila de comunicação
        // para a camada de aplicação imprimir
        printf("Mensagem recebida: %s\n", message);

        // Encerrar a conexão com o cliente
        // TODO: Fazer lógica para só encerrar quando todas as mensagens
        // da fila estiver enviada.
        close(cliente);
        break;
      }
    }

    close(descritor);
    printf("Servidor encerrado!\n");

  } else { // PROCESSO PAI (CAMADA DE APLICAÇÃO)

    // Espera o processo pai terminar (sincronização)
    // TODO: Fazer a lógica para receber e imprimir mensagem até a fila acabar
    waitpid(process_son, NULL, 0);
    application_header();

    // TODO: Aqui vamos imprimir a mensagem da camada de transporte
    // enviado via fila de mensagens

  }

  return 0;

}
