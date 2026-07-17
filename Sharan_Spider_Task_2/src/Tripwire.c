#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define TEAMMATE_IP "1.1.1.1"                                                  /// We are hardcoding a constant team IP address

int main() {
    int raw_socket;                                                            /// We are creating a variable to hold the socket_id
    raw_socket=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);                           /// The socket function is opened, and the OS is asked for an IPv4 connection, thereby asking for raw access to intercept the TCP packets.     
    if(raw_socket<0){                                                          /// If the socket id returned is lesser than 0 
        printf("USER FAILED TO RUN THE PROGRAM");                              /// Error is printed.                    
        return 1;                                                              /// This exits the programme safely 
    }

    unsigned char buffer[65536];                                               /// We are creating an UNSIGNED CHAR BUFFER         
    struct sockaddr_in source_socket_address;                                  /// A struct is created to store the receiving data 
    socklen_t saddr_len = sizeof(source_socket_address);                       /// We are getting the length of the struct source_socket_address. Also socklen_t is a special variable to hold the lengths of sockeet structuress     

    while(1){
        int data_size;                                                         /// A simple int variable is created to store the size of the data received     

        data_size=recvfrom(raw_socket,buffer,sizeof(buffer),0, (struct sockaddr *)&source_socket_address, &saddr_len);     /// The recvfrom() function returns the number of data bytes received. The entire data is stored in the buffer, while the details about the sender is stored in the struct.

        if(data_size<0){                                                       /// If the received data is 0..... 
            printf("ERROR\n");                                                 /// Error is printed 
            return 1;                                                          /// Programme is safely exited 
        }

        if(data_size < sizeof(struct iphdr)) {                                 /// If the data size is less than 20 which is the minimum size of an IP header which is at first of the data received via the recvfrom() function.
            continue;                                                          /// If it is lesser means the loop is continued and the programme waits for another TCP packet 
        }

        struct iphdr *ip_header= (struct iphdr *) buffer;                      /// We are formatting the random buffer data into a organised way and we store the address in a new pointer
        struct sockaddr_in source;                                             /// We are creating a new struct called source.     
        source.sin_addr.s_addr=ip_header->saddr;                               /// We are storing the IP address in the source struct 

        char *src_ip_string =inet_ntoa(source.sin_addr);                       /// We are converting the IP address to a readable string 

        if(strcmp(src_ip_string,TEAMMATE_IP)!=0 && strcmp(src_ip_string,"127.0.0.1")!=0 ){              /// We are checking if the string IP address and the TEAMMATE IP address are equal. We are also putting an AND condition to check if the IP address string is the IP address of the own computer.

            printf("\033[1;31m[!] ALERT: UNAUTHORIZED PROBE DETECTED!\033[0m\n");                       /// Error is printed    
            printf("INTRUDER IP: %s\n", src_ip_string);                                                 /// Error is printed

        }
    }
}