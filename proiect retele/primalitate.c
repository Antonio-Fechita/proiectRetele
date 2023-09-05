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
    

	int a;
    int ok=0;
    scanf("%d",&a);
    if(a<2)
    printf("neprim");
    else if(a==2)
    printf("prim");
    else if(a%2==0)
    printf("neprim");
    else
    {   
        for(int i=3;i<=a/2;i=i+2)
        if(a%i==0)
        {
            ok=1;
            break;
        }

        if(ok==0)
        printf("prim");
        else
        printf("neprim");
    }
	return 0;
}
