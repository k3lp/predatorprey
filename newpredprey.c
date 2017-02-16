#include <stdlib.h>
#define _BSD_SOURCE
#include <stdio.h>
#include <math.h>
#include <time.h>
/* PREDATOR/PREY
Predator Prey Simulatie in C door Hidde Hensel

-Predator en/of prey moet je mee kunnen geven aan struct cell
-Predator en prey hebben beide 1 variabele energie
-Als predator naar lege cell gaat -energie
-Als prey naar lege cell gaat +energie
-Als predator naar cell gaat met prey, energie van prey naar predator
-Als prey naar cell gaat met predator verdwijnt prey
-Als predator en/of prey geen energie heeft -> verdwijnt
-Als predator en/of prey X energie bereikt, maak eenzelfde "object" aan met 1/2 X
- 
-x aantal predators y aantal preys, random verdeeld over de grid van cellen
-predators en preys nemen bij elke t, 1 stap in een random richting
-in de volgende cell mag niet nog een zelfde soort komen
-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
*/
typedef struct
{
	//prey in cell? 0 = empty, positief = energie
	int prey;
	//predator in cell? 0 = empty, positief = energie 
	int predator;
} cell;

int movement(int, int, cell *, cell *);
int printgrid(int, int, cell *);
int shuffle(cell *, int);

int main(int argc, char *argv[])
{
}

int printgrid(int x, int y, cell * cell1)
{
	int i, j;
	for(i=0;i<y;i++)
	{
    	for(j=0;j<x;j++)
    	{
			if(cell1[i*x+j].prey >= 1)
			{
				printf("K");
			}
			else if (cell1[i*x+j].predator >= 1)
			{
				printf("W");
			}
			else 
				printf(" ");        
    	}printf("\n");
	}
	return 0;
}








//shuffling for startgrid
int shuffle(cell * array, int n)
{
	int i,j,t,a;
	if (n > 1) 
	{
		for (i = 0; i < n - 1; i++)
		{
	  		j = i + rand() / (RAND_MAX / (n - i) + 1);
	  		t = array[j].prey;
	  		array[j].prey = array[i].prey;
	  		array[i].prey = t;
	  		a = array[j].predator;
	  		array[j].predator = array[i].predator;
	  		array[i].predator = a;
		}
	}return 0;
}
