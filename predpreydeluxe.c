#include <stdlib.h>
#define _BSD_SOURCE
#include <stdio.h>
#include <math.h>
#include <time.h>
/* PREDATOR/PREY
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
	FILE *predator_file = fopen("predator.txt","w+");
	FILE *prey_file = fopen("prey.txt","w+");
	//grid grootte
	int x = 20;
	int y = 20;
	int xy = x*y;
	int steps = 150;
	double *poppreyarray, *poppredarray, *poppreyarraytot, *poppredarraytot;
	poppreyarray = malloc(steps*sizeof(double));
	poppredarray = malloc(steps*sizeof(double));
	poppreyarraytot = malloc(steps*sizeof(double));
	poppredarraytot = malloc(steps*sizeof(double));
	//aantal preys/predators
	int prey = 60;
	int predator = 30;
	//beginwaardes energie
	int startprey = 50;
	int startpredator = 50;
	double preypop, predpop, preypoptot, predpoptot;
	cell *cell2 = malloc(sizeof(cell) * x * y);
	cell *cell1 = malloc(sizeof(cell) * x * y);
	//cellen initialiseren
	int v, i, j, k, l, c, popprey = 0, poppred = 0;

	//hoe vaak de simulatie
	v = 5000;
	for(c=0;c<v;c++)
	{

		//initialiseer/reset simulatie
		for(i=0;i<y;i++)
		{
			for(j=0;j<x;j++)
	   		{
				cell1[i*x+j].prey = 0;
				cell1[i*x+j].predator = 0;
			}
		}
		//cellen vullen met prey/predators 
		//vul eerst prey, daarna predator
		for(i=0;i<prey;i++)
		{
				cell1[i].prey = startprey;
		}
		i=prey;
		j = prey + predator;
		for(;i<j;i++)
		{
	   		cell1[i].predator = startpredator;
		}
		//shuffle de cellen, 
		shuffle(cell1, x*y);
		//print de startgrid
		printf("-------------------START-------------------------- \n");
		printgrid(x, y, cell1);
		printf("-------------------------------------------------- \n");
		for(k=0;k<y;k++)
		{
		    for(l=0;l<x;l++)
		    {
				if(cell1[k*x+l].prey >= 1)
				{
					popprey++;
				}
				else if (cell1[k*x+l].predator >= 1)
				{
					poppred++;
				}
			}
		}
		printf("K %d \n",popprey);
		printf("W %d \n",poppred);

		j = steps;
		for(i=0;i<j;i++)
		{
			poppreyarray[i] = 0;
			poppredarray[i] = 0;
		}
		//steps loop
		for(i=0;i<j;i++)
		{
			printf("------------stap %d--------------------------------- \n",i+1);
			printgrid(x, y, cell1);
			movement(x, y, cell1, cell2);
			printf("--------------------------------------------------- \n");
			//populatie in file zetten=
			popprey = 0; 
			poppred = 0;
			//pop count loop 
			for(k=0;k<y;k++)
			{
		    	for(l=0;l<x;l++)
		    	{
					if(cell1[k*x+l].prey >= 1)
					{
						popprey++;
					}
					else if (cell1[k*x+l].predator >= 1)
					{
						poppred++;
					}
				}
			}
			poppreyarray[i] += popprey;
			poppredarray[i] += poppred;
			printf("K %d \n",popprey);
			printf("W %d \n",poppred);
			//preypop = (double)poppreyarray[j] / (double)xy;
			//predpop = (double)poppredarray[j] / (double)xy;
		}
		//gemiddelde uitrekenen
		if(c == 0)
		{
			for(i = 0; i<j;i++)
			{
				poppreyarraytot[i] = poppreyarray[i];
				poppredarraytot[i] = poppredarray[i];
			}
		}
		else
		{
			for(i = 0; i<j;i++)
			{
				poppreyarraytot[i] = (poppreyarraytot[i] + poppreyarray[i])/2;
				poppredarraytot[i] = (poppredarraytot[i] + poppredarray[i])/2;
			}
		}
	}

	for(i=0;i<j;i++)
	{
		//printf("%g \n",(double)xy);
		//printf("%g \n",poppreyarraytot[i]);
		predpop = poppreyarraytot[i] / (double)xy;
		preypop = poppredarraytot[i] / (double)xy;
		fprintf(predator_file, "%d %g \n",i,predpop);
		fprintf(prey_file, "%d %g \n",i,preypop);
	}
	fclose(predator_file);
	fclose(prey_file);
	
	FILE *pipe = popen("gnuplot -persist", "w");
	fprintf(pipe, "plot 'prey.txt' using 1:2 title 'preypop' with lines, \
                        'predator.txt' using 1:2 title 'predpop' with lines\n");
    fprintf(pipe,"exit \n");
    pclose(pipe);
	return 0;
/*
de preypop en predpop PER STAP van elke simulatie bijhouden in aparte variabele,  deze variabele delen door aantal keer simulatie gedaan, dit schrijven in .txt
*/
}









