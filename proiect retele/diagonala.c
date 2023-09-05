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
    
	int n,a,sum=0;
    scanf("%d",&n);
    for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++)
    {
        scanf("%d",&a);
        if(i==j)
        sum=sum+a;
    }

    printf("%d",sum);

	return 0;
}
