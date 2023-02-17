#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int main(void){
   int num = 0,i;
   printf("Please enter the number of packets: ");
   scanf("%d", &num); //taking input for number of packets here

   if(num<=0){
        printf("Wrong input: the number of packets must be greater than 0."); //do input error checking here//
        return 0;
   }

   char packet[num]; //the array to store packet priority. e.g. HHL means high, high,low
   int high[num];   //high priority queue
   int low[num];    //low priority queue
   float buffer_size = 3;

   printf("Please enter the priority of the packets with no space between (Hrepresents high priority, L represents low priority):\n");
   scanf("%s", packet); //taking input array here

   if ((unsigned)strlen(packet) != num){
   //do input error checking here//
        printf("Wrong input: the number of packets is wrong.\n");
        return 0;
   }

   for(int i=0;i<num;i++){
   //do input error checking here//
   char c= packet[i];
   //printf("Letter %c\n",c);

   if(c =='\n'){
    printf("end\n");
    break;
    }else if((c !='H')&&(c!='L')){
          printf("Wrong input: the priority must be H or L.\n");
            return 0;
     }
   }


   //write the code to generate correct output here//
   for(i=0;i<num;i++){
     high[i]=-1;
     low[i]=-1;
   }

   printf("Input: %d %s  \n ", num, packet);
   int numArray= strlen(packet)/ buffer_size;
   int remainNum =strlen(packet)% 3;
   printf("THis is the number of Buffers we have %d\n", numArray);
   int index=0;

   for(i=0;i<numArray;i++){

   for(int j=0;j<buffer_size;j++){

    if(packet[index]=='H'){
           // printf("Going to store high %d\n", index);
     high[index]=index;
    } else if (packet[index]=='L') {
        //printf("Going to store low %d\n", index);
     low[index]=index;
   }
   index++;
   }
}

   for(int j=0; j<remainNum;j++){

    if(packet[index]=='H'){
            //printf("Going to store high %d\n", index);
     high[index]=index;
    } else if (packet[index]=='L') {
        //printf("Going to store low  %d\n", index);
     low[index]=index;
   }
   index++;
   }

printf("Output:");
index=0;
int countPrint;

for(i=0;i<numArray;i++){
    index=i*3;
    countPrint=0;
    while(countPrint<3){
        for(int j=0; j<buffer_size; j++){
            if(high[index]!= -1){
                  printf(" %d ", high[index] );
                  countPrint++;
            }
            index++;
        }

        index=i*3;
        if(countPrint<3){
            for(int j=0; j<buffer_size; j++){
                if(low[index]!= -1){
                    printf(" %d ", low[index] );
                    countPrint++;
                }
            index++;
            }
        }
     }

    }

   countPrint=0;

   while(countPrint< remainNum){

       for(int i=0;i<remainNum;i++){
           if(high[index]!= -1){
               printf(" %d ", high[index] );
               countPrint++;
           }
           index++;
       }

       index= index-remainNum;

        for(int i=0;i<remainNum;i++){
                if(low[index]!= -1){
                    printf(" %d ", low[index] );
                    countPrint++;
            }
            index ++;
        }
    }
 return 0;
}


