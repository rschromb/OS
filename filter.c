/*
Co-Authors:
Ekenechukwu Nwannunu

Ricky L Shrombeck

Operations Systems CS 302

Roger Kraft

26 January 2020

This is a filter program that formats a jumbled stream of input numbers
into organized columns.  All comments and statements referenced from Roger
Kraft's hw1_example incrementer program.


*/


#include <stdlib.h>
#include <stdio.h>

#define FLOAT_LENGTH 20

int main(int argc, char *argv[])
{
   int totColumn = 3;
   int precision = 13;
   char input[FLOAT_LENGTH];


   //Opens the configuration file and overrides the value in it.
   FILE * fp;
   if( ( (fp = fopen("filter.cfg", "r")) != NULL))
   {
      // get an operand value for the number of columns from file
      if(fgets(input, FLOAT_LENGTH, fp))
      {
         totColumn = atoi(input);
      }
      // get an operand value for the number of columns from file
      if(fgets(input, FLOAT_LENGTH, fp))
      {
         precision = atoi(input);
      }
   }
   // Override the default value with an environment varialbe value.
   // Environment varialbe for number of columns and precision.
   char* op = getenv("CS302HW1COLUMNS");
   if(op != NULL)
   {  //get an operand from the environment
      totColumn = atoi(op);
   }

   op = getenv("CS302HW1PRECISION");
   if(op != NULL)
   {  //get an operand from the environment
      precision = atoi(op);
   }
   // Get an argument from command line
   if(argc > 1)
   {  //get an operand from the command line
      totColumn = atoi(argv[1]);
   }

   if(argc > 2)
   {  //get an operand from the command line
      precision = atoi(argv[2]);
   }


   double x;
   int count = 0;
   // Reads the input of data until the end.
   // Processes the stream of numbers.
   while( scanf("%lf", &x) != EOF)
   {
      printf("%*.*f  ", (precision + 5), precision, x);

      count++;

      if(count == totColumn)
      {
         printf("\n");
         count = 0;
      }
   }
  return 0;
}