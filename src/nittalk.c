#include <stdio.h>
#include "nittalk.h"                                                                   /// Includes the FILE_HEADER nittalk.h file
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void server1(int num){                                                                 /// Creates a function to receive the file
    int server_id= socket(AF_INET,SOCK_STREAM,0);                                      /// A variable is kept to hold the number which is returned by the socket function
    struct sockaddr_in server_addr;                                                    /// sockaddr_in means socket address which is a standard struct provided by the linux, it has 3 fields. Address family, port number and IP address 

    server_addr.sin_family=AF_INET;                                                    /// The address family is stored in the struct
    server_addr.sin_port=htons(num);                                                   /// The port number is stored in the struct, but it is converted from host computer native int to a format which can be tramitted via network   
    server_addr.sin_addr.s_addr=INADDR_ANY;                                            /// This accepts any sort of IP address whether it is from local network, Ethernet or wifi 

    uint64_t private_key1=generate_private_key();                                      /// A private key is created for the receiver
    uint64_t public_key1=crypto_math(BASE_INT,PRIME_NO,private_key1);                  /// A public key is created for the receiver 

    uint64_t client_pub_key;                                                           /// A variable is created to hold the sender's public key

    bind(server_id,(struct sockaddr*)&server_addr,sizeof(server_addr));                /// This first takes the server_id plugs in with the IP address and the port no. It uses type cast bevause it is an univtsal function and it should accept all family of addresses
    listen(server_id,5);                                                               /// This turns the socket on and wait for the incoming connections, with maximum 5 connections at a time                                                                 
    int client_socket;

    

    client_socket=accept(server_id,NULL,NULL);                                         /// The Server ID tells to which port it should listen to, the 2nd nd 3rd arguement is about the caller's ID and the size which is not necessary

    recv(client_socket,&client_pub_key,sizeof(client_pub_key),0);                      /// The sender's public key is sent and it is stored in client_pub_key
    send(client_socket,&public_key1,sizeof(public_key1),0);                            /// The receiver's public key is sent to the sender

    uint64_t shared_key=crypto_math(client_pub_key,PRIME_NO,private_key1);             /// The common key is computed from the shared public keys 

    file_header received_file;                                                         /// struct received_file is created from nittalk.h file

    recv(client_socket,&received_file,sizeof(received_file),0);                        /// The incoming bytes is stored in the received_file variable which was received via the client_socket port

    if(strncmp(received_file.ver_bytes,"NIT\x00",4)!=0){                               /// If the initial 4 bytes of the received data is not NIT 
        close(client_socket);                                                          /// The port is closed
        printf("ERROR: HEADER NOT MATCHING\n");                                        /// Error is printed             
        return;
    }

    else{
        int num1=received_file.file_size;                                              /// The file size is stored in a variable     
        num1=ntohl(num1);                                                              /// The number is converted it from network transmission form to host computer type 
        received_file.file_size=num1;                                                  /// The file size is then updated 

        FILE *fp=fopen(received_file.file_name,"wb");                                  /// File is opened in write-bytes mode     
        char temp[4096];                                                               /// A temporary array is created     
        ssize_t total_bytes_received=0;                                                /// A variable for total bytes received is initiated 

        while(total_bytes_received<received_file.file_size){                           /// As long as the total bytes received is lesser than the file size     
            int bytes_read=recv(client_socket,&temp,sizeof(temp),0);                   /// recv function returns the number of bytes copied into the temp array 

            if(bytes_read==0){                                                         /// If nothing is copied means, the loop is broken 
                break;
            }

            else{
                for(int i=0;i<bytes_read;i++){                                         /// A loop is made to traverse through the temp array for decryption  
                    uint8_t noise=keystream_generator(&shared_key);                    /// A random data is obtained using the key_stream generator. However this random noise will be the same for encrypting and decrypting as we are giving the shared key as the input
                    temp[i]=temp[i]^noise;                                             /// We are XORing to decrypt
                }

                fwrite(temp,1,bytes_read,fp);                                          /// The bytes from temp array is stored in the file
                total_bytes_received=total_bytes_received+bytes_read;                  /// Total bytes received is updated
            }
        } 

        printf("✅ File '%s' received and decrypted successfully!\n", received_file.file_name);
        fclose(fp);                                                                    /// The file is closed         
        close(client_socket);                                                          /// The port is closed 
    }
}

