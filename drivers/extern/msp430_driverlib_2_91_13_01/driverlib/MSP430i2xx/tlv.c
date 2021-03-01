//*****************************************************************************
//
// tlv.c - Driver for the tlv Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup tlv_api tlv
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_TLV__
#include "tlv.h"

#include <assert.h>

void TLV_getInfo(uint8_t tag, uint8_t *length, uint16_t **data_address) {

    // Get access type
    *length = (tag & 0xC0) >> 5;
    if(*length == 0) {
        *length = 1;
    }

    *data_address = ((uint16_t *)((uint16_t)(TLV_START + (tag & 0x3F))));

    return;
}

bool TLV_performChecksumCheck(void) {

    uint16_t checksum = 0;
    uint16_t *tlv_address = ((uint16_t *)TLV_START) + TLV_CHKSUM_L + 1;
    uint16_t *tlv_end = (uint16_t *) TLV_END;

    do {
        checksum ^= *tlv_address++;
    } while(tlv_address <= tlv_end);

    checksum ^= 0xFFFF;
    checksum++;

    if(checksum == HWREG16(TLV_START + TLV_CHKSUM_L)) {
        return true;
    } else {
        return false;
    }
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for tlv_api
//! @}
//
//*****************************************************************************
