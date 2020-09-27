/*
   Ricky Schrombeck
   HW2 CS 309
   Professor Kraft
   2/10/2020
*/

#include <stdio.h>
#include <windows.h>
#include<stdlib.h>

void printError(char* functionName);

int main()
{
   //Declare required variables, and structures needed for launching a process
   char lpCommandLine[256];
   PROCESS_INFORMATION processInfo;
   STARTUPINFO startInfo;
   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);  
   int choice = 0;
   
   do
   {
   sprintf(lpCommandLine, getenv("SYSTEMROOT"));
   
   printf("Please make a choice from the following list:\n");//Input desired number and use it to concatenate a string to send to CreateProcess
   printf("  0: Quit\n");
   printf("  1: Run Notepad\n");
   printf("  2: Run WordPad\n");
   printf(" *3: Run cmd shell\n");
   printf("  4: Run Calculator\n");
   printf("  5: Run Explorer\n");
   printf("Enter your choice now: ");
   scanf("%d", &choice);
   
   if(choice == 0)
   break;
   if(choice == 1)
   sprintf(lpCommandLine, "%s\\notepad.exe", lpCommandLine);
   if(choice == 2)
   sprintf(lpCommandLine, "%s\\write.exe", lpCommandLine);
   if(choice == 3)//Changes the STARTINFO structure members to create a custom window.
   {
   sprintf(lpCommandLine, "%s\\system32\\cmd.exe", lpCommandLine);
   startInfo.dwFlags = STARTF_USEFILLATTRIBUTE;
   startInfo.dwFillAttribute = FOREGROUND_BLUE| BACKGROUND_INTENSITY| BACKGROUND_BLUE| BACKGROUND_RED| BACKGROUND_GREEN;
   startInfo.lpTitle = "What is your command?";
   _putenv("PROMPT=Speak to me$g");
   }
   if(choice == 4)
   sprintf(lpCommandLine, "%s\\system32\\calc.exe", lpCommandLine);
   if(choice == 5)
   sprintf(lpCommandLine, "%s\\explorer.exe", lpCommandLine);
   
      if(! CreateProcess(NULL,
                         lpCommandLine,
                         NULL,
                         NULL,
                         FALSE,
                         NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
                         NULL,
                         NULL,
                         &startInfo,
                         &processInfo))
      {
         printError("CreateProcess");
      }
      else
      {
         printf("Started program %d with PID = %d\n", choice, (int)processInfo.dwProcessId);
         if(choice == 3)
         {
            printf("  waiting for program %d to terminate.\n", choice);//Creates pause until cmd window is closed.
            if(WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
            {
               DWORD exitValue;
               GetExitCodeProcess(processInfo.hProcess, &exitValue);
               printf("Program %d exited with return value %d\n", choice, (int)exitValue);//Returns exit value upon cmd terminating.
            }
         }
         CloseHandle(processInfo.hThread);
         CloseHandle(processInfo.hProcess);
      };
    printf("\n");
    sprintf(lpCommandLine, "", NULL);
   }while(choice != 0);
   
   return 0;
}
//Function to send error message to output if process fails to launch
void printError(char* functionName)
{
   LPVOID lpMsgBuf;
   int error_num;
   error_num = GetLastError();
   
    FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL,
         error_num,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
         (LPTSTR) &lpMsgBuf,
         0,
         NULL
   );
   
   fprintf(stderr, "\n%s failed on error %d: ", functionName, error_num);
   fprintf(stderr, (char*)lpMsgBuf);
   LocalFree( lpMsgBuf );
}