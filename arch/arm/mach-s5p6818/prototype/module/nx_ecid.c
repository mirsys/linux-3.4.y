//------------------------------------------------------------------------------
//
//	Copyright (C) Nexell Co. 2012
//
//  This confidential and proprietary software may be used only as authorized by a
//  licensing agreement from Nexell Co.
//  The entire notice above must be reproduced on all authorized copies and copies
//  may only be made to the extent permitted by a licensing agreement from Nexell Co.
//
//	Module		: ECID
//	File			: nx_ecid.c
//	Description	:
//	Author		: Sei
//	History		: 2014.10.16 Revision Comments. (Deoks)
//------------------------------------------------------------------------------
#include <nx_chip.h>
#include <nx_ecid.h>

static struct
{
    struct NX_ECID_RegisterSet * pRegister;
} __g_ModuleVariables = {CNULL, };

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return	CTRUE		indicate that Initialize is successed.
 *			CFALSE		indicate that Initialize is failed.
 */
CBOOL	NX_ECID_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_ModuleVariables.pRegister = CNULL;
		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 */
U32     NX_ECID_GetNumberOfModule( void )
{
    return  NUMBER_OF_ECID_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@return		Module's physical address
 */
U32		NX_ECID_GetPhysicalAddress( void )
{
	return  (U32)PHY_BASEADDR_ECID_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 */
U32		NX_ECID_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_ECID_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ). 
 *	@param[in]	BaseAddress 	Module's base address
 *	@return		None.
 */
void	NX_ECID_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_ModuleVariables.pRegister = (struct NX_ECID_RegisterSet *) BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 */
U32		NX_ECID_GetBaseAddress( void )
{
	return (U32)__g_ModuleVariables.pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ). 
 *	@return		CTRUE			indicate that Initialize is successed.
 *				CFALSE			indicate that Initialize is failed.
 */
CBOOL	NX_ECID_OpenModule( void )
{
    NX_ASSERT( __g_ModuleVariables.pRegister != CNULL );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).
 *	@return		CTRUE			indicate that Deinitialize is successed.
 *				CFALSE			indicate that Deinitialize is failed.
 */
CBOOL	NX_ECID_CloseModule( void )
{
    NX_ASSERT( __g_ModuleVariables.pRegister != CNULL );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *   				this function only can use after transaction .
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).  
 *	@return		CTRUE			indicate that Module is Busy.
 *				CFALSE			indicate that Module is NOT Busy.
 */
CBOOL	NX_ECID_CheckBusy( void )
{
	return CFALSE;
}

CBOOL	NX_ECID_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Colck & Reset Control
//------------------------------------------------------------------------------
#if 0
U32     NX_ECID_GetClockNumber( void )
{
    static const U32 CLKGEN_ECID_LIST[] =
    {
        CLOCKINDEX_LIST( ECID )
    };

    return (U32)CLKGEN_ECID_LIST[0];
}
#endif

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *   				this function only can use after transaction .
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ x ).  
 *	@return		CTRUE			indicate that Module is Busy.
 *				CFALSE			indicate that Module is NOT Busy.
 */
U32     NX_ECID_GetResetNumber( void )
{
    const U32 RSTCON_ECID_LIST[] =
    {
        RESETINDEX_LIST( ECID, i_nRST )
    };

    return (U32)RSTCON_ECID_LIST[0];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ECID (Electronic Chip ID )
 *	@param[in]	ECID[4]		Get ECID Value 
 *	@return		None.
 */
void NX_ECID_GetECID( U32 ECID[4] )
{
    NX_ASSERT( __g_ModuleVariables.pRegister != CNULL );

	ECID[0] = __g_ModuleVariables.pRegister->ECID[0];
	ECID[1] = __g_ModuleVariables.pRegister->ECID[1];
	ECID[2] = __g_ModuleVariables.pRegister->ECID[2];
	ECID[3] = __g_ModuleVariables.pRegister->ECID[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Name of chip
 *	@param[in]	ChipName	Get Chip Name.  (49Byte)
 *	@return		None.
 *	@remark	GUID is created by Microsoft's uuidgen.exe. It's 128bit integer.
 */
void NX_ECID_GetChipName( U8 ChipName[49] )
{
	U32 i;
	for( i = 0 ; i < 48 ; i ++ )
	{
		ChipName[i] = ReadIO32(&__g_ModuleVariables.pRegister->CHIPNAME[i]);
	}
	ChipName[i] = 0;
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get GUID of chip
  *	@param[in]	guid		Get GUID
  *	@return		None.
  *	@remark	GUID is created by Microsoft's uuidgen.exe. It's 128bit integer.
  */
void NX_ECID_GetGUID( NX_GUID * GUID )
{
	GUID->GUID0     = ReadIO32(&__g_ModuleVariables.pRegister->GUID0);
	GUID->GUID1     = ReadIO32(&__g_ModuleVariables.pRegister->GUID1);
	GUID->GUID2     = ReadIO32(&__g_ModuleVariables.pRegister->GUID2);
	GUID->GUID3[0]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[0]);
	GUID->GUID3[1]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[1]);
	GUID->GUID3[2]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[2]);
	GUID->GUID3[3]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[3]);
	GUID->GUID3[4]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[4]);
	GUID->GUID3[5]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[5]);
	GUID->GUID3[6]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[6]);
	GUID->GUID3[7]  = ReadIO32(&__g_ModuleVariables.pRegister->GUID3[7]);
}