void client1(const char *target_ip_add, const char *filename,int port){                /// Creates a function to send the file
    int client_id= socket(AF_INET,SOCK_STREAM,0);                                      /// A variable is kept to hold the socket number which is returned by the socket function
    struct sockaddr_in target_addr;                                                    /// sockaddr_in means socket address which is a standard struct provided by the linux, it has 3 fields. Address family, port number and IP address 

    uint64_t shared_key;

    target_addr.sin_family=AF_INET;                                                    /// The target address family is stored in the struct
    target_addr.sin_port=htons(port);                                                  /// The port number is stored in the struct, but it is converted from host computer native int to a format which can be tramitted via network 

    uint64_t private_key=generate_private_key();                                       /// A private key is generated for the sender                 
    uint64_t public_key=crypto_math(BASE_INT,PRIME_NO,private_key);                    /// A public key is created using the private key 

    inet_pton(AF_INET,target_ip_add,&target_addr.sin_addr);                            /// This converts the human readable IP address to binary form and stores in the struct 

    if(connect(client_id,(struct sockaddr*)&target_addr,sizeof(target_addr))==-1){     /// If the connection failed
        printf("ERROR\n");                                                             /// Error is printed                 
        return;
    }

    FILE *source_file=fopen(filename,"rb");                                            /// File is opened in read binary mode
    if(source_file==NULL){                                                             /// If the address in NULL means.....
        printf("ERROR\n");                                                             /// Error is printed                         
        return;
    }

    fseek(source_file,0,SEEK_END);                                                     /// Pointer is set to the end of the file EOP          
    uint32_t end_n=ftell(source_file);                                                 /// This tells us that at which byte the pointer is locating, thereby giving the file size 
    fseek(source_file,0,SEEK_SET);                                                     /// The pointer is again set to the starting point 

    file_header send_file;                                                             /// A send_file struct is created
    strncpy(send_file.ver_bytes,"NIT\x00",4);                                          /// The first 4 bytes are filled
    strncpy(send_file.file_name,filename,64);                                          /// The filename is updated  
    send_file.file_size=htonl(end_n);                                                  /// The file size is converted into a network tranmission format and it is stored in the struct

    send(client_id,&public_key,sizeof(public_key),0);                                  /// The public key of the sender is sent to the receiver             
    uint64_t server_pub_key;                                                           /// A variable is made to hold the public key of the receiver             

    recv(client_id, &server_pub_key, sizeof(server_pub_key), 0);                       /// The public key of the receiver is stored in the server_pub_key         

    shared_key=crypto_math(server_pub_key,PRIME_NO,private_key);                       /// The shared key is computed

    send(client_id,&send_file,sizeof(send_file),0);                                    /// This sends the details of file header across the server and tells how many bites of file size to be expected


    char temp[4096];                                                                   /// A temporary array is created
    while(1){

        int bytes_read=fread(temp,1,sizeof(temp),source_file);                         /// The variable bytes_read is created so that to keep track of the bytes copied from source_file to temp with 1 byte at a time

        if(bytes_read==0){                                                             /// If no bytes are copied..... 
            break;                                                                     /// The loop is broken                 
        }

        else{
            for(int i=0;i<bytes_read;i++){                                             /// A for loop is kept to traverse the temp array for encryption 
                uint8_t noise=keystream_generator(&shared_key);                        /// A random data is obtained using the key_stream generator. However this random noise will be the same for encrypting and decrypting as we are giving the shared key as the input                 
                temp[i]=temp[i]^noise;                                                 /// We are XORing to encrypt     
            }

            send(client_id,temp,bytes_read,0);                                         /// The bytes are sent across the server             
            
        }
    }
    printf("✅ File '%s' encrypted and transmitted successfully!\n", filename);
    
    fclose(source_file);                                                               /// The file is closed 
    close(client_id);                                                                  /// The socket is closed 
}