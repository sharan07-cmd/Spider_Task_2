#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "nittalk.h"

uint64_t crypto_math(const uint64_t base_int, const uint64_t prime_no, const uint64_t exp_num){
    uint64_t base=base_int % prime_no;                 /// This ensures that the base is lesser than the prime number
    uint64_t prime=prime_no;                           /// The prime number is copied into a variable "prime"  
    uint64_t exp=exp_num;                              /// The exponent number is copied to a variable "exp" 
                                            
    uint64_t result=1;                                 /// A variable result is set to 1     
    
    while(exp>0){                                      /// A while loop is set to implement the square and multiply algorithm
        if((exp & 1)==1){                              /// The exponent and 1 is made to undergo bitwise AND operator to check if the bit of the exponent is 1      
            result = (result * base) % prime;          /// The result is multipled with the base and performed modulus division with the prime number
        }
        base=((__int128)base*base) % prime ;           /// This is the squaring step in the square and multiplying algorithm. There is a type cast on base because when squaring takes place, it might be more than 64 bytes causing overflow
        exp>>=1;                                       /// Exponent undergoes right shift by 1                         
    }
    return result;
}

uint64_t generate_private_key(){                       /// A function for generating private keys is created
    FILE *fp=fopen("/dev/urandom","rb");               /// A random file is opened in read binary mode 

    if(fp==NULL){                                      /// If the file address is not found  
        printf("ERROR: ACCESS DENIED");                /// Error is NULL
        exit(1);                                       /// Programme is exited
    }

    uint64_t priv1;                                    /// A 64 bit variable is created to hold the private key
    fread(&priv1,sizeof(uint64_t),1,fp);               /// The data from the random file is shoved into the private key variable upto 64 bits 
    fclose(fp);                                        /// The file is closed             
    return (priv1 % (PRIME_NO - 1)) + 1;               /// As per fermat's little theorem, modular exponents always loop back to prime - 1 exactly. We are adding 1 because to take care if the random private key is exactly divisble by prime-1
}

uint8_t keystream_generator(uint64_t *common_key){          /// A function is created to have a psuedo random number generator, which stretches the common key
    uint64_t multiplier = 6364136223846793005ULL;           /// A multiplier variable is created 
    uint64_t increment = 1442695040888963407ULL;            /// An increment variable is created

    *common_key=((*common_key)*multiplier)+increment;       /// The common key is updated by multiplying the multiplier and added by the increment, resulting in a 64 bit output
    return (*common_key) >> 56;                             /// The output is 64 bit, but we need only 8 bits,so we are shifting by 56  
}

