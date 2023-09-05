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
    
	int n;
    int v[1000];
    int a;
    scanf("%d",&n);
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a);
        v[i]=a;
    }

    for(int i=1;i<=n-1;i++)
    for(int j=i+1;j<=n;j++)
    if(v[i]<v[j])
    {
        a=v[i];
        v[i]=v[j];
        v[j]=a;
    }

    for(int i=1;i<=n-1;i++)
    printf("%d ",v[i]);
    printf("%d",v[n]);

	return 0;
}
