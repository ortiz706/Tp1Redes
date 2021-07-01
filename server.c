#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

 //código para locais de vacina

        #define MAX_VAC_PLACES 50

        struct Locations{int x;int y;}places[MAX_VAC_PLACES];

        int locationsOnSystem =0; //locais cadastrados no sistema

        //procura pelo local no sistema
         int find(int x, int y){
            
         for(int i=0; i< locationsOnSystem;i++){
             if(places[i].x == x && places[i].y == y){
             return 0;
             }
         }
         return 1;
         }


        // int find(int x, int y){
        //     for(int i=0; i<locationsOnSystem;i++){
        //         printf("%i", places[i].x);
        //         return 1;
        //     }
        //     return 1;
        // }

        //remove local do sistema
        void rmPlace(int x, int y){
        

        for(int i=0; i< locationsOnSystem;i++){
            if(places[i].x == x && places[i].y == y){
                
                for(int a = i; a<locationsOnSystem-1; a++){
                    places[a].x = places[a+1].x;
                    places[a].y = places[a+1].y;
                    
                }
                
                locationsOnSystem--;
                
            }
        }
       
        }


        void printLocais(char * aux1){
           
            char a[10000];

            for (int i = 0; i < locationsOnSystem; i++)
            {
                sprintf(a,"%i %i ", places[i].x, places[i].y);
                strcat(aux1,a);
            }
            

        }


        struct Locations nearest(int x, int y){
            //distancia entre 2 pontos é
            //d=sqrt((xb-xa)^2+(yb-ya)^2)
            int d = 0;
            int aux = 0;
            struct Locations local;

            for (int i = 0; i < locationsOnSystem; i++)
            {
                //calcular distancia entre o local inserido e todos os locais
                aux = sqrt(((places[i].x-x)^2)+((places[i].y-y)^2));
                
                //conferir se a distancia calculada é menor que a anterior
                if (i==0 || aux<d)
                {
                    d=aux;
                    local.x=places[i].x;
                    local.y=places[i].y;

                }
                  
            }
            
            
            return local;
            
        }

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ]; 
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr); //primeira mensagem

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr); //terceira mensagem

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0); //recebe mensagem do cliente
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        //quebrando a mensagem recebida


        char a[10]="", b[10]="", c[10]="";
        char * msg = buf;
        char * comando[3];
        comando[0] = a;
        comando[1] = b;
        comando[2] = c;

        int contador = 0;
        msg = strtok(msg," \n");
        while (msg!=NULL)
        {
            comando[contador] = msg;
            msg = strtok(NULL," \n");
            contador++;
        }
        

        

        
        
        

       
       

        if( 0==strcmp(comando[0], "add") || 0==strcmp(comando[0], "Add") || 0==strcmp(comando[0], "ADD")){
           
           if(locationsOnSystem>=MAX_VAC_PLACES){
                sprintf(buf, "Local nao adicionado, numero maximo de lugares ja atingido.");
                
           }else if(find(atoi(comando[1]),atoi(comando[2])) !=1){ 
               sprintf(buf, "Local nao adicionado, já existe no sistema.");
           }else{
                places[locationsOnSystem].x = atoi(comando[1]) ;
                places[locationsOnSystem].y = atoi(comando[2]) ;
                locationsOnSystem++;
               sprintf(buf, "Local adicionado.");
           }

        }else if(0==strcmp(comando[0], "rm") || 0==strcmp(comando[0], "Rm") || 0==strcmp(comando[0], "RM")){
            
            if(find(atoi(comando[1]),atoi(comando[2])) != 1){
                rmPlace(atoi(comando[1]), atoi(comando[2]));
                sprintf(buf, "Local removido.");
            }else{
                sprintf(buf, "Local nao cadastrado.");
            }
        }else if(0==strcmp(comando[0], "list") || 0==strcmp(comando[0], "List") || 0==strcmp(comando[0], "LIST")){
            
            char aux1[100];
            printLocais(aux1);
            
            sprintf(buf, "Locais registrados no sistema: %s", aux1 );
        }else if(0==strcmp(comando[0], "query") || 0==strcmp(comando[0], "Query") || 0==strcmp(comando[0], "QUERY")){
           struct Locations localmaisproximo = nearest(atoi(comando[1]) ,atoi(comando[2]));
           sprintf(buf, "Local mais proximo para sua vacinação: %i %i", localmaisproximo.x, localmaisproximo.y);
        }else if(0==strcmp(comando[0], "kill") || 0==strcmp(comando[0], "Kill") || 0==strcmp(comando[0], "KILL")){
            sprintf(buf, "Servidor finalizado.");
            exit(EXIT_SUCCESS);
        }else{
            sprintf(buf, "Comando nao reconhecido.");
        }

        count = send(csock, buf, strlen(buf), 0); //envia retorno para o cliente de qual servidor esta conectado
        if (count != strlen(buf)) {
            logexit("send");
        }   


        close(csock);
    }

    exit(EXIT_SUCCESS);
}