//*****************************************************************************
//
// mpy.c - Driver for the mpy Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup mpy_api mpy
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_MPY__
#include "mpy.h"

#include <assert.h>

void MPY_setOperandOne8Bit (uint8_t multiplicationType,
    uint8_t operand)
{
    HWREG8(MPY_BASE + OFS_MPY + multiplicationType) = operand;
}

void MPY_setOperandOne16Bit (uint8_t multiplicationType,
    uint16_t operand)
{
    HWREG16(MPY_BASE + OFS_MPY + multiplicationType) = operand;
}

void MPY_setOperandTwo8Bit (uint8_t operand)
{
    HWREG8(MPY_BASE + OFS_OP2) = operand;
}

void MPY_setOperandTwo16Bit (uint16_t operand)
{
    HWREG16(MPY_BASE + OFS_OP2) = operand;
}

uint32_t MPY_getResult(void)
{
    uint32_t result;
    result = HWREG16(MPY_BASE + OFS_RESLO);
    result += ((uint32_t) HWREG16(MPY_BASE + OFS_RESHI) << 16);
    return result;
}

uint16_t MPY_getSumExtension (void)
{
    return ( HWREG16(MPY_BASE + OFS_SUMEXT) );
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for mpy_api
//! @}
//
//*****************************************************************************
