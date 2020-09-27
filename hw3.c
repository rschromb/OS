/*
Authored by:
Ekenechukwu Nwannunu
&
Ricky Schrombeck
*/
#include <windows.h>
#include <stdio.h>
#include <math.h>

typedef struct processor_data {
   int affinityMask;            	/* affinity mask of this processor (just one bit set) */
   PROCESS_INFORMATION processInfo; /* process currently running on this processor */
   int running; /* 1 when this processor is running a task, 0 otherwise */
} ProcessorData;

/* function prototypes */
void printError(char* functionName);
void setProcPoolAffinity(ProcessorData processorPool[], int input, int index);//Takes in ProcessorData array, input value to go into array, and index of array,
                                                                          	//Sets affinityMask for the index value of an array of ProcessorData.
void assignHandle(ProcessorData processorPool[], HANDLE processHandles[], int arg);//Takes in array of ProcessorData, array of handles, and int argument
                                                                               	//Sets the arg index of array of handles to the hThread from processInfo struct
void intializeRunning(ProcessorData *processorPool, int arg);//Takes in ProcessorData array, and int argument. Sets running of that index to 0
void setRunning(ProcessorData processorPool[], int arg);//Takes in ProcessorData array, and int argument. Changes the running member to either 1 or 0.
int getRunning(ProcessorData processorPool[], int arg);//Takes in ProcessorData array, and int argument. Returns the value of running for the argument.
void swap( int * array1,  int * array2);

