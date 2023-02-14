#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int n,a,b;
    scanf("%d %d %d",&n,&a,&b);
    int arr[n][n];

    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            int t;
            scanf("%d",&t);
            if(t<a || t>b)
            {
                printf("Maa chuda\n");
                j--;
            }
            else
                arr[i][j]=t;

            
        }
    }

    // outer loop: for each row
    // inner loop: creates fork which creates child for each cell


    return 0;
}