#include "client.h"
#include <stdlib.h>

int main( int argc, char* argv[] )
{
   char *host;
   CLIENT *clnt;
   int *result_1;
   int dummy;

   host = argv[1];


   clnt = clnt_create(host, VSNFS_PROG,
   VSNFS_VER1, "udp");
   if (clnt == NULL) {
      clnt_pcreateerror(host);
      exit(1);
   }
   result_1 = vsnfs_proc_null_1(&dummy, clnt);
   if (result_1 == NULL) {
      clnt_perror(clnt, "call failed:");
   }
   clnt_destroy( clnt );
   printf("result = %d\n",*result_1);
   return 1;
}


