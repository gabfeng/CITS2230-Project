/*
 CITS2230 Project 1
 Name(s):		<Martin, French>, <Gabriel, Feng>
 Student number(s):	<20743871>, <20253518>
 Version 1.06
 Last edited: 210911 19:00
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/***************************************************/
/* Constants and Structire definitions */

#define MAXPROCESSES 100

typedef struct 
{
    /* The name of the process */
    char name[6];
    /* The arrival time of the process */
    int arrivalTime;
    /* The length of the process */
    int length;
} PROCESS;

typedef struct schedule
{
    int startTime;
    int runTime;
    struct schedule * next;
} TUPLE;

typedef struct 
{
    char name[6];
    TUPLE * head;
    TUPLE * tail;
} ORDER;

/****************************************************/
/* Global variable definitions */
PROCESS processes[MAXPROCESSES];
ORDER jobList[MAXPROCESSES];
PROCESS arrivals[MAXPROCESSES];
int pCount = 0;
int clock = 0;
/****************************************************/

/****************************************************/
/* Functions go here*/

static void initialisejobList()
{
    for (int i = 0; i < pCount; i++)
    {
        strcpy(jobList[i].name, processes[i].name);
    }
}

/**
 *  readProcessFile(filename) reads the contents of the process input file named filename
 */
static void readProcessFile(char filename[])
{
    FILE *input;
    char line[BUFSIZ];
    
    input = fopen(filename, "r");
    
    if (input == NULL) {
        printf("File \'%s\' could not be opened\n",filename);
        exit(EXIT_FAILURE);
    }
    
    
    while (fgets(line, sizeof line, input) != NULL) {
        if (sscanf(line, "%s %d %d", processes[pCount].name, &processes[pCount].arrivalTime, &processes[pCount].length) != 3)
        {
            fprintf(stderr, "Error in input file\n");
            exit(EXIT_FAILURE);
        }
        
        strcpy(arrivals[pCount].name,processes[pCount].name);
        arrivals[pCount].arrivalTime = processes[pCount].arrivalTime;
        arrivals[pCount].length = processes[pCount].length;
        
        pCount++;
    }
    strcpy(processes[pCount].name, "end");  
    processes[pCount].arrivalTime = 0;
    processes[pCount].length = 0;
    
    initialisejobList();
    
    printf("Sucessfully read process file \'%s\'\n",filename);
}

/**
 *  Creates a file called out.txt if one does not exist. Writes the details of
 *  the scheduling of a job. Writing mode is of append, so if file does exist it creates one.
 *  Prints out the jobList array.
 */
static void writeSchedule()
{
	FILE *fp;
	TUPLE * curr = (TUPLE *)malloc(sizeof(TUPLE));
    
	fp = fopen("out.txt", "a");
	if(fp == NULL)
	{
		fprintf(stderr,"Problem opening file\n");
		exit(EXIT_FAILURE);
	}
	
    for (int i = 0; i < pCount; i++) {
        fprintf(fp,"%s ",jobList[i].name);
        
        for(curr = jobList[i].head; curr; curr = curr->next)
        {
            fprintf(fp,"(%d, %d) ",curr->startTime, curr->runTime);
        }
        fprintf(fp,"\n");
	}
    
	fclose(fp);
}

static int findJobNum(char source[])
{
    for (int i = 0; i <pCount; i++)
    {
        if (strcmp(jobList[i].name, source) == 0)
        {
            return i;
        }
    }
    
    fprintf(stderr,"Cannot find Job Number!\n");
    exit(EXIT_FAILURE);
}

/**
 *  Sorts the processes by their arrival time.
 *  Rearraged processes array.
 */
 
static void sortProcessesFCFS(PROCESS sort[]) 
{
    PROCESS tmp;
    int pos;
    
    fprintf(stdout,"Arranging processes by arrival time......\n");
    
    if (pCount > 1) {
        for (int i = 1; i <pCount; i++) {
            strcpy(tmp.name, sort[i].name);
            tmp.arrivalTime = sort[i].arrivalTime;
            tmp.length = sort[i].length;
            pos = i-1;
            
            while ( pos >= 0 && sort[pos].arrivalTime > tmp.arrivalTime) {
                sort[pos+1] = sort[pos];
                pos--;
            }
            
            strcpy(sort[pos+1].name, tmp.name);
            sort[pos+1].arrivalTime = tmp.arrivalTime;
            sort[pos+1].length = tmp.length;
        }
    }
    else
    {
        fprintf(stderr,"There are no processes!\n");
        exit(EXIT_FAILURE);
    }
}

/**
 *  firstComeFirstServe() schedules processes based on the arrival time of the processes.
 *  Stores schedule into jobList array. 
 */
