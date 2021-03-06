#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#define SERVER_PORT 50000
#define SERVER_REPLY_SIZE 1000000
#define QUIT_COMMANDE "QUIT"
#define WANT_COMMANDE "WANT "
#define NEED_COMMANDE "NEED "
#define NOSUCHFILE "NOSUCHFILE"
#define CLIENT_REQUEST_SIZE 210

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char criteres[CLIENT_REQUEST_SIZE-5] , server_reply[SERVER_REPLY_SIZE], toSend[CLIENT_REQUEST_SIZE];

    //creation de la socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    //Connection au serveur master
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //communication avec le server master
    while(1)
    {
        //recuperation des criteres de recherche
        printf("saisissez les criteres de recherche [C1,C2,C3] : ");
        scanf("%s" , criteres);
        if(strcmp(QUIT_COMMANDE, criteres)==0) {
            close(sock);
            return 0;
        }
        //envoie des crietres au serveur master
        strncpy(toSend,WANT_COMMANDE,strlen(WANT_COMMANDE));
        strcat(toSend,criteres);
        if( send(sock , toSend , strlen(toSend) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Reponse du serveur master
        if( recv(sock , server_reply , SERVER_REPLY_SIZE , 0) < 0)
        {
            puts("recv failed");
            break;
        }

        puts(server_reply);
        //si aucun fichier trouve, retour a la saisie des criteres
        if(strstr(server_reply, NOSUCHFILE) != NULL)
            continue;
        memset(server_reply, 0, sizeof(server_reply));
        memset(toSend, 0, sizeof(toSend));
        memset(criteres, 0, sizeof(criteres));
        //recuperation de l'id du fichier voulu
        printf("saisissez l'id du fichier a recuperer : ");
        scanf("%s" , criteres);
        if(strcmp(QUIT_COMMANDE, criteres)==0) {
            close(sock);
            return 0;
        }
        //envoie de la requete au serveur
        strncpy(toSend,NEED_COMMANDE,strlen(NEED_COMMANDE));
        strcat(toSend,criteres);
        if( send(sock , toSend , strlen(toSend) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Recuperation du contenu du fichier et l'afficher
        if( recv(sock , server_reply , SERVER_REPLY_SIZE , 0) < 0)
        {
            puts("recv failed");
            break;
        }
        puts(server_reply);
        memset(server_reply, 0, sizeof(server_reply));
    }

    close(sock);
    return 0;
}