#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE    1000

main()
{
    int i;
    int number_of_points;
    long random_num;

    scanf("%d",&number_of_points);


    printf("%d\n",number_of_points);
    for(i=0;i<number_of_points;i++)
    {
        random_num = random();
        printf("%ld\n", (random_num % MAX_SIZE));
    }   
}