//Movement functie
int movement(int x, int y, cell *cell1, cell *cell2)
{
int keuze, i, j, k, l, stapenergie;
int procreate = 150;
int cellbuffer, cellbuffer2;
stapenergie = 10;
int done = 0;
int count = 0;
int popprey = 0, poppred = 0;
//aantal preys/predators
for(k=0;k<y;k++)
{
	for(l=0;l<x;l++)
	{
		if(cell1[k*x+l].prey >= 1)
		{
			popprey++;
		}
		else if (cell1[k*x+l].predator >= 1)
		{
			poppred++;
		}
	}
}
int countprey = popprey;
int countpredator = poppred;
srand((unsigned)time(NULL));
//copy cell to cell2
for(i=0;i<y;i++)
{
	for(j=0;j<x;j++)
	{
		cellbuffer = cell1[i*x+j].prey;
		cellbuffer2 = cell1[i*x+j].predator;
      	cell2[i*x+j].prey = cellbuffer;
		cell2[i*x+j].predator = cellbuffer2;
    }
}
//value prey van cell naar een random aanliggende cell
//mag geen prey voorkomen
for(i=0;i<y;i++)
{
    for(j=0;j<x;j++)
		{
			done = 0;
			if(countprey + countpredator < x*y)
			{
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				//-------------------------Prey----------------------------------//
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				if(cell1[i*x+j].prey != 0)
				{
					count = 0;
					while(!done)
					{
						if(count <= 50)
						{
							keuze = (rand() % 7);
							//printf("%d", keuze);
							switch(keuze)
							{
								//---------------------------------------------------------------//
								case 0:
								//left
								//random aanliggende cell checken
								if(j != 0)
								{
									if(cell2[i*x+j-1].prey == 0 && cell2[i*x+j-1].predator < 1)
									{
										//move energie
										cell2[i*x+j-1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[i*x+j-1].prey += stapenergie;
										done = 1;
										if(cell2[i*x+j-1].prey >= procreate)
										{
											cell2[i*x+j-1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[i*x+j-1].prey == 0 && cell2[i*x+j-1].predator >= 1)
									{
										//value prey naar predator
										cell2[i*x+j-1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
								case 1:
								//right
								//random aanliggende cell checken
								if(j!=(x-1))
								{
									if(cell2[i*x+j+1].prey == 0 && cell2[i*x+j+1].predator >= 1)
									{
										//move energie
										cell2[i*x+j+1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[i*x+j+1].prey += stapenergie;
										done = 1;
										if(cell2[i*x+j+1].prey >= procreate)
										{
											cell2[i*x+j+1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[i*x+j+1].prey == 0 && cell2[i*x+j+1].predator >= 1)
									{
										//value prey naar predator
										cell2[i*x+j+1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else
										break;
								}
								count++;
								break;
							
								//---------------------------------------------------------------//
								case 2:
								//topleft
								//random aanliggende cell checken
								if(i>0 && j != 0)
								{
									if(cell2[(i-1)*x+j-1].prey == 0 && cell2[(i-1)*x+j-1].predator < 1)
									{
										//move energie
										cell2[(i-1)*x+j-1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[(i-1)*x+j-1].prey += stapenergie;
										done = 1;
										if(cell2[(i-1)*x+j-1].prey >= procreate)
										{
											cell2[(i-1)*x+j-1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[(i-1)*x+j-1].prey == 0 && cell2[(i-1)*x+j-1].predator >= 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j-1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else
										break;
								}
								count++;
								break;
							
								//---------------------------------------------------------------//
								case 3:
								//topmid
								//random aanliggende cell checken
								if(i>0)
								{
									if(cell2[(i-1)*x+j].prey == 0 && cell2[(i-1)*x+j].predator < 1)
									{
								
										//move energie
										cell2[(i-1)*x+j].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[(i-1)*x+j].prey += stapenergie;
										done = 1;
										if(cell2[(i-1)*x+j].prey >= procreate)
										{
											cell2[(i-1)*x+j].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[(i-1)*x+j].prey == 0 && cell2[(i-1)*x+j].predator >= 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else 
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
								case 4:
								//topright
								//random aanliggende cell checken
								if(i>0 && j!=(x-1))
								{
									if(cell2[(i-1)*x+j+1].prey == 0 && cell2[(i-1)*x+j+1].predator < 1)
									{
										//move energie
										cell2[(i-1)*x+j+1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[(i-1)*x+j+1].prey += stapenergie;
										done = 1;
										if(cell2[(i-1)*x+j+1].prey >= procreate)
										{
											cell2[(i-1)*x+j+1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[(i-1)*x+j+1].prey == 0 && cell2[(i-1)*x+j+1].predator >= 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j+1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									
									}
									else
										break;
								}
							
								count++;
								break;

								//---------------------------------------------------------------//
								case 5:
								//botleft
								//random aanliggende cell checken
								if(j != 0)
								{
									if(cell2[(i+1)*x+j-1].prey == 0 && cell2[(i+1)*x+j-1].predator < 1)
									{
										//move energie
										cell2[(i+1)*x+j-1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[(i+1)*x+j-1].prey += stapenergie;
										done = 1;
										if(cell2[(i+1)*x+j-1].prey >= procreate)
										{
											cell2[(i+1)*x+j-1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[(i+1)*x+j-1].prey == 0 && cell2[(i+1)*x+j-1].predator >= 1)
									{
										//value prey naar predator
										cell2[(i+1)*x+j-1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else
										break;
								}
								count++;
								break;
						
								//---------------------------------------------------------------//
								case 6:
								//botmid
								//random aanliggende cell checken
								if(cell2[(i+1)*x+j].prey == 0 && cell2[(i+1)*x+j].predator < 1)
								{
									//move energie
									cell2[(i+1)*x+j].prey = cell1[i*x+j].prey;
									cell2[i*x+j].prey = 0;
									//geef energie door stap
									cell2[(i+1)*x+j].prey += stapenergie;
									done = 1;
									if(cell2[(i+1)*x+j].prey >= procreate)
									{
										cell2[(i+1)*x+j].prey = cell1[i*x+j].prey / 2;
										cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
										countprey++;
									}

								}
								else if (cell2[(i+1)*x+j].prey == 0 && cell2[(i+1)*x+j].predator >= 1)
								{

									//value prey naar predator
									cell2[(i+1)*x+j].predator += cell1[i*x+j].prey;
									cell2[i*x+j].prey = 0;
									done = 1;
								}
								count++;
								break;
						
								//---------------------------------------------------------------//
								case 7:
								//botright
								//random aanliggende cell checken
								if(j!=(x-1))
								{
									if(cell2[(i+1)*x+j+1].prey == 0 && cell2[(i+1)*x+j+1].predator < 1)
									{
										//move energie
										cell2[(i+1)*x+j+1].prey = cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										//geef energie door stap
										cell2[(i+1)*x+j+1].prey += stapenergie;
										done = 1;
										if(cell2[(i+1)*x+j+1].prey >= procreate)
										{
											cell2[(i+1)*x+j+1].prey = cell1[i*x+j].prey / 2;
											cell2[i*x+j].prey = cell1[i*x+j].prey / 2;
											countprey++;
										}
									}
									else if (cell2[(i+1)*x+j+1].prey == 0 && cell2[(i+1)*x+j+1].predator >= 1)
									{
										//value prey naar predator
										cell2[(i+1)*x+j+1].predator += cell1[i*x+j].prey;
										cell2[i*x+j].prey = 0;
										done = 1;
									}
									else
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
							}
						}
						else
						{
							cell2[i*x+j].prey += stapenergie;
							done = 1;
						}
					}
				}
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				//------------------------Predator-------------------------------//
				//---------------------------------------------------------------//
				//---------------------------------------------------------------//
				else if(cell1[i*x+j].predator >= 1)
				{
					while(!done)
					{
						if(count <=50)
						{
							keuze = (rand() % 7);
							//printf("%d", keuze);
							switch(keuze)
							{
								//---------------------------------------------------------------//
								case 0:
								//left
								//random aanliggende cell checken
								if(j != 0)
								{
									if(cell2[i*x+j-1].prey == 0 && cell2[i*x+j-1].predator < 1)
									{
										//move energie
										cell2[i*x+j-1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[i*x+j-1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[i*x+j-1].prey != 0 && cell2[i*x+j-1].predator < 1)
									{
										//value prey naar predator
										cell2[i*x+j-1].predator = cell1[i*x+j].predator + cell2[i*x+j-1].prey;
										cell2[i*x+j-1].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[i*x+j-1].predator >= procreate)
										{
											cell2[i*x+j-1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
								case 1:
								//right
								//random aanliggende cell checken
								if(j!=(x-1))
								{
									if(cell2[i*x+j+1].prey == 0 && cell2[i*x+j+1].predator < 1)
									{
										//move energie
										cell2[i*x+j+1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[i*x+j+1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[i*x+j+1].prey != 0 && cell2[i*x+j+1].predator < 1)
									{
										//value prey naar predator
										cell2[i*x+j+1].predator = cell1[i*x+j].predator + cell2[i*x+j+1].prey;
										cell2[i*x+j+1].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[i*x+j+1].predator >= procreate)
										{
											cell2[i*x+j+1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else
										break;
								}
								count++;
								break;
							
								//---------------------------------------------------------------//
								case 2:
								//topleft
								//random aanliggende cell checken
								if(i>0 && j != 0)
								{
									if(cell2[(i-1)*x+j-1].prey == 0 && cell2[(i-1)*x+j-1].predator < 1)
									{
										//move energie
										cell2[(i-1)*x+j-1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[(i-1)*x+j-1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[(i-1)*x+j-1].prey != 0 && cell2[(i-1)*x+j-1].predator < 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j-1].predator = cell1[i*x+j].predator + cell2[(i-1)*x+j-1].prey;
										cell2[(i-1)*x+j-1].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[(i-1)*x+j-1].predator >= procreate)
										{
											cell2[(i-1)*x+j-1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else
										break;
								}
								count++;
								break;
							
								//---------------------------------------------------------------//
								case 3:
								//topmid
								//random aanliggende cell checken
								if(i>0)
								{
									if(cell2[(i-1)*x+j].prey == 0 && cell2[(i-1)*x+j].predator < 1)
									{
								
										//move energie
										cell2[(i-1)*x+j].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[(i-1)*x+j].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[(i-1)*x+j].prey != 0 && cell2[(i-1)*x+j].predator < 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j].predator = cell1[i*x+j].predator + cell2[(i-1)*x+j].prey;
										cell2[(i-1)*x+j].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[(i-1)*x+j].predator >= procreate)
										{
											cell2[(i-1)*x+j].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else 
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
								case 4:
								//topright
								//random aanliggende cell checken
								if(i>0 && j!=(x-1))
								{
									if(cell2[(i-1)*x+j+1].prey == 0 && cell2[(i-1)*x+j+1].predator < 1)
									{
										//move energie
										cell2[(i-1)*x+j+1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[(i-1)*x+j+1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[(i-1)*x+j+1].prey != 0 && cell2[(i-1)*x+j+1].predator < 1)
									{
										//value prey naar predator
										cell2[(i-1)*x+j+1].predator = cell1[i*x+j].predator + cell2[(i-1)*x+j+1].prey;
										cell2[(i-1)*x+j+1].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[(i-1)*x+j+1].predator >= procreate)
										{
											cell2[(i-1)*x+j+1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}					
									}
									else
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
								case 5:
								//botleft
								//random aanliggende cell checken
								if(j != 0)
								{
									if(cell2[(i+1)*x+j-1].prey == 0 && cell2[(i+1)*x+j-1].predator < 1)
									{
										//move energie
										cell2[(i+1)*x+j-1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[(i+1)*x+j-1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[(i+1)*x+j-1].prey != 0 && cell2[(i+1)*x+j-1].predator < 1)
									{
										//value prey naar predator
										cell2[(i+1)*x+j-1].predator = cell1[i*x+j].predator + cell2[(i+1)*x+j-1].prey;
										cell2[(i+1)*x+j-1].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[(i+1)*x+j-1].predator >= procreate)
										{
											cell2[(i+1)*x+j-1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else
										break;
								}
								count++;
								break;
						
								//---------------------------------------------------------------//
								case 6:
								//botmid
								//random aanliggende cell checken
								if(cell2[(i+1)*x+j].prey == 0 && cell2[(i+1)*x+j].predator < 1)
								{
									//move energie
									cell2[(i+1)*x+j].predator = cell1[i*x+j].predator;
									cell2[i*x+j].predator = 0;
									//geef energie door stap
									cell2[(i+1)*x+j].predator -= stapenergie;
									done = 1;

								}
								else if (cell2[(i+1)*x+j].prey != 0 && cell2[(i+1)*x+j].predator < 1)
								{

									//value prey naar predator
									cell2[(i+1)*x+j].predator = cell1[i*x+j].predator + cell2[(i+1)*x+j].prey;
									cell2[(i+1)*x+j].prey = 0;
									cell2[i*x+j].predator = 0;
									done = 1;
									if(cell2[(i+1)*x+j].predator >= procreate)
									{
										cell2[(i+1)*x+j].predator = cell1[i*x+j].predator / 2;
										cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
										countpredator++;
									}
								}
								count++;
								break;
						
								//---------------------------------------------------------------//
								case 7:
								//botright
								//random aanliggende cell checken
								if(j!=(x-1))
								{
									if(cell2[(i+1)*x+j+1].prey == 0 && cell2[(i+1)*x+j+1].predator < 1)
									{
										//move energie
										cell2[(i+1)*x+j+1].predator = cell1[i*x+j].predator;
										cell2[i*x+j].predator = 0;
										//geef energie door stap
										cell2[(i+1)*x+j+1].predator -= stapenergie;
										done = 1;
									}
									else if (cell2[(i+1)*x+j+1].prey != 0 && cell2[(i+1)*x+j+1].predator < 1)
									{
										//value prey naar predator
										cell2[(i+1)*x+j+1].predator = cell1[i*x+j].predator + cell2[(i+1)*x+j+1].prey;
										cell2[(i+1)*x+j].prey = 0;
										cell2[i*x+j].predator = 0;
										done = 1;
										if(cell2[(i+1)*x+j+1].predator >= procreate)
										{
											cell2[(i+1)*x+j+1].predator = cell1[i*x+j].predator / 2;
											cell2[i*x+j].predator = cell1[i*x+j].predator / 2;
											countpredator++;
										}
									}
									else
										break;
								}
								count++;
								break;

								//---------------------------------------------------------------//
							}
						}
						else
						{
							cell2[i*x+j].predator -= stapenergie;
							done = 1;
						}
					}
				}
			}
		}
	}
	//copy cell to cell2
	for(i=0;i<y;i++)
	{
    	for(j=0;j<x;j++)
		{
			cellbuffer = cell2[i*x+j].prey;
			cellbuffer2 = cell2[i*x+j].predator;
			cell1[i*x+j].prey = cellbuffer;
			cell1[i*x+j].predator = cellbuffer2;
		}
	}
	return 0;
}



int printgrid(int x, int y, cell * cell1)
{
	//FILE *predator_file = fopen("predator.txt","w+");
	//FILE *prey_file = fopen("prey.txt","w+");
	int i, j;
	//int popprey = 0, poppred = 0;
	for(i=0;i<y;i++)
	{
    	for(j=0;j<x;j++)
    	{
			if(cell1[i*x+j].prey >= 1)
			{
				printf("K");
				//popprey++;
			}
			else if (cell1[i*x+j].predator >= 1)
			{
				printf("W");
				//poppred++;
			}
			else 
				printf(" ");        
    	}printf("\n");
	}
	//fprintf(predator_file, "%d %d \n",stap,poppred);
	//fprintf(prey_file, "%d %d \n",stap,popprey);
	//fclose(predator_file);
	//fclose(prey_file);
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
