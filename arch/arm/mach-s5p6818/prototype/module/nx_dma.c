//------------------------------------------------------------------------------
//
//  Copyright (C) Nexell Co. 2012
//
//  This confidential and proprietary software may be used only as authorized by a
//  licensing agreement from Nexell Co.
//  The entire notice above must be reproduced on all authorized copies and copies
//  may only be made to the extent permitted by a licensing agreement from Nexell Co.
//
//  Module      : Dma Controller
//  File        : nx_dma.c
//  Description :
//  Author      :
//  History     :
//                  2007/04/03  first
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <pyrope_framework.h>
#include "nx_chip.h"
#include "nx_dma.h"


#define CHANNELBUFFERSIZE 4096

//static    struct NX_DMA_RegisterSet *__g_pRegister = CNULL;
static  struct
{
    struct NX_DMA_RegisterSet *pRegister;
} __g_ModuleVariables[NUMBER_OF_DMA_MODULE] = { {CNULL,}, };

static NX_DMA_LLINODE DMA_LLIBODY;
static DMA_NODEPTR DMA_LLIHEAD;
static U32 g_DMA_COMMANDBuffer;

//-----------------------------------------------------------------------------------------------
void    NX_DMA_CreateLLI( U32 start, U32 size )
{
    DMA_NODEPTR headnode;
    DMA_NODEPTR firstnode;
	    
    headnode = DMA_LLIHEAD = &DMA_LLIBODY;
    //headnode  = (DMA_NODEPTR)malloc(sizeof(NX_DMA_LLINODE));
    firstnode = (DMA_NODEPTR)malloc(sizeof(NX_DMA_LLINODE));

    g_DMA_COMMANDBuffer = (U32)headnode;

    headnode->ptr = start;
    headnode->size = size;

    firstnode->ptr = headnode->ptr;
    firstnode->size = NULL;

    firstnode->next = NULL;
    firstnode->prev = headnode;
    headnode->next = firstnode;
    headnode->prev = firstnode;

    //return (DMA_NODEPTR)headnode;
} 

void* 	NX_DMA_LLIuAlloc( U32 size, U32 alignment )
{
    DMA_NODEPTR headnode = DMA_LLIHEAD;
    DMA_NODEPTR newnode;
    DMA_NODEPTR current;

    NX_ASSERT( size > 0 );
    NX_ASSERT( alignment > 0 );

    newnode = (DMA_NODEPTR)malloc(sizeof(NX_DMA_LLINODE));
    current = headnode->next;

    while(current->next != NULL)
    {
        if(current->next->ptr - (current->ptr+current->size) > (size+2*alignment))
        {
            newnode->ptr = (current->ptr + current->size + alignment - 1) & ~(alignment -1);
            newnode->size = size;

            if(newnode->ptr+newnode->size > headnode->ptr+headnode->size) 
                return NULL;
            break;
        }

        current = current->next;
    }

    if(current->next == NULL)
    {
        newnode->ptr = (current->ptr + current->size + alignment - 1) & ~(alignment -1);
        newnode->size = size;
        newnode->next = NULL;
        headnode->prev = newnode;
    }
    else
    {
        newnode->next = current->next;
        current->next->prev = newnode;
    }
    newnode->prev = current;
    current->next = newnode;

    return (void*)newnode->ptr;
}

void    NX_DMA_LLIuFree( void *addr )
{
    U32 freeaddr = (U32)addr;
    DMA_NODEPTR freenode;
    DMA_NODEPTR headnode = (DMA_NODEPTR)g_DMA_COMMANDBuffer;

    freenode = headnode->next;

    while(freenode != NULL)
    {
        if(freenode->ptr <= freeaddr && (freenode->ptr+freenode->size) > freeaddr)
        {
            freenode->prev->next = freenode->next;
            freenode->next->prev = freenode->prev;
            free(freenode);
            break;
        }
        freenode = freenode->next;
    }
}

void    NX_DMA_SetLLIAddress( U32 LLIAddress, U32 LLISize )
{
    NX_ASSERT( CNULL != LLIAddress );

    NX_DMA_CreateLLI( LLIAddress, LLISize );
    g_DMA_COMMANDBuffer = (U32)NX_DMA_LLIuAlloc( LLI_BUFFERSIZE, 0x10 );
}


U32     NX_DMA_GetLLIAddress( void )
{
    return (U32) g_DMA_COMMANDBuffer;
}

//-----------------------------------------------------------------------------------------------

//
//------------------------------------------------------------------------------
//  Module Interface
//------------------------------------------------------------------------------

/**
 *  @brief  Initialize of prototype enviroment & local variables.
 *  @return CTRUE    indicate that Initialize is successed.
 *               CFALSE   indicate that Initialize is failed.
 *  @see    NX_DMA_GetNumberOfModule
 */
