/* gcc -Wall -fPIC `mysql_config --include --libs --cflags` */ 

#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define         MySQL_HOST    "Hostname"
#define         MySQL_USER    "UserName"
#define         MySQL_PASS    "password"
#define         MySQL_DB      "mysql"

int main()
{
   MYSQL mysql;
   MYSQL_RES *res;
   MYSQL_ROW row;
   int j; 
   char buf[24]; 
   char query[80];
   mysql_init(&mysql);
   mysql_real_connect(&mysql,MySQL_HOST,MySQL_USER,MySQL_PASS,MySQL_DB,0,NULL,0);
   sprintf(query,"SELECT * from user");
   mysql_real_query(&mysql,query,(unsigned int)strlen(query));
   res = mysql_use_result(&mysql);
   while((row = mysql_fetch_row(res))) { 

     j = snprintf(buf,sizeof(buf),"%s ",row[0]); 
     printf("j=%d str=%s\n", j, (j < sizeof(buf)) ? buf : "TOO Large");
 
   } 
   mysql_free_result(res);
   mysql_close(&mysql);

   return 0;
}

