/*=============================================================================
 * File       :  FW_CONFIG.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FW configuration
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FW_CONFIG_H__


#define __FW_CONFIG_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* model identifier type */
#define FW_CONFIG__TYPE__THERMOSTAT                     0   // model identifier type - thermostat
#define FW_CONFIG__TYPE__PHOTOVOLTAIC                   1   // model identifier type - photovoltaic

/* model identifier hardware */
#define FW_CONFIG__HARDWARE__DIN                        0   // model identifier hardware - DIN
#define FW_CONFIG__HARDWARE__WALL                       1   // model identifier hardware - WALL


/*-----------------------------------------------------------------------------
 *  (*) standard value
 *-----------------------------------------------------------------------------*/


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *                                 VERSION
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*-----------------------------------------------------------------------------
 * Version
 *-----------------------------------------------------------------------------*/
/* version */
  #define FW_CONFIG__VERSION__LSHD                          // version - LINKY    (DIN)
//#define FW_CONFIG__VERSION__LSHW                          // version - LINKY    (WALL)                 // (*)
//#define FW_CONFIG__VERSION__LFVD                          // version - LINKY FV (DIN) 
//#define FW_CONFIG__VERSION__LFVW                          // version - LINKY FV (WALL)


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *                                     HW
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*-----------------------------------------------------------------------------
 * NTC model
 *-----------------------------------------------------------------------------*/
/* NTC model list */
#define FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC         0   // NTC model - 4K7 theoretic
#define FW_CONFIG__HW__NTC_MODEL__4K7_REAL              1   // NTC model - 4K7 real
#define FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC         2   // NTC model - 10K theoretic
#define FW_CONFIG__HW__NTC_MODEL__10K_REAL              3   // NTC model - 10K real
#define FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC  4   // NTC model - 10K Murata theoretic

/* NTC model */
//#define FW_CONFIG__HW__NTC_MODEL                      FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC
//#define FW_CONFIG__HW__NTC_MODEL                      FW_CONFIG__HW__NTC_MODEL__4K7_REAL
//#define FW_CONFIG__HW__NTC_MODEL                      FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC
//#define FW_CONFIG__HW__NTC_MODEL                      FW_CONFIG__HW__NTC_MODEL__10K_REAL
  #define FW_CONFIG__HW__NTC_MODEL                      FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC   // (*)


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *                                     DEFAULT
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*-----------------------------------------------------------------------------
 * SIM PIN
 *-----------------------------------------------------------------------------*/
/* SIM PIN list */
#define FW_CONFIG__SIM_PIN__OFF                         0   // SIM PIN - off
#define FW_CONFIG__SIM_PIN__ON                          1   // SIM PIN - on

/* SIM PIN */
  #define FW_CONFIG__SIM_PIN                            FW_CONFIG__SIM_PIN__OFF                          // (*)
//#define FW_CONFIG__SIM_PIN                            FW_CONFIG__SIM_PIN__ON


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *                                     TEST
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*-----------------------------------------------------------------------------
 * TEST - Mode 1
 *-----------------------------------------------------------------------------*/
/* mode 1 list */
#define FW_CONFIG__TEST__MODE_1__NORMAL                 0   // mode 1 - normal
#define FW_CONFIG__TEST__MODE_1__FAST                   1   // mode 1 - fast

/* mode 1 */
  #define FW_CONFIG__TEST__MODE_1                       FW_CONFIG__TEST__MODE_1__NORMAL                    // (*)
//#define FW_CONFIG__TEST__MODE_1                       FW_CONFIG__TEST__MODE_1__FAST


/*-----------------------------------------------------------------------------
 * TEST - Mode2
 *-----------------------------------------------------------------------------*/
/* mode 2 list */
#define FW_CONFIG__TEST__MODE_2__NORMAL                 0   // mode 2 - normal
#define FW_CONFIG__TEST__MODE_2__JUMP                   1   // mode 2 - jump

/* mode 2 */
  #define FW_CONFIG__TEST__MODE_2                       FW_CONFIG__TEST__MODE_2__NORMAL                    // (*)
//#define FW_CONFIG__TEST__MODE_2                       FW_CONFIG__TEST__MODE_2__JUMP


/*-----------------------------------------------------------------------------
 * TEST - Temperature exchange
 *-----------------------------------------------------------------------------*/
/* temperature exchange list */
#define FW_CONFIG__TEST__TEMP_EXCHANGE__NO              0   // temperature exchange - no
#define FW_CONFIG__TEST__TEMP_EXCHANGE__YES             1   // temperature exchange - yes

/* temperature exchange */
  #define FW_CONFIG__TEST__TEMP_EXCHANGE                FW_CONFIG__TEST__TEMP_EXCHANGE__NO                 // (*)
//#define FW_CONFIG__TEST__TEMP_EXCHANGE                FW_CONFIG__TEST__TEMP_EXCHANGE__YES


/*-----------------------------------------------------------------------------
 * TEST - Temperature compensation
 *-----------------------------------------------------------------------------*/
/* temperature compensation list */
#define FW_CONFIG__TEST__TEMP_COMPENSATION__NO          0   // temperature compensation - no
#define FW_CONFIG__TEST__TEMP_COMPENSATION__YES         1   // temperature compensation - yes

/* temperature compensation */
//#define FW_CONFIG__TEST__TEMP_COMPENSATION            FW_CONFIG__TEST__TEMP_COMPENSATION__NO
  #define FW_CONFIG__TEST__TEMP_COMPENSATION            FW_CONFIG__TEST__TEMP_COMPENSATION__YES            // (*)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* time jump */
typedef struct
{
    u8 type;      // model identifier type
    u8 hardware;  // model identifier hardware
} FW_CONFIG__ID_MODEL;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set time jump */
void FwConfig_IdModel_GetDefault(FW_CONFIG__ID_MODEL *ptr_data);
void FwConfig_IdModel_Get       (FW_CONFIG__ID_MODEL *ptr_data);
void FwConfig_IdModel_Set       (FW_CONFIG__ID_MODEL *ptr_data);
bool FwConfig_IdModel_IsValid   (FW_CONFIG__ID_MODEL *ptr_data);




#endif
