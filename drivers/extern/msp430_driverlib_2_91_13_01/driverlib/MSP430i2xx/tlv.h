//*****************************************************************************
//
// tlv.h - Driver for the TLV Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_TLV_H__
#define __MSP430WARE_TLV_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_TLV__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The following are values that can be passed to the tag parameter for
// functions: TLV_getInfo().
//
//*****************************************************************************
#define TLV_CHECKSUM                                      (0x40 | TLV_CHKSUM_L)
#define TLV_TAG_DIE_RECORD                                 (TLV_DIE_RECORD_TAG)
#define TLV_LENGTH_DIE_RECORD                              (TLV_DIE_RECORD_LEN)
#define TLV_WAFER_LOT_ID                              (0x80 | TLV_LOT_WAFER_ID)
#define TLV_DIE_X_POSITION                               (0x40 | TLV_DIE_X_POS)
#define TLV_DIE_Y_POSITION                               (0x40 | TLV_DIE_Y_POS)
#define TLV_TEST_RESULTS                          (0x40 | TLV_DIE_TEST_RESULTS)
#define TLV_REF_CALIBRATION_TAG                                   (TLV_REF_TAG)
#define TLV_REF_CALIBRATION_LENGTH                                (TLV_REF_LEN)
#define TLV_REF_CALIBRATION_REFCAL1                           (TLV_CAL_REFCAL1)
#define TLV_REF_CALIBRATION_REFCAL0                           (TLV_CAL_REFCAL0)
#define TLV_DCO_CALIBRATION_TAG                                   (TLV_DCO_TAG)
#define TLV_DCO_CALIBRATION_LENGTH                                (TLV_DCO_LEN)
#define TLV_DCO_CALIBRATION_CSIRFCAL                         (TLV_CAL_CSIRFCAL)
#define TLV_DCO_CALIBRATION_CSIRTCAL                         (TLV_CAL_CSIRTCAL)
#define TLV_DCO_CALIBRATION_CSERFCAL                         (TLV_CAL_CSERFCAL)
#define TLV_DCO_CALIBRATION_CSERTCAL                         (TLV_CAL_CSERTCAL)
#define TLV_SD24_CALIBRATION_TAG                                 (TLV_SD24_TAG)
#define TLV_SD24_CALIBRATION_LENGTH                              (TLV_SD24_LEN)
#define TLV_SD24_CALIBRATION_SD24TRIM                        (TLV_CAL_SD24TRIM)

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Gets TLV Info
//!
//! The TLV structure uses a tag or base address to identify segments of the
//! table where information is stored. This can be used to retrieve calibration
//! constants for the device or find out more information about the device.
//! This function retrieves the address of a tag and the length of the tag
//! request. Please check the device datasheet for tags available on your
//! device.
//!
//! \param tag represents the tag for which the information needs to be
//!        retrieved.
//!        Valid values are:
//!        - \b TLV_CHECKSUM
//!        - \b TLV_TAG_DIE_RECORD
//!        - \b TLV_LENGTH_DIE_RECORD
//!        - \b TLV_WAFER_LOT_ID
//!        - \b TLV_DIE_X_POSITION
//!        - \b TLV_DIE_Y_POSITION
//!        - \b TLV_TEST_RESULTS
//!        - \b TLV_REF_CALIBRATION_TAG
//!        - \b TLV_REF_CALIBRATION_LENGTH
//!        - \b TLV_REF_CALIBRATION_REFCAL1
//!        - \b TLV_REF_CALIBRATION_REFCAL0
//!        - \b TLV_DCO_CALIBRATION_TAG
//!        - \b TLV_DCO_CALIBRATION_LENGTH
//!        - \b TLV_DCO_CALIBRATION_CSIRFCAL
//!        - \b TLV_DCO_CALIBRATION_CSIRTCAL
//!        - \b TLV_DCO_CALIBRATION_CSERFCAL
//!        - \b TLV_DCO_CALIBRATION_CSERTCAL
//!        - \b TLV_SD24_CALIBRATION_TAG
//!        - \b TLV_SD24_CALIBRATION_LENGTH
//!        - \b TLV_SD24_CALIBRATION_SD24TRIM
//! \param length Acts as a return through indirect reference. The function
//!        retrieves the value of the TLV tag length. This value is pointed to
//!        by *length and can be used by the application level once the
//!        function is called.
//! \param data_address acts as a return through indirect reference. Once the
//!        function is called data_address points to the pointer that holds the
//!        value retrieved from the specified TLV tag.
//!
//! \return None
//
//*****************************************************************************
extern void TLV_getInfo(uint8_t tag,
                        uint8_t *length,
                        uint16_t **data_address);

//*****************************************************************************
//
//! \brief Performs checksum check on TLV
//!
//! The 2's complement checksum is calculated on the data stored in the TLV. If
//! the calculated checksum is equal to the checksum stored in the TLV then the
//! user knows that the TLV has not been corrupted. This API can be used after
//! a BOR before writing configuration constants to the appropriate registers.
//!
//!
//! \return true if checksum of TLV matches value stored in TLV, false
//!         otherwise
//
//*****************************************************************************
extern bool TLV_performChecksumCheck(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_TLV_H__
