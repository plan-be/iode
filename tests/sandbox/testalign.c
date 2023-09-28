#include "iode.h"

int main(int argc, char *argv[])
{
   KDB              kdb1;
   TLINE            tline;
   EQ               eq;
   unsigned long    addr_tline, addr_tl_graph, addr_tl_pad,
                    addr_eq, addr_eq_method, addr_eq_smpl;

   addr_tline = &tline;
   addr_tl_graph = &(tline.tl_graph);
   addr_tl_pad = &(tline.tl_pad);
   addr_eq = &(eq);
   addr_eq_method = &(eq.method);
   addr_eq_smpl = &(eq.smpl);
   // addr_tl_axis = &(tline.tl_axis); // illegal address

   printf("sizeof(KDB)      = %ld\n", sizeof(KDB));
   printf("sizeof(TLINE)    = %ld\n", sizeof(TLINE));
   printf("sizeof(EQ)       = %ld\n", sizeof(EQ));
   printf("pos of tl_graph  = %ld\n", addr_tl_graph - addr_tline);    
   printf("pos of tl_pad    = %ld\n", addr_tl_pad - addr_tline);    
   printf("pos of eq_method = %ld\n", addr_eq_method - addr_eq);    
   printf("pos of eq_smpl   = %ld\n", addr_eq_smpl - addr_eq);    
} 
