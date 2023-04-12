
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#define DEBUG
#include "./hashmap.h"

uint32 compare(void * a,void *b){
    printf("(compare '%s' '%s')",a,b);
    return b&&strcmp(a,b);
} 


int main(){
_hashmap * map = new_hashmap(32);  

map->compare=compare;

map->hash=str_hash;

char str[10005];
long long res;

for(long long i=0;i<=2000;i++){
    sprintf(str+i*5,"%d",i);
    
    printf("\ncur: %d size:%d put %s %d ",map->cur_capacity,map->max_capacity,str+i*5,i);
    hput(map,str+i*5 ,i);

}

for(long long i=0;i<=2000;i++){
    res=(long long)hget(map,str+i*5);
    printf("\nget %d = %d\n",i,res);
    assert(res==i);
}
printf("\ncomfirmed! yes!\n");

}