/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */
/**************************************************************************//**
 * @ingroup	 SAM3S_ACC
 * @file
 * @brief    Implementation of Analog Comparator Controller (ACC).
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <tmos.h>
#include "platform_cpp.h"


/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Initialize the ACC controller
 *
 * \param pAcc Pointer to an Acc instance.
 * \param idAcc ACC identifier
 * \param ucSelplus input connected to inp, 0~7
 * \param ucSelminus input connected to inm,0~7
 * \param wAc_en Analog comprator enabled/disabled
 * \param wEdge CF flag triggering mode
 * \param wInvert INVert comparator output,use pattern defined in the device header file
 */
extern void ACC_Configure( Acc *pAcc, uint8_t idAcc, uint8_t ucSelplus, uint8_t ucSelminus,
                           uint16_t wAc_en, uint16_t wEdge, uint16_t wInvert )
{
    /* Enable peripheral clock*/
    PMC->PMC_PCER1 = 1 << (idAcc - 32) ;

    /*  Reset the controller */
    pAcc->ACC_CR |= ACC_CR_SWRST ;

    /*  Write to the MR register */
    ACC_CfgModeReg( pAcc,
                    ( (ucSelplus<<ACC_MR_SELPLUS_Pos) & ACC_MR_SELPLUS_Msk ) |
                    ( (ucSelminus<<ACC_MR_SELMINUS_Pos) & ACC_MR_SELMINUS_Msk ) |
                    ( (wAc_en<<8) & ACC_MR_ACEN ) |
                    ( (wEdge<<ACC_MR_EDGETYP_Pos) & ACC_MR_EDGETYP_Msk ) |
                    ( (wInvert<<12) & ACC_MR_INV ) ) ;
    /* set hysteresis and current option*/
    pAcc->ACC_ACR = (ACC_ACR_ISEL_HISP | ((0x01 << ACC_ACR_HYST_Pos) & ACC_ACR_HYST_Msk));

    /* Automatic Output Masking Period*/
    while ( pAcc->ACC_ISR & (uint32_t)ACC_ISR_MASK ) ;
}

/**
 * Return the Channel Converted Data
 * \param pAcc Pointer to an Acc instance.
 * \param ucSelplus input applied on ACC SELPLUS
 * \param ucSelminus input applied on ACC SELMINUS
 */
extern void ACC_SetComparisionPair( Acc *pAcc, uint8_t ucSelplus, uint8_t ucSelminus )
{
    uint32_t dwTemp ;

    ASSERT( ucSelplus < 8 && ucSelminus < 8 ) ;

    dwTemp = pAcc->ACC_MR ;

    pAcc->ACC_MR = dwTemp & (uint32_t) ((~ACC_MR_SELMINUS_Msk) & (~ACC_MR_SELPLUS_Msk));

    pAcc->ACC_MR |= ( ((ucSelplus << ACC_MR_SELPLUS_Pos) & ACC_MR_SELPLUS_Msk) |
                      ((ucSelminus << ACC_MR_SELMINUS_Pos) & ACC_MR_SELMINUS_Msk) ) ;

}
/**
 * Return Comparison Result
 * \param pAcc Pointer to an Acc instance.
 * \param dwStatus value of ACC_ISR
 */
extern uint32_t ACC_GetComparisionResult( Acc *pAcc, uint32_t dwStatus )
{
    uint32_t dwTemp = pAcc->ACC_MR ;

    if ( (dwTemp & ACC_MR_INV) == ACC_MR_INV )
    {
        if ( dwStatus & ACC_ISR_SCO )
        {
            return 0 ; /* inn>inp*/
        }
        else
        {
            return 1 ;/* inp>inn*/
        }
    }
    else
    {
        if ( dwStatus & ACC_ISR_SCO )
        {
            return 1 ; /* inp>inn*/
        }
        else
        {
            return 0 ;/* inn>inp*/
        }
    }
}

/** @} ingroup hardware_sam3s */