CBOOL   NX_DMA_Initialize( void )
{
    //  @modified Gamza static variable(__g_ModuleVariables) is automatically filled by '0'
    //                  ë§Œì•½ ì´ˆê¸°??ê³¼ì •???„ì—­ë³€?˜ë? 0?¼ë¡œ ì´ˆê¸°???˜ëŠ” ?‘ì—… ?´ì™¸???¼ì„
    //                  ?´ì•¼?œë‹¤ë©?bInit ê°’ì„ CFALSEë¡??˜ì •?´ì•¼?œë‹¤.
    static CBOOL bInit = CTRUE;
    //register struct NX_DMA_RegisterSet *pRegister;
    U32 i;

    //NX_CONSOLE_Init();

    if( CFALSE == bInit )
    {
        for(i=0; i<NUMBER_OF_DMA_MODULE; i++)
        {
            __g_ModuleVariables[i].pRegister = CNULL;
        }
        bInit = CTRUE;
    }

    NX_DMA_SetLLIAddress( LLI_BUFFERADDR, LLI_BUFFERSIZE );

    return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get number of modules in the chip.
 *  @return     Module's number.
 *  @see        NX_DMA_Initialize
 */
U32     NX_DMA_GetNumberOfModule( void )
{
    return NUMBER_OF_DMA_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *  @brief      Get module's physical address.
 *  @return     Module's physical address
 *  @see                                        NX_DMA_GetSizeOfRegisterSet,
 *              NX_DMA_SetBaseAddress,          NX_DMA_GetBaseAddress,
 *              NX_DMA_OpenModule,              NX_DMA_CloseModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
U32     NX_DMA_GetPhysicalAddress( U32 ModuleIndex )
{
    const U32 PhysicalAddr[] =
    {
        PHY_BASEADDR_LIST( DMA )
    };
    NX_CASSERT( NUMBER_OF_DMA_MODULE == (sizeof(PhysicalAddr)/sizeof(PhysicalAddr[0])) );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

    return  PhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a size, in byte, of register set.
 *  @return     Size of module's register set.
 *  @see        NX_DMA_GetPhysicalAddress,
 *              NX_DMA_SetBaseAddress,          NX_DMA_GetBaseAddress,
 *              NX_DMA_OpenModule,              NX_DMA_CloseModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
U32     NX_DMA_GetSizeOfRegisterSet( void )
{
    return sizeof( struct NX_DMA_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set a base address of register set.
 *  @param[in]  BaseAddress Module's base address
 *  @return     None.
 *  @see        NX_DMA_GetPhysicalAddress,      NX_DMA_GetSizeOfRegisterSet,
 *                                              NX_DMA_GetBaseAddress,
 *              NX_DMA_OpenModule,              NX_DMA_CloseModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
void    NX_DMA_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
    NX_ASSERT( CNULL != BaseAddress );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
    __g_ModuleVariables[ModuleIndex].pRegister = (struct NX_DMA_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a base address of register set
 *  @return     Module's base address.
 *  @see        NX_DMA_GetPhysicalAddress,      NX_DMA_GetSizeOfRegisterSet,
 *              NX_DMA_SetBaseAddress,
 *              NX_DMA_OpenModule,              NX_DMA_CloseModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
U32     NX_DMA_GetBaseAddress( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

    return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}


//------------------------------------------------------------------------------
/**
 *  @brief      Initialize selected modules with default value.
 *  @return     \b CTRUE    indicate that Initialize is successed. \n
 *              \b CFALSE   indicate that Initialize is failed.
 *  @see        NX_DMA_GetPhysicalAddress,      NX_DMA_GetSizeOfRegisterSet,
 *              NX_DMA_SetBaseAddress,          NX_DMA_GetBaseAddress,
 *                                              NX_DMA_CloseModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
CBOOL   NX_DMA_OpenModule(U32 ModuleIndex)
{
    register struct NX_DMA_RegisterSet *pRegister;
    pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
    WriteIO32(&pRegister->Configuration, 0x1);
    return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Deinitialize selected module to the proper stage.
 *  @return     \b CTRUE    indicate that Deinitialize is successed. \n
 *              \b CFALSE   indicate that Deinitialize is failed.
 *  @see        NX_DMA_GetPhysicalAddress,      NX_DMA_GetSizeOfRegisterSet,
 *              NX_DMA_SetBaseAddress,          NX_DMA_GetBaseAddress,
 *              NX_DMA_OpenModule,
 *              NX_DMA_CheckBusy,               NX_DMA_CanPowerDown
 */
CBOOL   NX_DMA_CloseModule( U32 ModuleIndex )
{
    U32 i;
    register struct NX_DMA_RegisterSet *pRegister;
    pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
    WriteIO32(&pRegister->Configuration, 0x0);
    NX_DMA_ClearInterruptPendingAll();

    for (i=0; i<NUMBER_OF_DMA_CHANNEL ; i++)    NX_DMA_Stop(i+(ModuleIndex*8), CTRUE);

    return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether the selected modules is busy or not.
 *  @return     \b CTRUE    indicate that Module is Busy. \n
 *              \b CFALSE   indicate that Module is NOT Busy.
 *  @see        NX_DMA_GetPhysicalAddress,      NX_DMA_GetSizeOfRegisterSet,
 *              NX_DMA_SetBaseAddress,          NX_DMA_GetBaseAddress,
 *              NX_DMA_OpenModule,              NX_DMA_CloseModule,
 *                                              NX_DMA_CanPowerDown
 */
CBOOL   NX_DMA_CheckBusy( void )
{
    U32 i;
    U32 CheckValue;

    CheckValue = 0;
    for (i=0; i<NUMBER_OF_DMA_CHANNEL*NUMBER_OF_DMA_MODULE ; i++)   CheckValue |= NX_DMA_CheckRunning(i);

    if (0 != CheckValue)    return CTRUE;
    return CFALSE;
}

U32     NX_DMA_GetInterruptNumber( U32 nChannel )
{
    const U32 INTNumber[NUMBER_OF_DMA_MODULE] =
    {
        INTNUM_LIST(DMA)
    };
    U32         DMA_ModuleIndex = nChannel/8;

    NX_CASSERT( NUMBER_OF_DMA_MODULE == (sizeof(INTNumber)/sizeof(INTNumber[0])) );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );

    return INTNumber[DMA_ModuleIndex];
}
//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *  @brief      Set a specified interrupt to be enable or disable.
 *  @param[in]  IntNum  Interrupt Number .
 *  @param[in]  Enable  \b CTRUE    indicate that Interrupt Enable. \n
 *                      \b CFALSE   indicate that Interrupt Disable.
 *  @return     None.
 *  @see                                            NX_DMA_GetInterruptEnable,
 *              NX_DMA_GetInterruptPending,
 *              NX_DMA_ClearInterruptPending,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
void    NX_DMA_SetInterruptEnable( U32 nChannel, U32 IntNum, CBOOL Enable )
{
    const U32 INTENB_M      = 31; // module
    const U32 INTENB_C      = 14; // channel
    const U32 INTENB_C_MASK = (3UL<<INTENB_C);
    const U32 INTENB_M_MASK = (1UL<<INTENB_M);

    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    register struct NX_DMA_RegisterSet *pRegister;
    register    U32 regval;

    NX_ASSERT( NX_DMA_NUM_OF_INT > IntNum );
    NX_ASSERT( (0==Enable) || (1==Enable) );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    regval = ReadIO32(&pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & ~INTENB_M_MASK;
    regval |= (Enable << INTENB_M);
    WriteIO32(&pRegister->Channel[DMA_ChannelIndex].SGLLI.Control, regval);
    regval = ReadIO32(&pRegister->Channel[DMA_ChannelIndex].Configuration) & ~INTENB_C_MASK;
    regval |= (Enable << (INTENB_C+IntNum));
    WriteIO32(&pRegister->Channel[DMA_ChannelIndex].Configuration, regval);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether a specified interrupt is enabled or disabled.
 *  @param[in]  IntNum  Interrupt Number.
 *  @return     \b CTRUE    indicate that Interrupt is enabled. \n
 *              \b CFALSE   indicate that Interrupt is disabled.
 *  @see        NX_DMA_SetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
CBOOL   NX_DMA_GetInterruptEnable( U32 nChannel, U32 IntNum )
{
    register struct NX_DMA_RegisterSet *pRegister;
    register U32 regvalue;
    const U32 INTENB_POS    = 14;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NX_DMA_NUM_OF_INT > IntNum );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    //pRegister = __g_pRegister;
    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    regvalue = ReadIO32(&pRegister->Channel[DMA_ChannelIndex].Configuration);

    if( regvalue & ( 1UL << (IntNum+INTENB_POS)) )
    {
        return CFALSE;
    }

    return CTRUE;
}


//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether a specified interrupt is pended or not
 *  @param[in]  IntNum  Interrupt Number.
 *  @return     \b CTRUE    indicate that Pending is seted. \n
 *              \b CFALSE   indicate that Pending is Not Seted.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *                                                  NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
CBOOL   NX_DMA_GetInterruptPending( U32 nChannel , U32 IntNum )
{
    register struct NX_DMA_RegisterSet *pRegister;
    //register U32 regvalue;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    IntNum = IntNum;
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    if( ReadIO32(&pRegister->IntStatus) & 1UL<<DMA_ChannelIndex )
    {
        return CTRUE;
    }

    return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Clear a pending state of specified interrupt.
 *  @param[in]  IntNum  Interrupt number.
 *  @return     None.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *                                                  NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
void    NX_DMA_ClearInterruptPending( U32 nChannel, U32 IntNum )
{
    register struct NX_DMA_RegisterSet *pRegister;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    IntNum = IntNum;
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    if (0 != IntNum)    WriteIO32(&pRegister->IntTCClear, 1UL<<DMA_ChannelIndex);
    else                WriteIO32(&pRegister->IntErrClr, 1UL<<DMA_ChannelIndex);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set all interrupts to be enables or disables.
 *  @param[in]  Enable  \b CTRUE    indicate that Set to all interrupt enable. \n
 *                      \b CFALSE   indicate that Set to all interrupt disable.
 *  @return     None.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *                                                  NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
void    NX_DMA_SetInterruptEnableAll( CBOOL Enable )
{
    register struct NX_DMA_RegisterSet *pRegister;
    register U32 regvalue;
    const U32 INTENB_M  = 31;
    const U32 INTENB_C  = 14;
    const U32 INTENB_M_MASK = (1UL<<INTENB_M);
    const U32 INTENB_C_MASK = (3UL<<INTENB_C);
    int i, j;

    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( (0==Enable) || (1==Enable) );

    //NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    for (j=0 ; j<NUMBER_OF_DMA_MODULE; j++)
    {
        pRegister = __g_ModuleVariables[j].pRegister;

        for (i=0; i<NUMBER_OF_DMA_CHANNEL;i++)
        {
            regvalue = ReadIO32(&pRegister->Channel[i].SGLLI.Control) & ~INTENB_M_MASK;
            regvalue |= (Enable << INTENB_M);
            WriteIO32(&pRegister->Channel[i].SGLLI.Control, regvalue);
            regvalue = ReadIO32(&pRegister->Channel[i].Configuration) & ~INTENB_C_MASK;
            regvalue |= (Enable << INTENB_C) | (Enable << (INTENB_C+1));
            WriteIO32(&pRegister->Channel[i].Configuration, regvalue);
        }
    }
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether some of interrupts are enable or not.
 *  @return     \b CTRUE    indicate that At least one( or more ) interrupt is enabled. \n
 *              \b CFALSE   indicate that All interrupt is disabled.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
CBOOL   NX_DMA_GetInterruptEnableAll( void )
{
    return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether some of interrupts are pended or not.
 *  @return     \b CTRUE    indicate that At least one( or more ) pending is seted. \n
 *              \b CFALSE   indicate that All pending is NOT seted.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *                                                  NX_DMA_ClearInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
CBOOL   NX_DMA_GetInterruptPendingAll( void )
{
    register struct NX_DMA_RegisterSet *pRegister;
    NX_ASSERT( CNULL != __g_ModuleVariables );
    //NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    int j;
    for (j=0; j<NUMBER_OF_DMA_MODULE; j++)
    {
        pRegister = __g_ModuleVariables[j].pRegister;

        if(ReadIO32(&pRegister->IntStatus))
        {
            return CTRUE;
        }
    }

    return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Clear pending state of all interrupts.
 *  @return     None.
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,
 *              NX_DMA_GetInterruptPendingNumber
 */
void    NX_DMA_ClearInterruptPendingAll( void )
{
    register struct NX_DMA_RegisterSet *pRegister;
    int j;
    NX_ASSERT( CNULL != __g_ModuleVariables );
    //NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    for (j=0; j<NUMBER_OF_DMA_MODULE; j++)
    {
        pRegister = __g_ModuleVariables[j].pRegister;
        WriteIO32(&pRegister->IntTCClear, 0xFF );
        WriteIO32(&pRegister->IntErrClr, 0xFF );
    }
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a interrupt number which has the most prority of pended interrupts
 *  @return     Pending Number( If all pending is not set then return -1 ).
 *  @see        NX_DMA_SetInterruptEnable,          NX_DMA_GetInterruptEnable,
 *              NX_DMA_SetInterruptEnable64,        NX_DMA_GetInterruptEnable64,
 *              NX_DMA_GetInterruptPending,     	NX_DMA_GetInterruptPending64,
 *              NX_DMA_ClearInterruptPending,       NX_DMA_ClearInterruptPending64,
 *              NX_DMA_SetInterruptEnableAll,       NX_DMA_GetInterruptEnableAll,
 *              NX_DMA_GetInterruptPendingAll,      NX_DMA_ClearInterruptPendingAll
 */
U32     NX_DMA_GetInterruptPendingNumber( U32 nChannel )    // -1 if None
{
    register struct NX_DMA_RegisterSet *pRegister;
    register U32 regvalue;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    NX_ASSERT( CNULL != __g_ModuleVariables );

    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    regvalue = ReadIO32(&pRegister->IntTCStatus);
    regvalue &= (1UL << DMA_ChannelIndex);
    if (0 != regvalue)  return 1;
    regvalue = ReadIO32(&pRegister->IntErrorStatus);
    regvalue &= (1UL << DMA_ChannelIndex);
    if (0 != regvalue)  return 0;
    else                return -1;
}

#define NX_DMA_DEBUG	0

void    NX_DMA_SetAttribute( U32 nChannel, NX_DMA_CMDSET *pCmdSet )
{
    register    struct NX_DMA_RegisterSet *pRegister;
    register    U32 regvalue     = 0;

    U32         MAXTransferSize  = 0;
    U32 		CurTransferSize  = 0;
    U32         TransferSize     = pCmdSet->TxSize;
    U32 		Bytes 			 = ( (pCmdSet->SrcWidth << 4)/8);

    U32         DMA_ModuleIndex  = nChannel / 8;
    U32         DMA_ChannelIndex = nChannel % 8;

    //NX_ASSERT( 0 == (((U32)pCmdSet->SrcAddr) % 8) );
    //NX_ASSERT( 0 == (((U32)pCmdSet->DstAddr) % 8) );
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
    NX_ASSERT( NUMBER_OF_DMA_MODULE  > DMA_ModuleIndex  );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister  = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = pCmdSet->SrcAddr;
    pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = pCmdSet->DstAddr;

    if( Bytes == 0 ) Bytes = 1;

    if      (8  == pCmdSet->SrcWidth)    MAXTransferSize = (4*1024 )-Bytes; 
    else if (16 == pCmdSet->SrcWidth)  	 MAXTransferSize = (8*1024 )-Bytes;
    else                            	 MAXTransferSize = (16*1024)-Bytes;

    if( TransferSize >= (MAXTransferSize+Bytes) ) CurTransferSize = (MAXTransferSize/Bytes);
    else                                      	  CurTransferSize = (TransferSize/Bytes);

    //TransferSize = TransferSize-MAXTransferSize;  
    //---------------------------------------------------------------------------------------------
    regvalue |= (U32)( ( CurTransferSize       << 0  ) // Transfer size
                     | ( pCmdSet->SrcBurstSize << 12 ) // SBSize (source burst size)
                     | ( pCmdSet->DstBurstSize << 15 ) // DBSize (destination burst size)
                     | ( pCmdSet->SrcWidth     << 18 ) // SWidth (source transfer width)
                     | ( pCmdSet->DstWidth     << 21 ) // DWidth (destination transffer width)
                     | ( pCmdSet->SrcAHBSel    << 24 ) // Source AHB Master x Select
                     | ( pCmdSet->DstAHBSel    << 25 ) // Destination AHB Master x Select
                     | ( pCmdSet->SrcAddrInc   << 26 ) // SI (source increment)
                     | ( pCmdSet->DstAddrInc   << 27 ) // DI (destination increment) 
                     | ( 1UL   << 31 )
                     );

    pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue; // Control Register Set
    pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI 	= 0x0;

#if NX_DMA_DEBUG
    NX_MSG("TxSize:       %8d \r\n", pCmdSet->TxSize );
    NX_MSG("SrcBurstSize: %8d \r\n", pCmdSet->SrcBurstSize );
    NX_MSG("DstBurstSize: %8d \r\n", pCmdSet->DstBurstSize );
    NX_MSG("SrcWidth:     %8d \r\n", pCmdSet->SrcWidth );
    NX_MSG("DstWidth:     %8d \r\n", pCmdSet->DstWidth );
    NX_MSG("SrcAHBSel:    %8d \r\n", pCmdSet->SrcAHBSel );
    NX_MSG("DstAHBSel:    %8d \r\n", pCmdSet->DstAHBSel );
    NX_MSG("SrcAddrInc:   %8d \r\n", pCmdSet->SrcAddrInc );
    NX_MSG("DstAddrInc:   %8d \r\n", pCmdSet->DstAddrInc );
    NX_MSG("Control Register : %8x \r\n", regvalue );
#endif
    regvalue = 0;
    //---------------------------------------------------------------------------------------------
	if( pCmdSet->FlowCtrl != 0 ) {
        if( (pCmdSet->FlowCtrl == 6) || (pCmdSet->FlowCtrl == 2) )
        	regvalue = (U32)( pCmdSet->SrcPeriID << 1  );    // Source Peripheral.
        else if( (pCmdSet->FlowCtrl == 5) || (pCmdSet->FlowCtrl == 1) )
            regvalue = (U32)( pCmdSet->DstPeriID << 6  );    // Destination Peripheral.
        else if( (pCmdSet->FlowCtrl == 7) || (pCmdSet->FlowCtrl == 3) || (pCmdSet->FlowCtrl == 4) )
        {
            regvalue = (U32)( ( pCmdSet->SrcPeriID << 1  )    // Source Peripheral.
                            | ( pCmdSet->DstPeriID << 6  )    // Destination Peripheral.
                            );
        }           
    }

    regvalue |= (U32)( ( pCmdSet->FlowCtrl     << 11 ) // Flow Control and Transfer type.
                     | ( pCmdSet->IntIE        << 14 ) // Interrupt Error Mask.
                     | ( pCmdSet->IntITC       << 15 ) // Terminal Count interrupt Mask.
                    );
    //---------------------------------------------------------------------------------------------
    pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

#if NX_DMA_DEBUG
    NX_MSG("FlowCtrl:  %8d \r\n", pCmdSet->FlowCtrl );
    NX_MSG("SrcPeriID: %8d \r\n", pCmdSet->SrcPeriID );
    NX_MSG("DstPeriID: %8d \r\n", pCmdSet->DstPeriID );
    NX_MSG("IntIE:     %8d \r\n", pCmdSet->IntIE );
    NX_MSG("IntITC:    %8d \r\n", pCmdSet->IntITC );
    NX_MSG("Configuration Register : %8x \r\n", regvalue );
#endif    

}

void NX_DMA_Build_LLI2( U32 nChannel, NX_DMA_CMDSET *pCmdSet )
{
    register    struct NX_DMA_RegisterSet *pRegister;
    register    U32 regvalue     = 0;

    U32			MAXTransferSize  = 0;
    U32			CurTransferSize  = 0;
    U32			TransferSize     = pCmdSet->TxSize;
	U32			uAllicSize 	     = 0;
    U32			Bytes 			 = ( (pCmdSet->SrcWidth << 4)/8);

    U32			Number_of_LLI    = 0;
    U32			LLI_ADDR 		 = 0;
    U32			LLI_Count 		 = 0;
    U32			CmdBufferAddr 	 = 0;

    U32         DMA_ModuleIndex  = nChannel / 8;
    U32         DMA_ChannelIndex = nChannel % 8;

    //NX_ASSERT( 0 == (((U32)pCmdSet->SrcAddr) % 8) );
    //NX_ASSERT( 0 == (((U32)pCmdSet->DstAddr) % 8) );
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
    NX_ASSERT( NUMBER_OF_DMA_MODULE  > DMA_ModuleIndex  );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister  = __g_ModuleVariables[DMA_ModuleIndex].pRegister;

    if( Bytes == 0 ) Bytes = 1;

    if 		(8  == pCmdSet->SrcWidth)    MAXTransferSize = (4*1024 )-Bytes;
    else if (16 == pCmdSet->SrcWidth)  	 MAXTransferSize = (8*1024 )-Bytes;
    else                            	 MAXTransferSize = (16*1024)-Bytes;

    if( TransferSize >= (MAXTransferSize+Bytes) ) CurTransferSize = (MAXTransferSize/Bytes);
    else                                      	  CurTransferSize = (TransferSize+(Bytes-1)/Bytes);

    uAllicSize = (16 * (TransferSize/CurTransferSize))+1;
	LLI_ADDR   = CmdBufferAddr = (U32)NX_DMA_LLIuAlloc( uAllicSize, 0x10 );
    regvalue   = pRegister->Channel[DMA_ChannelIndex].SGLLI.Control;
    

 	Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
    
    if( Number_of_LLI != 0 )
    { 
        U32     SourceAddr      = 0;
        U32     DestinationAddr = 0;
        
	    LLI_Count = 0;
	    
	    while(1)
	    {
	        Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
	        if (0 != Number_of_LLI) CurTransferSize = MAXTransferSize/Bytes;
	        else                    CurTransferSize = TransferSize/Bytes;

	        TransferSize = TransferSize-MAXTransferSize;

            if( (pCmdSet->SrcPeriID >= 0) && (pCmdSet->SrcPeriID <= 31) )
	            SourceAddr      = (pCmdSet->SrcAddr);
            else
                SourceAddr      = (pCmdSet->SrcAddr)+(LLI_Count*MAXTransferSize);
                
            if( (pCmdSet->DstPeriID >= 0) && (pCmdSet->DstPeriID <= 31) )
	            DestinationAddr = (pCmdSet->DstAddr);
	        else
	            DestinationAddr = (pCmdSet->DstAddr)+(LLI_Count*MAXTransferSize);
	 
	        //if(0 == Number_of_LLI) regvalue |= (1UL<<31);
	        regvalue = (regvalue&~0xfff) | CurTransferSize;

	        *(volatile U32*)(LLI_ADDR+0x0) = SourceAddr;
	        *(volatile U32*)(LLI_ADDR+0x4) = DestinationAddr;
	        *(volatile U32*)(LLI_ADDR+0x8) = LLI_ADDR+0x10;
	        *(volatile U32*)(LLI_ADDR+0xC) = (regvalue);

	        if(0 == Number_of_LLI) 
	        {
	            *(volatile U32*)(LLI_ADDR+0x8) = 0;
	            break;
	        }
	        LLI_Count++;

	    #if 0
	        NX_MSG("----------------------------\r\n");
	        NX_MSG("LLIADDR+0x0: %8x \r\n", (volatile U32*)(LLI_ADDR+0x0) );
	        NX_MSG("LLIADDR+0x4: %8x \r\n", (volatile U32*)(LLI_ADDR+0x4) );
	        NX_MSG("LLIADDR+0x8: %8x \r\n", (volatile U32*)(LLI_ADDR+0x8) );
	        NX_MSG("LLIADDR+0xC: %8x \r\n", (volatile U32*)(LLI_ADDR+0xC) );
	        NX_MSG("----------------------------\r\n");
	        NX_MSG("LLIADDR+0x0: %8x \r\n", *(volatile U32*)(LLI_ADDR+0x0) );
	        NX_MSG("LLIADDR+0x4: %8x \r\n", *(volatile U32*)(LLI_ADDR+0x4) );
	        NX_MSG("LLIADDR+0x8: %8x \r\n", *(volatile U32*)(LLI_ADDR+0x8) );
	        NX_MSG("LLIADDR+0xC: %8x \r\n", *(volatile U32*)(LLI_ADDR+0xC) );
	        NX_MSG("----------------------------\r\n");
	        NX_MSG("Control Register: %8x \r\n", regvalue );
	    #endif
	        LLI_ADDR += 0x10;   
	    }
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}

	regvalue  = 0;
    regvalue  = pRegister->Channel[DMA_ChannelIndex].Configuration;
    regvalue |= (U32)(1);

	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

	regvalue = pRegister->Configuration;
	regvalue |= (1UL << 0);

	pRegister->Configuration = regvalue;		//DMA Module Enable
}

void    NX_DMA_Transfer( U32 nChannel, NX_DMA_CMDSET *pCmdSet )
{
	NX_DMA_SetAttribute( nChannel, pCmdSet );
	NX_DMA_Build_LLI2( nChannel, pCmdSet );
}

//------------------------------------------------------------------------------
// DMA Configuration Function
//------------------------------------------------------------------------------
/**
 *	@brief		Transfer memory to memory.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	pSource			source buffer address.\n this value must be aligned by 64 bits(8 bytes).
 *	@param[in]	pDestination	destination buffer address.\n this value must be aligned by 64 bits(8 bytes).
 *	@param[in]	TransferSize	transfer size in bytes.
 *	@return		None.
 *	@see										NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_TransferMemToMem( U32 nChannel, const void* pSource, void* pDestination, U32 TransferSize )
{
	U32			INTENB;
	//register	struct tag_ModuleVariables*		pVariables;
	U32			CmdBufferAddr;
	U32 		regvalue;
	U32 		Number_of_LLI;
	U32 		LLI_Count;
	U32 		CurTransferSize;
	
	U32 		DMA_ModuleIndex = nChannel/8;
	U32 		DMA_ChannelIndex = nChannel%8;
	register struct NX_DMA_RegisterSet *pRegister;
	//register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 8) );
	NX_ASSERT( 0 == (((U32)pDestination) % 8) );

	//NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
	NX_ASSERT( CNULL != __g_ModuleVariables);
//	NX_ASSERT( CNULL != g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
	NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

	pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
//	CmdBufferAddr = g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address+(CHANNELBUFFERSIZE*DMA_ChannelIndex);
    CmdBufferAddr = g_DMA_COMMANDBuffer + (CHANNELBUFFERSIZE*DMA_ChannelIndex);

	//MAXTransferSize = (16*1024)-4;
	U32 MAXTransferSize;
	MAXTransferSize = (16*1024)-4;
	Number_of_LLI = TransferSize/(MAXTransferSize+4);
	#ifdef NX_DEBUGLOG
	NX_CONSOLE_Printf(" Req TransferSize : %d\n", TransferSize );
	NX_CONSOLE_Printf(" Number_of_LLI : %d\n", Number_of_LLI );
	#endif
	if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
	else					CurTransferSize = TransferSize/4;
	
	TransferSize = TransferSize-MAXTransferSize;
	#ifdef NX_DEBUGLOG
	NX_CONSOLE_Printf(" CurTransferSize : 0x%x\n", CurTransferSize );
	NX_CONSOLE_Printf(" Nexet TransferSize : 0x%x\n", TransferSize );
	#endif
	
	//INTENB = NX_DMA_GetInterruptEnable(ModuleIndex, nChannel, 0);
	INTENB = NX_DMA_GetInterruptEnable(nChannel, 0);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = (U32)pSource;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = (U32)pDestination;

	regvalue = (pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & 0x80000000;

	INTENB = (regvalue>>31) & 1;
	
	regvalue |= (U32)( CurTransferSize		// Transfer size
					| (3<<26) // SBSize (source burst size)
					| (3<<12) // SBSize (source burst size)
					| (3<<15) // DBSize (destination burst size)
					| (2<<18) // SWidth (source transfer width)
					| (2<<21) // DWidth (destination transfer width)
					| (0UL<<24) // source master bus 0: AHB1, 1; AHB:2
					| (0UL<<25) // destination master bus
					);
	// tranfer size 가 16Kbyte-4byte 보다 클경우 LLI 의 마지막 에서 interrupt enable 한다.
	if (0 != Number_of_LLI)	regvalue = regvalue & ~(1UL<<31);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = 0x0;
	if (0 != Number_of_LLI)
	{
		LLI_Count = 1;
		while(1)
		{
			Number_of_LLI = TransferSize/(MAXTransferSize+4);
			if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
			else					CurTransferSize = TransferSize/4;

			TransferSize = TransferSize-MAXTransferSize;
			// last link list 일때 interrupt enable 한다.
			if(0 == Number_of_LLI) regvalue |= (INTENB<<31);

			regvalue = (regvalue&~0xfff) | CurTransferSize;

			NX_DMA_Build_LLI( (U32)pSource+(LLI_Count*MAXTransferSize),
								(U32)pDestination+(LLI_Count*MAXTransferSize),
								(U32)regvalue,
								(U32)CmdBufferAddr+((LLI_Count-1)*4),
								(U32)Number_of_LLI);
		#ifdef NX_DEBUGLOG
			NX_CONSOLE_Printf(" Number_of_LLI[%d]\n", LLI_Count );
			NX_CONSOLE_Printf(" Number_of_LLI[%d] NexeLLI : %d\n", LLI_Count,  Number_of_LLI);
			NX_CONSOLE_Printf(" Number_of_LLI[%d] SrcAddr : 0x%x\n", LLI_Count,  pSource+(LLI_Count*MAXTransferSize));
			NX_CONSOLE_Printf(" Number_of_LLI[%d] DstAddr : 0x%x\n", LLI_Count,  pDestination+(LLI_Count*MAXTransferSize));
			NX_CONSOLE_Printf(" Number_of_LLI[%d] LLIADDR : 0x%x\n", LLI_Count,  CmdBufferAddr+(LLI_Count*4));
			NX_CONSOLE_Printf(" Number_of_LLI[%d] CurTransferSize : 0x%x\n", LLI_Count,  CurTransferSize);
			NX_CONSOLE_Printf(" Number_of_LLI[%d] Next TrSize : 0x%x\n", LLI_Count,  TransferSize);
		#endif
			if(0 == Number_of_LLI)
			{
				break;
			}
			LLI_Count++;
		}
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}
	regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration & (0x3<<14);
	regvalue |= (U32)(1);
	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;
}

void	NX_DMA_TransferMemToIO( U32 nChannel, const void* pSource, void* pDestination, U32 DestinationPeriID, U32 DestinationBitWidth, U32 TransferSize )
{
	//register	struct tag_ModuleVariables*		pVariables;
	U32			CmdBufferAddr;
	U32 		regvalue;
	U32 		Number_of_LLI;
	U32 		LLI_Count;
	U32 		CurTransferSize;
	U32 		INTENB;
	
	U32 		DMA_ModuleIndex = nChannel/8;
	U32 		DMA_ChannelIndex = nChannel%8;
	register struct NX_DMA_RegisterSet *pRegister;
	//register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 8) );
	NX_ASSERT( 0 == (((U32)pDestination) % 2) );

	NX_ASSERT( CNULL != __g_ModuleVariables);
//	NX_ASSERT( CNULL != g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
	NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

	pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
//	CmdBufferAddr = g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address + (CHANNELBUFFERSIZE*DMA_ChannelIndex);
    CmdBufferAddr = g_DMA_COMMANDBuffer + (CHANNELBUFFERSIZE*DMA_ChannelIndex);

	//MAXTransferSize = (16*1024)-4;
	U32 MAXTransferSize;
	MAXTransferSize = (16*1024)-4;
	Number_of_LLI = TransferSize/(MAXTransferSize+4);
	if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
	else					CurTransferSize = TransferSize/4;
	
	TransferSize = TransferSize-MAXTransferSize;
	
	INTENB = NX_DMA_GetInterruptEnable(nChannel, 0);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = (U32)pSource;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = (U32)pDestination;

	regvalue = (pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & 0x80000000;

	INTENB = (regvalue>>31) & 1;
	
	regvalue |= (U32)( CurTransferSize		// Transfer size
					| (0<<12) // SBSize (source burst size)
					| (0<<15) // DBSize (destination burst size)
					| (2<<18) // SWidth (source transfer width)
					| ((DestinationBitWidth>>4)<<21) // DWidth (destination transfer width)
					| (0UL<<24) // source master bus 0: AHB1, 1; AHB:2
					| (1UL<<25) // destination master bus
					| (1UL<<26) // SI (source increment)
					| (0UL<<27)	// DI (destination increment)
					);
	// tranfer size 가 16Kbyte-4byte 보다 클경우 LLI 의 마지막 에서 interrupt enable 한다.
	if (0 != Number_of_LLI)	regvalue = regvalue & ~(1UL<<31);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = 0x0;
	if (0 != Number_of_LLI)
	{
		LLI_Count = 1;
		while(1)
		{
			Number_of_LLI = TransferSize/(MAXTransferSize+4);
			if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
			else					CurTransferSize = TransferSize/4;

			TransferSize = TransferSize-MAXTransferSize;
			// last link list 일때 interrupt enable 한다.
			if(0 == Number_of_LLI) regvalue |= (INTENB<<31);

			regvalue = (regvalue&~0xfff) | CurTransferSize;

			NX_DMA_Build_LLI( (U32)pSource+(LLI_Count*MAXTransferSize),
								(U32)pDestination,
								(U32)regvalue,
								(U32)CmdBufferAddr+((LLI_Count-1)*4),
								(U32)Number_of_LLI);
			if(0 == Number_of_LLI)
			{
				break;
			}
			LLI_Count++;
		}
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}
	regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration & (0x3<<14);
	regvalue |= (U32)(1
					| (DestinationPeriID<<6) // destination id
					| (1<<11) // mode
					);
	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

}

void	NX_DMA_TransferIOToMem( U32 nChannel, const void* pSource, U32 SourcePeriID, U32 SourceBitWidth, void* pDestination, U32 TransferSize )
{
	//register	struct tag_ModuleVariables*		pVariables;
	U32			CmdBufferAddr;
	U32 		regvalue;
	U32 		Number_of_LLI;
	U32 		LLI_Count;
	U32 		CurTransferSize;
	U32			INTENB;
	U32 		DMA_ModuleIndex = nChannel/8;
	U32 		DMA_ChannelIndex = nChannel%8;
	register struct NX_DMA_RegisterSet *pRegister;
	//register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 2) );
	NX_ASSERT( 0 == (((U32)pDestination) % 8) );
    //MES_REQUIRE ( 64 > SourcePeriID );
    //MES_REQUIRE ( 8 == SourceBitWidth || 16 == SourceBitWidth || 32 == SourceBitWidth );

	NX_ASSERT( CNULL != __g_ModuleVariables);
//	NX_ASSERT( CNULL != g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
	NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

	pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
//	CmdBufferAddr = g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address + (CHANNELBUFFERSIZE*DMA_ChannelIndex);
    CmdBufferAddr = g_DMA_COMMANDBuffer + (CHANNELBUFFERSIZE*DMA_ChannelIndex);

	//MAXTransferSize = (16*1024)-4;
	U32 Bytes = SourceBitWidth/8;
	U32 MAXTransferSize;
    if (8 == SourceBitWidth)		MAXTransferSize = (4*1024)-Bytes;
    else if (16 == SourceBitWidth)	MAXTransferSize = (8*1024)-Bytes;
    else							MAXTransferSize = (16*1024)-Bytes;
	//MAXTransferSize = (16*1024)-4;
	Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
	if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/Bytes;
	else					CurTransferSize = TransferSize/Bytes;
	
	TransferSize = TransferSize-MAXTransferSize;
	
	INTENB = NX_DMA_GetInterruptEnable(nChannel, 0);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = (U32)pSource;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = (U32)pDestination;

	regvalue = (pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & 0x80000000;

	INTENB = (regvalue>>31) & 1;
	
	regvalue |= (U32)( (CurTransferSize&0xFFF)		// Transfer size
					| (0<<12) // SBSize (source burst size)
					| (0<<15) // DBSize (destination burst size)
					| ((SourceBitWidth>>4)<<18) // SWidth (source transfer width)
					| (2<<21) // DWidth (destination transfer width)
					| (1UL<<24) // source master bus 0: AHB1, 1; AHB:2
					| (0UL<<25) // destination master bus
					| (0UL<<26) // SI (source increment)
					| (1UL<<27)	// DI (destination increment)
					);
	if (0 != Number_of_LLI)	regvalue = regvalue & ~(1UL<<31);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = 0x0;
	if (0 != Number_of_LLI)
	{
		LLI_Count = 1;
		while(1)
		{
			Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
			if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/Bytes;
			else					CurTransferSize = TransferSize/Bytes;

			TransferSize = TransferSize-MAXTransferSize;
			// last link list 일때 interrupt enable 한다.
			if(0 == Number_of_LLI) regvalue |= (INTENB<<31);

			regvalue = (regvalue&~0xfff) | CurTransferSize;

			NX_DMA_Build_LLI( (U32)pSource,
								(U32)pDestination+(LLI_Count*MAXTransferSize),
								(U32)regvalue,
								(U32)CmdBufferAddr+((LLI_Count-1)*Bytes),//(U32)CmdBufferAddr+((LLI_Count-1)*4),
								(U32)Number_of_LLI);
			if(0 == Number_of_LLI)
			{
				break;
			}
			LLI_Count++;
		}
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}
	//m_pRegister->Channel[nChannel].SGLLI.LLI = CmdBufferAddr&~0x3;
	regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration & (0x3<<14);
	regvalue |= (U32)(1
					| (SourcePeriID<<1) // source id
					| (2<<11) // mode
					);
	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

}

void	NX_DMA_TransferMemToIO_Burst( U32 nChannel, const void* pSource, void* pDestination, U32 DestinationPeriID, U32 DestinationBitWidth, NX_DMA_BURST_SIZE DestinationBurstSize, U32 TransferSize )
{
	//register	struct tag_ModuleVariables*		pVariables;
	U32			CmdBufferAddr;
	U32 		regvalue;
	U32 		Number_of_LLI;
	U32 		LLI_Count;
	U32 		CurTransferSize;
	U32 		INTENB;
	U32 		DMA_ModuleIndex = nChannel/8;
	U32 		DMA_ChannelIndex = nChannel%8;
	register struct NX_DMA_RegisterSet *pRegister;
	//register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 8) );
	NX_ASSERT( 0 == (((U32)pDestination) % 2) );

	//NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
	NX_ASSERT( CNULL != __g_ModuleVariables);
//	NX_ASSERT( CNULL != g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
	NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

	pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
//	CmdBufferAddr = g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address + (CHANNELBUFFERSIZE*DMA_ChannelIndex);
    CmdBufferAddr = g_DMA_COMMANDBuffer + (CHANNELBUFFERSIZE*DMA_ChannelIndex);

	//MAXTransferSize = (16*1024)-4;
	U32 MAXTransferSize;
	MAXTransferSize = (16*1024)-4;
	Number_of_LLI = TransferSize/(MAXTransferSize+4);
	if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
	else					CurTransferSize = TransferSize/4;
	
	TransferSize = TransferSize-MAXTransferSize;
	
	INTENB = NX_DMA_GetInterruptEnable(nChannel, 0);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = (U32)pSource;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = (U32)pDestination;

	regvalue = (pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & 0x80000000;

	INTENB = (regvalue>>31) & 1;
	
	regvalue |= (U32)( CurTransferSize		// Transfer size
					| (0<<12) // SBSize (source burst size)
					| (DestinationBurstSize<<15) // DBSize (destination burst size)
					| (2<<18) // SWidth (source transfer width)
					| ((DestinationBitWidth>>4)<<21) // DWidth (destination transfer width)
					| (0UL<<24) // source master bus 0: AHB1, 1; AHB:2
					| (1UL<<25) // destination master bus
					| (1UL<<26) // SI (source increment)
					| (0UL<<27)	// DI (destination increment)
					);
	// tranfer size 가 16Kbyte-4byte 보다 클경우 LLI 의 마지막 에서 interrupt enable 한다.
	if (0 != Number_of_LLI)	regvalue = regvalue & ~(1UL<<31);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = 0x0;
	if (0 != Number_of_LLI)
	{
		LLI_Count = 1;
		while(1)
		{
			Number_of_LLI = TransferSize/(MAXTransferSize+4);
			if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/4;
			else					CurTransferSize = TransferSize/4;

			TransferSize = TransferSize-MAXTransferSize;
			// last link list 일때 interrupt enable 한다.
			if(0 == Number_of_LLI) regvalue |= (INTENB<<31);

			regvalue = (regvalue&~0xfff) | CurTransferSize;

			NX_DMA_Build_LLI( (U32)pSource+(LLI_Count*MAXTransferSize),
								(U32)pDestination,
								(U32)regvalue,
								(U32)CmdBufferAddr+((LLI_Count-1)*4),
								(U32)Number_of_LLI);
			if(0 == Number_of_LLI)
			{
				break;
			}
			LLI_Count++;
		}
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}
	regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration & (0x3<<14);
	regvalue |= (U32)(1
					| (DestinationPeriID<<6) // destination id
					| (1<<11) // mode
					);
	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

}

void	NX_DMA_TransferIOToMem_Burst( U32 nChannel, const void* pSource, U32 SourcePeriID, U32 SourceBitWidth, NX_DMA_BURST_SIZE SourceBurstSize, void* pDestination, U32 TransferSize )
{
	//register	struct tag_ModuleVariables*		pVariables;
	U32			CmdBufferAddr;
	U32 		regvalue;
	U32 		Number_of_LLI;
	U32 		LLI_Count;
	U32 		CurTransferSize;
	U32			INTENB;
	U32 		DMA_ModuleIndex = nChannel/8;
	U32 		DMA_ChannelIndex = nChannel%8;
	register struct NX_DMA_RegisterSet *pRegister;
	//register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 2) );
	NX_ASSERT( 0 == (((U32)pDestination) % 8) );
    //MES_REQUIRE ( 64 > SourcePeriID );
    //MES_REQUIRE ( 8 == SourceBitWidth || 16 == SourceBitWidth || 32 == SourceBitWidth );

	NX_ASSERT( CNULL != __g_ModuleVariables);
//	NX_ASSERT( CNULL != g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address );
    NX_ASSERT( CNULL != g_DMA_COMMANDBuffer );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
	NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

	pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
//	CmdBufferAddr = g_DMA_COMMANDBuffer[DMA_ModuleIndex].Address + (CHANNELBUFFERSIZE*DMA_ChannelIndex);
    CmdBufferAddr = g_DMA_COMMANDBuffer + (CHANNELBUFFERSIZE*DMA_ChannelIndex);

	//MAXTransferSize = (16*1024)-4;
	U32 Bytes = SourceBitWidth/8;
	U32 MAXTransferSize;
    if (8 == SourceBitWidth)		MAXTransferSize = (4*1024)-Bytes;
    else if (16 == SourceBitWidth)	MAXTransferSize = (8*1024)-Bytes;
    else							MAXTransferSize = (16*1024)-Bytes;
	//MAXTransferSize = (16*1024)-4;
	Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
	if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/Bytes;
	else					CurTransferSize = TransferSize/Bytes;
	
	TransferSize = TransferSize-MAXTransferSize;
	
	INTENB = NX_DMA_GetInterruptEnable(nChannel, 0);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.SRCADDR = (U32)pSource;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.DSTADDR = (U32)pDestination;

	regvalue = (pRegister->Channel[DMA_ChannelIndex].SGLLI.Control) & 0x80000000;

	INTENB = (regvalue>>31) & 1;
	
	regvalue |= (U32)( (CurTransferSize&0xFFF)		// Transfer size
					| (SourceBurstSize<<12) // SBSize (source burst size)
					| (0<<15) // DBSize (destination burst size)
					| ((SourceBitWidth>>4)<<18) // SWidth (source transfer width)
					| (2<<21) // DWidth (destination transfer width)
					| (1UL<<24) // source master bus 0: AHB1, 1; AHB:2
					| (0UL<<25) // destination master bus
					| (0UL<<26) // SI (source increment)
					| (1UL<<27)	// DI (destination increment)
					);
	if (0 != Number_of_LLI)	regvalue = regvalue & ~(1UL<<31);

	pRegister->Channel[DMA_ChannelIndex].SGLLI.Control = regvalue;
	pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = 0x0;
	if (0 != Number_of_LLI)
	{
		LLI_Count = 1;
		while(1)
		{
			Number_of_LLI = TransferSize/(MAXTransferSize+Bytes);
			if (0 != Number_of_LLI)	CurTransferSize = MAXTransferSize/Bytes;
			else					CurTransferSize = TransferSize/Bytes;

			TransferSize = TransferSize-MAXTransferSize;
			// last link list 일때 interrupt enable 한다.
			if(0 == Number_of_LLI) regvalue |= (INTENB<<31);

			regvalue = (regvalue&~0xfff) | CurTransferSize;

			NX_DMA_Build_LLI( (U32)pSource,
								(U32)pDestination+(LLI_Count*MAXTransferSize),
								(U32)regvalue,
								(U32)CmdBufferAddr+((LLI_Count-1)*Bytes),//(U32)CmdBufferAddr+((LLI_Count-1)*4),
								(U32)Number_of_LLI);
			if(0 == Number_of_LLI)
			{
				break;
			}
			LLI_Count++;
		}
		pRegister->Channel[DMA_ChannelIndex].SGLLI.LLI = CmdBufferAddr&~0x3;
	}
	//m_pRegister->Channel[nChannel].SGLLI.LLI = CmdBufferAddr&~0x3;
	regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration & (0x3<<14);
	regvalue |= (U32)(1
					| (SourcePeriID<<1) // source id
					| (2<<11) // mode
					);
	pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;

}

CBOOL	NX_DMA_Build_LLI( U32 pSource, U32 pDestination, U32 ControlReg, U32 LLI_ADDR, U32 NextLLI)
{
	if(0 != NextLLI)
	{
		*(volatile U32*)(LLI_ADDR+0x0) = (U32)(pSource );
		*(volatile U32*)(LLI_ADDR+0x4) = (U32)(pDestination );
		*(volatile U32*)(LLI_ADDR+0x8) = (U32)(LLI_ADDR+4 );
		*(volatile U32*)(LLI_ADDR+0xC) = (U32)(ControlReg );
	}
	else
	{
		*(volatile U32*)(LLI_ADDR+0x0) =(U32)(pSource );
		*(volatile U32*)(LLI_ADDR+0x4) =(U32)(pDestination );
		*(volatile U32*)(LLI_ADDR+0x8) =(U32)(0x0 );
		*(volatile U32*)(LLI_ADDR+0xC) =(U32)(ControlReg );
	}
}


//------------------------------------------------------------------------------
/**
 *  @brief      Run DMA transfer.
 *  @param[in]  ModuleIndex     an index of module.
 *  @return     None.
 *  @remarks    DMA have different sequence by DMA operation mode ( Wirte Back or Write Through ). \n
 *  @code
 *              //--------------------------------------------------------------
 *              // Write Back Mode Operation
 *              //--------------------------------------------------------------
 *
 *              NX_DMA_SetCommandBufferMode( CTRUE );               // DMA set Write Back Mode
 *              ...                                                 // DMA configuration
 *
 *              while( CFALSE == NX_DMA_IsCommandBufferReady() )    // Check command buffer
 *              {
 *                  CNULL;
 *              }
 *              NX_DMA_Run();                                       // DMA run
 *
 *              //--------------------------------------------------------------
 *              // Write Through Mode Operation
 *              //--------------------------------------------------------------
 *
 *              NX_DMA_SetCommandBufferMode( CFALSE );              // DMA set Write Through Mode
 *              ...
 *
 *              NX_DMA_Run();                                       // DMA run

 *  @endcode
 *  @see        NX_DMA_SetCommandBufferMode,    NX_DMA_GetCommandBufferMode,
 *                                              NX_DMA_CheckRunning,
 *              NX_DMA_Stop,                    NX_DMA_CommandBufferFlush,
 *              NX_DMA_IsCommandBufferFlush,    NX_DMA_IsCommandBufferReady,
 *              NX_DMA_IsCommandBufferEmpty,    NX_DMA_GetCommandBufferFreeCount
 */
void    NX_DMA_Run( U32 nChannel )
{
    register    struct  NX_DMA_RegisterSet      *pRegister;
    U32 regvalue;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
    regvalue = pRegister->Channel[DMA_ChannelIndex].Configuration;
    regvalue |= 1;
    pRegister->Channel[DMA_ChannelIndex].Configuration = regvalue;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Check whether DMA transfer is running or not.
 *  @param[in]  ModuleIndex     an index of module.
 *  @return     \b CTURE    indicates DMA transfer is running.\n
 *              \b CFALSE   indicates DMA transfer is idle.
 *  @see    NX_DMA_SetCommandBufferMode,    NX_DMA_GetCommandBufferMode,
 *          NX_DMA_Run,
 *          NX_DMA_Stop,                    NX_DMA_CommandBufferFlush,
 *          NX_DMA_IsCommandBufferFlush,    NX_DMA_IsCommandBufferReady,
 *          NX_DMA_IsCommandBufferEmpty,    NX_DMA_GetCommandBufferFreeCount
 */
U32 NX_DMA_CheckRunning ( U32 nChannel )
{
    register    struct  NX_DMA_RegisterSet      *pRegister;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
    return (U32)((ReadIO32(&pRegister->EnbldChns) & 0xFF) & 1UL<<DMA_ChannelIndex);
    //return (CBOOL)((pRegister->CSR_CPC[ModuleIndex*2] & 0xF) != 0x0);
}


//------------------------------------------------------------------------------
/**
 *  @brief      Stop/Cancel DMA Transfer.
 *  @param[in]  ModuleIndex     an index of module.
 *  @param      Enable      \b CTRUE    indicate that DMA Stop is Enable. \n
 *                          \b CFALSE   indicate that DMA Stop is Disable.
 *  @return     None.
 *  @remark     If DMA running write back mode, then user have to clear command buffer.\n
 *              Also, user should check command buffer flush is completed or not.\n \n
 *              After NX_DMA_Stop( CTRUE ), user should check DMA running status. \n
 *              If DMA is really stopped, user should setting NX_DMA_Stop( CFALSE ).
 *  @code
 *              if( NX_DMA_GetCommandBufferMode() )
 *              {
 *                  NX_DMA_CommandBufferFlush();                        // Command Buffer Flush
 *                  while(  CTRUE != NX_DMA_IsCommandBufferFlush() )    // wait during command buffer flushing
 *                  { CNULL; }
 *              }
 *
 *              NX_DMA_Stop(CTRUE);
 *              while( NX_DMA_CheckRunning() )
 *              {
 *                  CNULL;                                              // wait during DMA stop
 *              }
 *              NX_DMA_Stop(CFALSE);
 *  @endcode
 *  @see        NX_DMA_SetCommandBufferMode,    NX_DMA_GetCommandBufferMode,
 *              NX_DMA_Run,                     NX_DMA_CheckRunning,
 *                                              NX_DMA_CommandBufferFlush,
 *              NX_DMA_IsCommandBufferFlush,    NX_DMA_IsCommandBufferReady,
 *              NX_DMA_IsCommandBufferEmpty,    NX_DMA_GetCommandBufferFreeCount
 */
void    NX_DMA_Stop ( U32 nChannel, CBOOL Enable )
{
    const U32 Enb   = (1UL<<0);
    const U32 Holt  = (1UL<<18);
    register    struct  NX_DMA_RegisterSet      *pRegister;
    U32 regvalue;
    U32         DMA_ModuleIndex = nChannel/8;
    U32         DMA_ChannelIndex = nChannel%8;

    //NX_ASSERT( NUMBER_OF_DMA_CHANNEL > nChannel );
    NX_ASSERT( CNULL != __g_ModuleVariables );
    NX_ASSERT( NUMBER_OF_DMA_MODULE > DMA_ModuleIndex );
    NX_ASSERT( NUMBER_OF_DMA_CHANNEL > DMA_ChannelIndex );

    pRegister = __g_ModuleVariables[DMA_ModuleIndex].pRegister;
    regvalue = ReadIO32(&pRegister->Channel[DMA_ChannelIndex].Configuration);

    if( Enable )
    {
        regvalue  &= ~Enb;
    }
    else
    {
        regvalue  |= Holt;
    }

    WriteIO32(&pRegister->Channel[DMA_ChannelIndex].Configuration, regvalue);
}

//-----------------------------------------------------------------------------------------------
static	CBOOL		__NX_DMA_State[NUMBER_OF_DMA_MODULE*NUMBER_OF_DMA_CHANNEL] = { CFALSE, };

void   NX_DMA_SetUnLockChannel( U32 PeriID ) 
{
	U32 	DMA_ModuleIndex;
	U32		DMA_ChannelIndex;
	
	DMA_ModuleIndex  = PeriID / 16;
	DMA_ChannelIndex = PeriID % 16;
		
	__NX_DMA_State[(DMA_ModuleIndex * NUMBER_OF_DMA_CHANNEL) + DMA_ChannelIndex] = CFALSE;

}

int		NX_DMA_GetUnLockChannel( U32 PeriID ) 
{
	U32 	ChannelIdx;
	U32 	DMA_ModuleIndex;
	
	DMA_ModuleIndex = PeriID / 16;
	
	for( ChannelIdx = 0 ; ChannelIdx < NUMBER_OF_DMA_CHANNEL; ChannelIdx++ ) 
	{
		if( __NX_DMA_State[ChannelIdx + (DMA_ModuleIndex * NUMBER_OF_DMA_CHANNEL)] == CFALSE )
		{
			__NX_DMA_State[ChannelIdx + DMA_ModuleIndex * NUMBER_OF_DMA_CHANNEL] = CTRUE;
			return (ChannelIdx + DMA_ModuleIndex * NUMBER_OF_DMA_CHANNEL);
		}
	}
	
	return -1;
}