int main(int argc, char *argv[])
{
   int processorCount = 0;   	/* the number of allocated processors */
   HANDLE myProcess = GetCurrentProcess();
   DWORD_PTR processAffinityMask;
   DWORD_PTR systemAffinityMask;
	int *jobDurationTimes;

   if (argc < 3)
   {
  	fprintf(stderr, "usage, %s  SCHEDULE_TYPE  SECONDS...\n", argv[0]);
  	fprintf(stderr, "Where: SCHEDULE_TYPE = 0 means \"first come first serve\"\n");
  	fprintf(stderr, "   	SCHEDULE_TYPE = 1 means \"shortest job first\"\n");
  	fprintf(stderr, "   	SCHEDULE_TYPE = 2 means \"longest job first\"\n");
  	return 0;
   }
     
   /* read the job duration times off the command-line */

   const int totalJobTimes = argc - 2; // the total number of job times to be ran

   // Allocate memory for the job duration times being passed
   jobDurationTimes = (int *) malloc(sizeof(int) * totalJobTimes);

   printf("Scheduling job duration times: ");

   for (int i = 0; i < totalJobTimes; i++) {
   	jobDurationTimes[i] = (int) atoi(argv[i + 2]);
   	printf("%d ", jobDurationTimes[i]);	  	 
   }
	printf(" \n");

   /* sort the array of times based on the scheduler type selected at runtime */
   
   if (atoi(argv[1]) == 1) {
   	// Sort shortest job first
   	for (int i = 0; i < totalJobTimes - 1; i++) {
       	for (int j = i + 1; j < totalJobTimes; j++) {
           	if (jobDurationTimes[i] >  jobDurationTimes[j]) {
               	swap(&jobDurationTimes[i],  &jobDurationTimes[j]);
           	}
       	}
   	}

   } else if (atoi(argv[1]) == 2) {
   	// Sort longest job first
   	for (int i = 0; i < totalJobTimes - 1; i++) {
       	for (int j = i + 1; j < totalJobTimes; j++) {
           	if (jobDurationTimes[i] <  jobDurationTimes[j]) {
               	swap(&jobDurationTimes[i],  &jobDurationTimes[j]);
           	}
       	}
   	}
   }
  	printf("The sorted job times to be ran are: ");

   for (int i = 0; i < totalJobTimes; i++) {
   	printf("%d  ", jobDurationTimes[i]);
   }

   printf(" \n");

   /* Read the job duration times off the command-line. */
   
   /* Get the processor affinity mask for this process. */
 
  	GetProcessAffinityMask(myProcess, &processAffinityMask, &systemAffinityMask);
  	printf("processAffinityMask = %#6.4x\n", processAffinityMask);
   
   /* Count the number of processors set in the affinity mask. */
   int x = processAffinityMask; //The variable x is used frequently for various jobs, in this case it is being used to break down the affinity mask to
                            	//calculate the individual processors available.
   FILE * fp;
   fp = fopen("hw3.cfg", "w");
   while(x > 0)
   {
  	for(int i = 15; i >= 0; i--)
  	{
     	if(x >= pow(2, i))
     	{
        	x = x - pow(2, i);//Subtracts the appropiate power of two, and adds it to list of individual processor affinities.
        	processorCount++;
        	int temp = pow(2, i);
        	fprintf(fp, "%d\n", temp);
      	 
        	i = 0;
     	}
  	}
   }
   fclose(fp);
   x = argc - 2;//Now x is being used to maintain the argument count of times for computeProgram_64.exe
   /* Create, and then initialize, the processor pool array of data structures. */
   ProcessorData processorPool[processorCount]; /* an array of ProcessorData structures */
   HANDLE processHandles[processorCount];   	/* an array of handles to processes */
   int handleCount = 0;
   char lpCommandLine[256];
   sprintf(lpCommandLine, "computeProgram_64.exe");
   STARTUPINFO startInfo;
   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   fp = fopen("hw3.cfg", "r");
   
   for (int i = 0; i < processorCount; i++) {
            	if (processorPool[i].running) {
                	handleCount++;
            	}
        	}
     	printf("processorCount = %d \n", handleCount);
      /*Get Process ID*/
   int pid = GetCurrentProcessId();  
   printf("processID = %d\n", pid);

   for(int i = 0; i < processorCount; i++)
   {
  	char tempString[6];
  	fgets(tempString, 6, fp);
  	int temp = atoi(tempString);
  	setProcPoolAffinity(processorPool, temp, i);
   printf("launched %d second job on processor with mask = %#6.4x\n", jobDurationTimes[i], processorPool[i].affinityMask);
  	intializeRunning(processorPool, i);
   }
   /* Start the first group of processes. */
   int bookmark;//used in the event there are more arguments on the command line than processors available.
   for(int i = 0; i < processorCount && i < x; i++)
   {  
  	sprintf(lpCommandLine, "%s %d", lpCommandLine, jobDurationTimes[i]);//Concatenate the command line arguments onto the string to be used in CreateProcess
                      	 
  	if(! CreateProcess(NULL,
                     	lpCommandLine,
                     	NULL,
                     	NULL,
                     	FALSE,
                     	CREATE_NEW_CONSOLE|CREATE_SUSPENDED,
                     	NULL,
                     	NULL,
                     	&startInfo,
                     	&processorPool[i].processInfo))
  	{
     	printError("CreateProcess");
  	}
  	else
  	{
     	SetProcessAffinityMask(processorPool[i].processInfo.hProcess, processorPool[i].affinityMask);
      ResumeThread(processorPool[i].processInfo.hThread);
  	   setRunning(processorPool, i);
  	   assignHandle(processorPool, processHandles, i);
  	}
  	sprintf(lpCommandLine, "computeProgram_64.exe");//Resets the string, so that the new argument can be concatenated on.
  	if((i + 1) >= processorCount)//If more arguments exist than processors available, bookmark i for use in the while loop
  	{
  	bookmark = (i + 1);
     	 
  	}
   }
   fclose(fp);
   
   x = x - processorCount;//Update x to keep track of the remaining arguments
   
   if(x <= 0)//If no arguments are left, exit the program.
   return 0;
   
   /* Repeatedly wait for a process to finish and then,
  	if there are more jobs to run, run a new job on
  	the processor that just became free. */
   while (1)
   {
  	DWORD result = (-1);//Initialize results so that it can be used as a sentinel if not reassigned by WaitForMultipleObjects
 	 
  	/* Get, from the processor pool, handles to the currently running processes.x */   
  	/* Put those handles in an array.x */
  	/* Use a parallel array to keep track of where in the processor pool each handle came from.x */
  	int parallelArray[processorCount];
	 
  	/* Check that there are still processes running, if not, quit.x */
  	if(x == 0)
  	break;
 	 
   processHandles[processorCount] = malloc(handleCount * sizeof(processorPool));
  	/* wait for one of the running processes to end */
  	if (WAIT_FAILED == (result = WaitForMultipleObjects(handleCount, processHandles, FALSE, INFINITE)))
  	{
     	printError("WaitForMultipleObjects");
     	break;
  	}else
  	{
     	setRunning(processorPool, result);
     	parallelArray[1] = processorPool[result].affinityMask;
     	CloseHandle(processorPool[result].processInfo.hThread);
  	}
 	 
  	/* Translate result from an index in processHandles[] to an index in processorPool[]. */
  	parallelArray[0] = result;
  	/* Close the handles of the finished process and update the processorPool array. */
  	/* Check if there is another process to run on the processor that just became free. */
 	 
  	if(x > 0 && parallelArray[0] != (-1))
  	{
     	sprintf(lpCommandLine, "%s %d", lpCommandLine, jobDurationTimes[bookmark]);
      //printf("Job Duration Time = %d\n", jobDurationTimes[bookmark]);
      printf("launched %d second job on processor with mask = %#6.4x\n", jobDurationTimes[bookmark], processorPool[result].affinityMask);                 	 
     	if(! CreateProcess(NULL,
                     	lpCommandLine,
                     	NULL,
                     	NULL,
                     	FALSE,
                     	CREATE_NEW_CONSOLE|CREATE_SUSPENDED,
                     	NULL,
                     	NULL,
                     	&startInfo,
                     	&processorPool[result].processInfo))
     	{
        	printError("CreateProcess");
     	}
     	else
     	{
        	sprintf(lpCommandLine, "computeProgram_64.exe");
        	setProcPoolAffinity(processorPool, parallelArray[1], (int)result);
        	SetProcessAffinityMask(processorPool[result].processInfo.hProcess, processorPool[result].affinityMask); 
        	ResumeThread(processorPool[result].processInfo.hThread);
     	}
  	}
  	x--;//Decrement to keep track of remaining arguments to be scheduled.
  	bookmark++;//Increment to keep track of remaining command line arguments.
   }  
   return 0;
}
void intializeRunning(ProcessorData *processorPool, int arg)
{
   processorPool[arg].running = 0;
}
void setProcPoolAffinity(ProcessorData processorPool[], int input, int index)
{
   processorPool[index].affinityMask = input;
   
}

