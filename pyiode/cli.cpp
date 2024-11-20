#pragma once
#include "api/iode.h"

// Function to always call when importing iode 
void ODE_assign_super_PYIODE() 
{
   // IODE_assign_super_API(); // Not needed because already called by IodeInit(char*)
   //A2mMessage_super    = A2mMessage_super_PYIODE;
   //kconfirm_super      = kconfirm_super_PYIODE;
} 

int free_tbl(char** tbl)
{
    return SCR_free_tbl((unsigned char**) tbl);
}
