#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#define SIZE 1260785
#define BILLION 1000000000.0

//this function return the size of the file
int fsize(FILE *fp) { 
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return sz;
}

//this function get the socket and recieve the packets
void write_file(int sockf) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int n = 0;
    int num = 0;
    char buffer[SIZE];
    FILE *fp;
    int tmp;
    char *filename = "bigfile.txt";
    fp = fopen(filename, "r");
    int size = fsize(fp);
    int count = 0;
    int bool = 1;
    int sum = 0;
    while (sum <= 10*SIZE) {
        n = recv(sockf, buffer, SIZE, 0);
        if (n <= 0) { //if the recv function return 0 or less that mean the socket didnt get any.
            break;
            return;
        }
        sum = sum + strlen(buffer);
        if (sum >= 5*SIZE && bool) { //to check when it change to reno
            bool =0 ;
            clock_gettime(CLOCK_REALTIME, &end);
            double time_spent = (end.tv_sec - start.tv_sec) +
                                (end.tv_nsec - start.tv_nsec) / BILLION;
            printf("Time for transfer by cubic is: %f \n", time_spent);
            clock_gettime(CLOCK_REALTIME, &start);
        }
        bzero(buffer, SIZE);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;
    printf("Time for transfer by reno is: %f \n", time_spent);
    return;
}

int main() {
    char *ip = "127.0.0.1";
    int port = 10000;
    int sockf, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockf = socket(AF_INET, SOCK_STREAM, 0); // create a socket
    if (sockf < 0) { // check if the socket created currectly
        perror("socket error");
        exit(1);
    }
    printf("Server socket created successfully.\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    int err = bind(sockf, (struct sockaddr *) &server_addr, sizeof(server_addr)); //bind between the server to the client. in this case: measure.c and sender.c
    if (err < 0) {
        perror("bind error");
        exit(1);
    }
    printf("Binding successfully.\n");
    if (listen(sockf, 10) == 0) { //start to listen to sender.c
        printf("Connected...\n");
    } else {
        perror("Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockf, (struct sockaddr *) &new_addr, &addr_size); 
    write_file(new_sock);
    printf("The file was received successfully");
    return 0;
}
