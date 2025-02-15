#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void printBinaryInteger(int n);
void printBinaryByte(uint8_t n);
void generateByteToWS2812DataLookupTable(void);

int main(void)
{
     int i;

     // for(i = 0; i < 256; i++)
     // {
     //      printBinaryByte(i);
     // }
     generateByteToWS2812DataLookupTable();

     return 0;
}

void printBinaryByte(uint8_t n)
{
     for(uint8_t i = 0; i < 8; i++)
     {
          // printf("%d", (n >> (7 - i)) & 0x01);
          printf("%s ", ((n >> (7 - i)) & 0x01) ? "110" : "100");
     }
     printf("\n");
}

void generateByteToWS2812DataLookupTable(void)
{
     printf("const uint8_t byteToWS2812DataLUT[256][8] = {\n");

     for(int n = 0; n < 256; n++)
     {
          printf("{");
          for(uint8_t i = 0; i < 8; i++)
          {
               printf("%s", ((n >> (7 - i)) & 0x01) ? "0b110" : "0b100");
               if(i < 7)
               {
                    printf(", ");
               }
          }
          
          if(n < 255)
          {
               printf("},\n");
          }
          else
          {
               printf("}\n");
          }
     }
     printf("};");
     printf("\n");
}

void printBinaryInteger(int n)
{
     int i;
     const int base = 2;
     char *str = calloc(33, sizeof(char));
     unsigned int num = (unsigned int) n;

     for(i = 0; i < 32; i++)
     {
          str[31 - i] = (num % base) ? '1' : '0';

          num /= base;
     }

     str[32] = '\0';

     printf("%s\n", str);

     free(str);
}