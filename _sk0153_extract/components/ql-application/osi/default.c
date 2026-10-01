/*=============================================================================
 * File       :  DEFAULT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - default
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "default.h"
#include "program_flash.h"
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
       bool Default_DefaultCommand_Empty    (void);
       bool Default_DefaultCommand_Phonebook(void);
       bool Default_DefaultCommand_Commands (void);
       bool Default_DefaultCommand_All      (void);

       bool Default_DefaultResetKey(void);

static bool Default_Default_All(void);

static bool Default_Default_Configurations_AllNoPhonebook(void);
static bool Default_Default_Configurations_Phonebook     (void);
static bool Default_Default_Commands                     (void);
static bool Default_Default_Credit                       (void);




/*===========================================================================
 * Function   : Default_DefaultCommand_Empty
 *
 * Description: "DEFAULT" command
 *              set to default values all the configurations
 *              (except the phonebook, MYN number, FWD number, received command log)
 *              and the credit available
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
bool Default_DefaultCommand_Empty(void)
{
    bool result;


    result = Default_Default_Configurations_AllNoPhonebook();
    if (!result)
        return FALSE;

    result = Default_Default_Credit();
    if (!result)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_DefaultCommand_Phonebook
 *
 * Description: "DEFAULT PHONEBOOK" command
 *              set to default values the phonebook, MYN number, FWD number
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
bool Default_DefaultCommand_Phonebook(void)
{
    bool result;


    result = Default_Default_Configurations_Phonebook();
    if (!result)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_DefaultCommand_Commands
 *
 * Description: "DEFAULT COMMANDS" command
 *              set to default values the command log
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
bool Default_DefaultCommand_Commands(void)
{
    bool result;


    result = Default_Default_Commands();
    if (!result)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_DefaultCommand_All
 *
 * Description: "DEFAULT ALL" command
 *              set to default values all the parameters saved in internal flash memory
 *              (except the phonebook)
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
bool Default_DefaultCommand_All(void)
{
    bool result;


    result = Default_Default_All();
    if (!result)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_DefaultResetKey
 *
 * Description: reset key press
 *              set to default values all the configurations
 *              (except the phonebook, MYN number, FWD number, received command log)
 *              and the credit available
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
bool Default_DefaultResetKey(void)
{
    bool result;


    result = Default_Default_Configurations_AllNoPhonebook();
    if (!result)
        return FALSE;

    result = Default_Default_Credit();
    if (!result)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_Default_All
 *
 * Description: set all the internal flash memory to default values
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
static bool Default_Default_All(void)
{
    bool result_1;
    bool result_2;


    /* 000 - Reset */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__000_RESET);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__000_RESET);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 001 - Configurations */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 002 - Phone Info */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 003 - Queue */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 004 - Alarm */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 005 - Synchronize */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 006 - Random Configurations */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__006_RANDOM_CONFIGURATIONS);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__006_RANDOM_CONFIGURATIONS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 007 - Clock */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__007_CLOCK);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__007_CLOCK);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 008 - Retransmission */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 009 - Temperature Info */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 010 - Input Info */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 011 - Credit Info */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__011_CREDIT_INFO);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__011_CREDIT_INFO);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 012 - Output Info */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__012_OUTPUT_INFO);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__012_OUTPUT_INFO);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 013 - Log */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__013_LOG);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__013_LOG);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 014 - Regulation Function */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 015 - Device ID */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__015_DEVICE_ID);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__015_DEVICE_ID);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* 016 - Counters */
    result_1 = ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS);
    result_2 = ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS);
    if ((!result_1) || (!result_2))
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_Default_Configurations_AllNoPhonebook
 *
 * Description: set all the configurations to default values
 *              (except the phonebook, MYN number, FWD number)
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
static bool Default_Default_Configurations_AllNoPhonebook(void)
{
    bool result_1;
    bool result_2;


    /* general configurations */

    /* language */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* POD */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* commands */

    /* command DETACHMENT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* command RESTORE    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* command STATUS     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* command RESET      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* SIM configurations */

    /* GPRS APN */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* GPRS DNS */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* password configurations */

    /* password */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* buttons */

    /* buttons */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* credit configurations */

    /* credit */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* counter inputs */

    /* input C1 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input C2 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* alarm configurations */

    /* internal t_max alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* internal t_max alarm temperature */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external t_max alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external t_max alarm temperature */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* internal t_min alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* internal t_min alarm temperature */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external t_min alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external t_min alarm temperature */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN1      alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN1      alarm input       */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN2      alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN2      alarm input       */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN3      alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN3      alarm input       */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN4      alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input IN4      alarm input       */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* power          alarm enable      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* phone configurations */

    /* phone  activity counters */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* SMS delay */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* counter inputs */

    /* input C1 bands */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* input C2 bands */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* answers */

    /* answer  DETACHMENT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* answer  RESTORE    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* answer  STATUS     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* answer  RESET      */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* report  configurations */

    /* report */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* FV action configurations */

    /* autorestore */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* autoreconnection */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* ADC calibration configurations */

    /* ADC0 calibration */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* ADC1 calibration */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* temperature mode */

    /* internal temperature mode */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external temperature mode */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* extra commands */

    /* extra commands */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* calibration */

    /* internal temperature calibration */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external temperature calibration */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* log status */

    /* log status */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* energy */

    /* energy */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* functions */

    /* internal antifrost function */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external antifrost function */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* internal chrono function */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external chrono function */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* names */

    /* name OUT1    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name OUT2    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name IN1     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name IN2     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name IN3     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name IN4     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name TMIN    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name TMAX    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name TMINEXT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name TMAXEXT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name C1 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* name C2 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* messages */

    /* message IN1     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN2     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN3     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN4     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMIN    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMAX    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMINEXT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMAXEXT */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN1 R     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN2 R     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN3 R     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message IN4 R     */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMIN R    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMAX R    */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMINEXT R */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* message TMAXEXT R */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* timer output */

    /* internal timer output */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* external timer output */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* output configurations */

    /* active status output OUT1 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* active status output OUT2 */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2);
    if ((!result_1) || (!result_2))
        return FALSE;


    /* forward configurations */

    /* forward  */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD);
    if ((!result_1) || (!result_2))
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_Default_Configurations_Phonebook
 *
 * Description: set the phonebook, MYN number, FWD number configurations to default values
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
static bool Default_Default_Configurations_Phonebook(void)
{
    bool result_1;
    bool result_2;


    /* MYN phone number */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* phonebook */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK);
    if ((!result_1) || (!result_2))
        return FALSE;

    /* FWD phone number */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER);
    if ((!result_1) || (!result_2))
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_Default_Commands
 *
 * Description: set the received command log to default values
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
static bool Default_Default_Commands(void)
{
    bool result_1;
    bool result_2;


    /* rx command log */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);
    if ((!result_1) || (!result_2))
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Default_Default_Credit
 *
 * Description: set the credit info to default values
 * Input      : -
 * Output     : - FALSE: action fail
 *            : - TRUE : action success
 *===========================================================================*/
static bool Default_Default_Credit(void)
{
    bool result_1;
    bool result_2;


    /* available credit */
    result_1 = ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT);
    result_2 = ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT);
    if ((!result_1) || (!result_2))
        return FALSE;


    return TRUE;
}
