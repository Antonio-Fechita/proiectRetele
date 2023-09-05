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
    //srand((time(NULL) & 0xFFFF) | (getpid() << 16)); int f=rand() ;int r=f%8+1; sleep(r);
    //
    
	int min,max;
	int a;
    scanf("%d",&a);
    min=a;
    max=a;
    while(a!=0)
    {
        scanf("%d",&a);
        if(a!=0)
        {
            if(a>max)
            max=a;
            if(a<min)
            min=a;
        }
    }

    printf("%d %d",min,max);

	return 0;
}
