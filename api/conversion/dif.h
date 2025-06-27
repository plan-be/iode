#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*---------------- ENUMS -------------------------*/

enum IodeDIFElement
{
    DIF_EOD = 1,
    DIF_TABLE,
    DIF_VECTORS,
    DIF_DATA,
    DIF_BOT,
    DIF_COMMA,
    DIF_V,
    DIF_TUPLES  
};

#ifdef __cplusplus
}
#endif