void assignHandle(ProcessorData processorPool[], HANDLE processHandles[], int arg)
{
   processHandles[arg] = processorPool[arg].processInfo.hThread;
}

int getRunning(ProcessorData processorPool[], int arg)
{
   return (int)processorPool[arg].running;
}

void setRunning(ProcessorData processorPool[], int arg)
{
   if(processorPool[arg].running == 0)
   {
  	processorPool[arg].running = 1;
  	return;
   }
   
   if(processorPool[arg].running == 1)
   {
  	processorPool[arg].running = 0;
  	return;
   }
}
void swap(int * array1, int * array2) {
	int tmpVal = *array2;
	*array2 = *array1;
	*array1 = tmpVal;
}

/****************************************************************
   The following function can be used to print out "meaningful"
   error messages. If you call a Windows function and it returns
   with an error condition, then call this function right away and
   pass it a string containing the name of the Windows function that
   failed. This function will print out a reasonable text message
   explaining the error.
*/
void printError(char* functionName)
{
   LPVOID lpMsgBuf;
   int error_no;
   error_no = GetLastError();
   FormatMessage(
     	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
     	NULL,
     	error_no,
     	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* default language */
     	(LPTSTR) &lpMsgBuf,
     	0,
     	NULL
   );
   /* Display the string. */
   fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no);
   fprintf(stderr, (const char*)lpMsgBuf);
   /* Free the buffer. */
   LocalFree( lpMsgBuf );
}