static void firstComeFirstServe()
{
    fprintf(stdout,"First Come First Serve scheduling......\n");
    sortProcessesFCFS(processes);
    
    int i = 0;
    
    while (strcmp(processes[i].name,"end") != 0) {
        
        TUPLE * curr = (TUPLE *)malloc(sizeof(TUPLE));
        int jobnum = findJobNum(processes[i].name);
        
        if ( processes[i].arrivalTime <= clock)
        {
            curr->startTime = clock;
            for (int j = 0; j < processes[i].length; j++)
            {
                clock++;
            }
            curr->runTime = clock - curr->startTime;
        }
        else 
        { 
            clock ++; 
            continue; 
        }
        
        curr->next = NULL;
        if( jobList == NULL)
        {
            fprintf(stderr,"jobList not initialized\n");
            exit(EXIT_FAILURE);
        }
        else if(jobList[jobnum].head == NULL && jobList[jobnum].tail == NULL)
        {
            jobList[jobnum].head = jobList[jobnum].tail = curr;
        }
        else if(jobList[jobnum].head == NULL || jobList[jobnum].tail == NULL)
        {
            fprintf(stderr, "Something wrong with assignments\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            jobList[jobnum].tail->next = curr;
            jobList[jobnum].tail = curr;
        }
        i++;
    }
    
    writeSchedule();
}

static void roundRobin() 
{
    fprintf(stdout,"RR\n");
}

static void shortestProcessNext()
{
    fprintf(stdout,"Shortest Process Next Scheduling......\n");
}

/**
 *  Sorts processes by their length. In the PROCESSES processes array int length 
 *  is treated as remaining time. Already scheduled processes have priority.
 */
static void sortProcessesSRT()
{
    PROCESS tmp;
    int pos;
    
    fprintf(stdout,"Arranging processes by remaining time......\n");
    
    if (pCount > 1) {
        for (int i = 1; i <pCount; i++) {
            strcpy(tmp.name, processes[i].name);
            tmp.arrivalTime = processes[i].arrivalTime;
            tmp.length = processes[i].length;
            pos = i-1;
            
            while ( pos >= 0 && (processes[pos].length > tmp.length || processes[pos].arrivalTime > tmp.arrivalTime) && tmp.arrivalTime <= clock) 
            {
                processes[pos+1] = processes[pos];
                pos--;
            }
            
            strcpy(processes[pos+1].name, tmp.name);
            processes[pos+1].arrivalTime = tmp.arrivalTime;
            processes[pos+1].length = tmp.length;
        }
    }
    
    FILE *fp;
    
    fp = fopen("list.txt","a");
    if (fp == NULL)
    {
        fprintf(stderr,"Problem opening file\n");
		exit(EXIT_FAILURE);
        
    }
    fprintf(fp,"%d\n",clock);
        
    for (int i = 0; i <pCount; i++)
    {
        fprintf(fp,"%s (%d, %d)\n",processes[i].name, processes[i].arrivalTime, processes[i].length);
    }
    fprintf(fp,"\n");
    fclose(fp);

}

static bool checkfinished()
{
    bool finished = true;
    for (int i = 0; i < pCount;i++)
    {
        if(processes[i].length != 0)
        {
            finished = false;
            return finished;
        }
    }
    return finished;
}

static void shortestRemainingTime()
{
    fprintf(stdout,"Shortest Remaining Time......\n");
    
    int i = 0;
    int k = 0;
    
    sortProcessesFCFS(arrivals);        
    sortProcessesSRT();
    
    while(!checkfinished())
    {
        TUPLE * curr = (TUPLE *)malloc(sizeof(TUPLE));
        int jobnum = findJobNum(processes[i].name);
        
        if (processes[i].arrivalTime <= clock && processes[i].length > 0)
        {
            curr->startTime = clock;
            for (int j = 0; j<processes[i].length; j++)
            {
                if(arrivals[k].arrivalTime == clock)
                {
                    if(arrivals[k].length < processes[i].length)
                    {
                        sortProcessesSRT();     //If new processes comes up for schedueling
                        k++;                    //re-sort if new processes has shorter
                        break;                  //length than current process
                    }
                    else
                    {
                        clock++;
                        k++;
                        processes[i].length--;
                    }
                }
                else 
                {
                    clock++;
                    processes[i].length--;
                }
            }
            curr->runTime = clock - curr->startTime;
            
        }
        else
        {
            clock++;
            sortProcessesSRT();
            continue;
        }
        
        curr->next = NULL;
        if( jobList == NULL)
        {
            fprintf(stderr,"jobList not initialized\n");
            exit(EXIT_FAILURE);
        }
        else if(jobList[jobnum].head == NULL && jobList[jobnum].tail == NULL)
        {
            jobList[jobnum].head = jobList[jobnum].tail = curr;
        }
        else if(jobList[jobnum].head == NULL || jobList[jobnum].tail == NULL)
        {
            fprintf(stderr, "Something wrong with assignments\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            jobList[jobnum].tail->next = curr;
            jobList[jobnum].tail = curr;
        }

        
        
        if(processes[i].length == 0)
        {
            i++;
        }
        else continue;
    }
    
 /*   
    
    FILE *fp;
    
    fp = fopen("out.txt","a");
    if (fp == NULL)
    {
        fprintf(stderr,"Problem opening file\n");
		exit(EXIT_FAILURE);

    }
    for (int i = 0; i <pCount; i++)
    {
        fprintf(fp,"%s (%d, %d)\n",processes[i].name, processes[i].arrivalTime, processes[i].length);
    }
    fclose(fp);
    */
    writeSchedule();
}

int main(int argc, char *argv[])
{
    /* Exit with an error if the number of arguments (including
	 the name of the executable) is not precisely 3
	 */
	if(argc != 3)
	{
		fprintf(stderr, "Usage %s <scheduling algorithm> <process input file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else
	{
		/* Read the processes to be scheduled from file */
        readProcessFile(argv[2]);
        
        /* Run the correct scheduling algorithm */
        if (strcmp(argv[1],"FCFS") == 0) {
            firstComeFirstServe();
        }
        
        else if (strcmp(argv[1],"RR") == 0) {
            roundRobin();
        }
        
        else if (strcmp(argv[1],"SPN") == 0) {
            shortestProcessNext();
        }
        
        else if (strcmp(argv[1],"SRT") == 0) {
            shortestRemainingTime();
        }
        else
        {
            printf("\'%s\' is an invalid scheduling algorithm\n",argv[1]);
            exit(EXIT_FAILURE);
        }
        
        /* Wrote the output file */
      //  writeSchedule();
        
        /* Finished successfully! */
		exit(EXIT_SUCCESS);
	}
	return 0;
	
}