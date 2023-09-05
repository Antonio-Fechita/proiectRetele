#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


int main()
{
    //sleep(1);
    //sleep(2);
    //sleep(3);
    //sleep(4);
    srand((time(NULL) & 0xFFFF) | (getpid() << 16)); int f=rand() ;int r=f%8+1; sleep(r);
    //
    
    int a,b,c;
    scanf("%d",&a);
    scanf("%d",&b);
    scanf("%d",&c);
    printf("%d",a+b+c);

	return 0;
}
