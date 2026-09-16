#include "scr4.h"
#include "s_a2m.h"


void (*A2mMessage_super)(char*); // if not null, replaces the default implementation A2mMessage()

/*==================================================================
Fonction appelée pour afficher les messages en cours d'impression des
fichiers A2M provenant des fonctions A2mToMif(), etc.

Cette fonction peut être remplacée par une fonction utilisateur.

Par défaut, la fonction de librairie vaut simplement:

&CO
    SCR_comment(msg);
&TX

&SA A2mToMif(), A2mToRtf(), A2mToGdi(), A2mToHtml()
=============================================================== */

void A2mMessage(char *msg)
{
    if(A2mMessage_super)
        (*A2mMessage_super)(msg);
    else
        printf("\r%s", msg);
    
    // SCR_comment(msg);
}


/**
 *  Function replacing A2mMessage() to suppress a2m messages.
 *  
 *  @param [in] msg char*   message text 
 */
void A2mMessage_null(char*msg) 
{
}

/**
 *  Suppress or restore default ksmg() behaviour.
 *  
 *  @param [in] IsOn int    0: suppress the messages, 1: restore the default function
  */
void A2mMessage_toggle(int IsOn)
{
    static int  Current_IsOn = 1;
    static void (*A2mMessage_super_ptr)(char*); 
    
    if(IsOn && !Current_IsOn) { 
        A2mMessage_super = A2mMessage_super_ptr;
        Current_IsOn = 1;
    }
    else if(!IsOn && Current_IsOn) {
        A2mMessage_super_ptr = A2mMessage_super;
        A2mMessage_super = A2mMessage_null; 
        Current_IsOn = 0;
    }
}