//------------------------------------------------------------------------------
/// @brief		Get BoundingID of chip
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID Address x Signal. 
  *	@param[in]	Addr	Program Address Signal.
  *	@return		None.
  */
void    NX_ECID_SetA( U32 Addr )
{
    const U32   A_POS   = 7;
    const U32   A_MASK  = 7UL << A_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( Addr >= 0 && Addr < 8 );

    ReadValue   = ReadIO32(&pRegister->EC[0]);
    ReadValue  &= ~A_MASK;
    ReadValue  |= Addr << A_POS;

    WriteIO32(&pRegister->EC[0], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID Address x Signal. 
  *	@return		Address Signal. (0:Low, 1:Hight)
  */
U32     NX_ECID_GetA( void )
{
    const U32   A_POS   = 7;
    const U32   A_MASK  = 7UL << A_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[0]) & A_MASK) >> A_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID Chip Select Signal.
  *	@param[in]	Enable	Program CS Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetCS( CBOOL Enable )
{
    const U32   CS_POS  = 6;
    const U32   CS_MASK = 1UL << CS_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[0]);
    ReadValue  &= ~CS_MASK;
    ReadValue  |= Enable << CS_POS;

    WriteIO32(&pRegister->EC[0], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID Chip Select Signal. 
  *	@return		
  CS Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetCS( void )
{
    const U32   CS_POS  = 6;
    const U32   CS_MASK = 1UL << CS_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
	
    return (CBOOL)((ReadIO32(&pRegister->EC[0]) & CS_MASK) >> CS_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID SIGDEV Signal.
  *	@param[in]	Enable	Program SIGDEV Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetSIGDEV( CBOOL Enable )
{
    const U32   SIGDEV_POS  = 5;
    const U32   SIGDEV_MASK = 1UL << SIGDEV_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[0]);
    ReadValue  &= ~SIGDEV_MASK;
    ReadValue  |= Enable << SIGDEV_POS;

    WriteIO32(&pRegister->EC[0], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID SIGDEV Signal. 
  *	@return		SIGDEV Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetSIGDEV( void )
{
    const U32   SIGDEV_POS  = 5;
    const U32   SIGDEV_MASK = 1UL << SIGDEV_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[0]) & SIGDEV_MASK) >> SIGDEV_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID FSET Signal.
  *	@param[in]	Enable	Program FSET Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetFSET( CBOOL Enable )
{
    const U32   FSET_POS    = 4;
    const U32   FSET_MASK   = 1UL << FSET_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[0]);
    ReadValue  &= ~FSET_MASK;
    ReadValue  |= Enable << FSET_POS;

    WriteIO32(&pRegister->EC[0], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID SIGDEV Signal. 
  *	@return		SIGDEV Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetFSET( void )
{
    const U32   FSET_POS    = 5;
    const U32   FSET_MASK   = 1UL << FSET_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[0]) & FSET_MASK) >> FSET_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID PRCHG Signal.
  *	@param[in]	Enable	Program PRCHG Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetPRCHG( CBOOL Enable )
{
    const U32   PRCHG_POS   = 3;
    const U32   PRCHG_MASK  = 1UL << PRCHG_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[0]);
    ReadValue  &= ~PRCHG_MASK;
    ReadValue  |= Enable << PRCHG_POS;

    WriteIO32(&pRegister->EC[0], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID PRCHG Signal. 
  *	@return		PRCHG Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetPRCHG( void )
{
    const U32   PRCHG_POS   = 3;
    const U32   PRCHG_MASK  = 1UL << PRCHG_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[0]) & PRCHG_MASK) >> PRCHG_POS);
}


//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID PROG Signal.
  *	@param[in]	Enable	Program PROG Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetPROG( CBOOL Enable )
{
    const U32   PROG_POS    = 2;
    const U32   PROG_MASK   = 1UL << PROG_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[1]);
    ReadValue  &= ~PROG_MASK;
    ReadValue  |= Enable << PROG_POS;

    WriteIO32(&pRegister->EC[1], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID PROG Signal. 
  *	@return		PROG Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetPROG( void )
{
    const U32   PROG_POS    = 2;
    const U32   PROG_MASK   = 1UL << PROG_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (CBOOL)((ReadIO32(&pRegister->EC[1]) & PROG_MASK) >> PROG_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID SCK Signal.
  *	@param[in]	Enable	Program SCK Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetSCK( CBOOL Enable )
{
    const U32   SCK_POS  = 1;
    const U32   SCK_MASK = 1UL << SCK_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[1]);
    ReadValue  &= ~SCK_MASK;
    ReadValue  |= Enable << SCK_POS;

    WriteIO32(&pRegister->EC[1], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID SCK Signal. 
  *	@return		SCK Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetSCK( void )
{
    const U32   SCK_POS  = 1;
    const U32   SCK_MASK = 1UL << SCK_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (CBOOL)((ReadIO32(&pRegister->EC[1]) & SCK_MASK) >> SCK_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID SDI Signal.
  *	@param[in]	Enable	Program SDI Signal. ( 0 : Low, 1: High )
  *	@return		None.  
  */
void    NX_ECID_SetSDI( CBOOL Enable )
{
    const U32   SDI_POS  = 0;
    const U32   SDI_MASK = 1UL << SDI_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[1]);
    ReadValue  &= ~SDI_MASK;
    ReadValue  |= Enable << SDI_POS;

    WriteIO32(&pRegister->EC[1], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID SDI Signal. 
  *	@return		SDI Signal. ( 0 : Low, 1: High )
  */
CBOOL   NX_ECID_GetSDI( void )
{
    const U32   SDI_POS  = 0;
    const U32   SDI_MASK = 1UL << SDI_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (CBOOL)((ReadIO32(&pRegister->EC[1]) & SDI_MASK) >> SDI_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID Key Ready. 
  *	@return		HardWare Done. ( 0 : Not Ready, 1: Ready(Done) )
  */
CBOOL   NX_ECID_GetKeyReady( void )
{
    const U32   READY_POS   = 15;
    const U32   READY_MASK  = 1UL << READY_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (CBOOL)((ReadIO32(&pRegister->EC[2]) & READY_MASK)>>READY_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set HDCP.
  *	@param[in]	Enable	HDCP EFUSE Select. ( 0 : Secure Boot, 1: Secure JTAG )
  *	@return		None.    
  */
void    NX_ECID_SetHdcpEfuseSel( CBOOL Enable )
{
    const U32   SHES_POS  = 4;
    const U32   SHES_MASK = 1UL << SHES_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( (0==Enable) ||(1==Enable) );

    ReadValue   = ReadIO32(&pRegister->EC[2]);
    ReadValue  &= ~SHES_MASK;
    ReadValue  |= Enable << SHES_POS;

    WriteIO32(&pRegister->EC[2], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get HDCP.
  *	@return		Get HDCP EFUSE Select. ( 0 : Not Ready, 1: Ready(Done) )
  */
CBOOL   NX_ECID_GetHdcpEfuseSel( void )
{
    const U32   SHES_POS  = 4;
    const U32   SHES_MASK = 1UL << SHES_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (CBOOL)((ReadIO32(&pRegister->EC[2]) & SHES_MASK) >> SHES_POS);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Set eFuse Select.
  *	@param[in]	Sel	EFUSE Select. ( 0 : ECID, 1: Secure Boot, 2: Secure JTAG, 3:Backdoor JTAG )
  *	@return		None.    
  */
void    NX_ECID_SetSelectFlowingBank( U32 Sel )
{
    const U32   SELBANK_POS     = 0;
    const U32   SELBANK_MASK    = 3UL << SELBANK_POS;

    register struct NX_ECID_RegisterSet* pRegister;
    register U32 ReadValue;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );
    NX_ASSERT( Sel >= 0 && Sel < 4 );

    ReadValue   = ReadIO32(&pRegister->EC[2]);
    ReadValue  &= ~SELBANK_MASK;
    ReadValue  |= Sel << SELBANK_POS;

    WriteIO32(&pRegister->EC[2], ReadValue);
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get eFuse Select.
  *	@return		Sel	EFUSE Select. ( 0 : ECID, 1: Secure Boot, 2: Secure JTAG, 3:Backdoor JTAG ) 
  */
U32     NX_ECID_GetSelectFlowingBank( void )
{
    const U32   SELBANK_POS     = 0;
    const U32   SELBANK_MASK    = 3UL << SELBANK_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[2]) & SELBANK_MASK) >> SELBANK_POS);
}


//------------------------------------------------------------------------------
/* 
  *	@brief		Set ECID Bonding ID.
  *	@param[in]	Bonding ID. . ( 0 : ECID, 1: Secure Boot, 2: Secure JTAG, 3:Backdoor JTAG )
  *	@return		None.    
  */
void NX_ECID_SetBONDINGID
(
    CBOOL set_cs, CBOOL set_sigdev, CBOOL set_fset, CBOOL set_prchg
)
{
    NX_ASSERT( CNULL != __g_ModuleVariables.pRegister );
    register U32 Enable;
    Enable = (U32)( (set_cs<<6) | (set_sigdev<<5) | (set_fset<<4) | (set_prchg<<3) );
    WriteIO32(&__g_ModuleVariables.pRegister->EC[0], (U32)(Enable&0x0078));
}

//------------------------------------------------------------------------------
/* 
  *	@brief		Get ECID Bonding ID.
  *	@return		Bonding ID. 
  */
U32     NX_ECID_GetBONDINGID( void )
{
    const U32   BOND_POS   = 0;
    const U32   BOND_MASK  = 7UL << BOND_POS;

    register struct NX_ECID_RegisterSet* pRegister;

    pRegister   = __g_ModuleVariables.pRegister;
    NX_ASSERT( CNULL != pRegister );

    return (U32)((ReadIO32(&pRegister->EC[0]) & BOND_MASK) >> BOND_POS);
}


