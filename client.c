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
#include <arpa/inet.h>

// Constantes
#define ERROR  -1
#define TRUE  1
#define FALSE  0
#define PORTA 2000
#define MSG_LEN 4096

// Placa de rede do servidor remoto
struct sockaddr_in remoto;

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

int create_comunication() {
  int descritor;  // Usado para escrever e ler dados no socket

  // Cria a comunicação (endpoint) via protocolo IPv4 da camada de redes
  // (dominio) e protocolo TCP da camada de transporte (tipo)
  descritor = socket(AF_INET, SOCK_STREAM, 0);

  // Caso ocorra algum erro
  if (descritor == ERROR) {
    perror("socket");
    exit(1);
  } else {
    printf("Socket criado com sucesso!\n");
  }

  // Pega o protocolo IPv4
  remoto.sin_family = AF_INET;
  // Transforma a porta em números de rede
  remoto.sin_port = htons(PORTA);
  // Insere o ip do servidor, transforma o IP em IP de rede.
  remoto.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Zera a memória da estrutura de dados local
  memset(remoto.sin_zero, 0x0, 8);

  return descritor;

}

void connect_to_server(int *descritor) {
    // Armazena o tamanho da placa de rede do servidor
    int len = sizeof(remoto);

    // Conecta ao servidor e verifica se a conexão foi um sucesso!
    if(connect(*descritor, (struct sockaddr*) &remoto, len) == ERROR) {
      perror("connect");
      exit(1);
    } else {
      printf("Conexão estabelecida com sucesso!\n");
    }
}

void welcome_from_server(int *descritor, char *message) {
    int string_len;           // Armazena o tamanho da string de buffer

    // Receber a mensagem do servidor.
    if((string_len = recv(*descritor, message, MSG_LEN, 0)) > 0) {
      // Insere o valor nulo no final e tira a quebra de linha
      message[string_len - 1] = '\0';
      printf("Mensagem recebida: %s\n", message);
    }
}

int main() {

  pid_t process_son;        // Processo filho
  int descritor;            // Usado para escrever e ler dados no socket
  char message[MSG_LEN];    // Mensagem a enviar

  // Cria um processo filho exatamente igual ao processo pai
  process_son = fork();

  if(process_son == 0) { // PROCESSO FILHO (CAMADA DE APLICAÇÃO)

    application_header();
    /* printf("Insira a mensagem: "); */
    /* fgets(message, MSG_SIZE, stdin); */
    // TODO: Fazer a lógica para inserir mensagem até o usuário colocar flag 0
    sleep(2);

  } else { // PROCESSO PAI (CAMADA DE TRANSPORTE)

    // TODO: O processo deve esperar até ter algo na fila para poder imprimir
    waitpid(process_son, NULL, 0);
    transport_header();
    descritor = create_comunication();
    connect_to_server(&descritor);
    welcome_from_server(&descritor, message);

    // Limpar o buffer
    memset(message, 0x0, MSG_LEN);

    // TODO: Isso deve sair a mensagem deve vir da fila da camada de aplicação
    fgets(message, MSG_LEN, stdin);

    // Envia a mensagem para o servidor
    send(descritor, message, strlen(message), 0);

    // MODIFICAR AQUI
    // TODO: encerrar somente quando a fila de mensagem acabar
    close(descritor);
    printf("Cliente encerrado!\n");

  }

  return 0;

}
