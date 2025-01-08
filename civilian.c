#include <stdio.h>
#include "civilian.h"

void civilian_process()
{
    printf("Executing civilian process logic...\n");
    for (int i = 0; i < 5; i++)
    {
        printf("Civilian process iteration: %d\n", i + 1);
    }
}