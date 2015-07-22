/**

 * \file 'func.h'
 *
 * \author U. Hinz
 * \author B. Hennig
 *
 * \brief 'func.h' is a headerfile to be
 * included in 'func.c'. It provides error code constants,
 * (see: Error values),
 * USB buffer sizes (see: Values concerning buffers), constants
 * special for a certain functions (see: Values belonging to a
 * function) and function prototypes. Sixteen of them belong
 * to exported functions (see: Functions that are exported).
 * They are arranged in the same order as listed in the modul
 * definition file 'k8055.def'. All remaining function
 * prototypes (see: Functions that are not exported) are
 * subfunctions or helping items.
 *
 * Three basic files are needed for the project:
 *   For the compiler 'func.c' and
 *                    'func.h' (this file).
 *   For the linker   'k8055.def'
 *
 *
 * \version 1.0.13 -
 * 2010-10-09 constants renamed, commenting
 * \version 1.0.12 -
 * 2010-09-22 function K8055_Init_01 removed again, commenting
 * \version 1.0.11 -
 * 2010-09-19 function K8055_Init redone, renaming, commenting
 * \version 1.0.10 -
 * 2010-09-16 new function K8055_Init_01, renaming, commenting
 * \version 1.0.9 -
 * 2010-05-14 constants renamed, commenting
 * \version 1.0.8 -
 * 2010-03-25 new function K8055_InfoStr, commenting
 * \version 1.0.7 -
 * 2010-02-07 new function K8055_PrepairDACxOut, commenting
 * \version 1.0.5 -
 * 2010-01-22 new function K8055_PrepairDigitalOut(),
 * commenting
 * \version 1.0.4 -
 * 2009-12-14 new function K8055_DecodeDigitalInputs(),
 * commenting
 * \version 1.0.3 -
 * 2009-12-13 new function K805_ReadAllInputs()
 * \version 1.0.2 -
 * 2009-12-10 new function K8055_DummyFnc1()
 * \version 1.0.1 -
 * 2009-08-23 new defines, fix docu
 * \version 1.0.0 - 2009-08-12 init

 * \todo rename lib, better docu and defines
 */
#ifndef __K8055DD_H_FUNC_
#define __K8055DD_H_FUNC_


//---- Error values ----------------------------- BEGIN ---
//
/**
* \brief Signals a successful return from an OS2-API Call
*/
#define NO_DOS_ERROR 0x0




//-- Error values used in exported functions ---- BEGIN --#
//
/**
* \brief If an exported function was called and nothing
* went wrong, this is the Error Code that says 'no error'.
*
* Caution: Every exported function sets its Return Value
* internally to 'RET_OKAY' in the beginning. Example:
*
*      {
*        ULONG ulrc;
*         ...
*        ulrc = RET_OKAY;
*         ...
*        return ulrc;
*      }
*
* Any value, that is greater than 0 indicates an
* error of various origin and various kind.
* ( For details see the following #define statements
*   containing the string fragment 'ERROR_' )
*
*/
#define RET_OKAY   0x0


/**
* \brief During initialisation, a number of things can
* go wrong. If any of the steps may fail, ERROR_INIT is
* a representative of all this.
*
*/
#define ERROR_INIT   0x1


/**
* \brief Any pointer that was not initialised properly is
* risky or dangerous.
* Especially when routines are called, any wrongly used
* pointer must be discovered and avoided.
* 'ERROR_POINTER' is therefore the most important Error
* Code of that project. All exported functions are able to
* set this Error Code bit !
*
*/
#define ERROR_POINTER   0x2


/**
* \brief This Error Code dates from the earlier days
* of this project. It is not used at the moment.
*
*/
#define ERROR_INTERN   0x4


/**
* \brief This Error Code is more important within
* development phase. It says if the developer used the
* internal DLL byte buffers in a wrong way.
* 'ERROR_BUFFER' is not related to 'ERROR_RANGE'!
*
*/
#define ERROR_BUFFER   0x10


/**
* \brief Both, USB Data Transfer and USB Setup actions
* sinal how many bytes have been written or have been
* read. The Developer can check if expected byte numbers
* are OK or wrong. This Error Code can therefore show
* if the USB device K8055 was unpluged or does not work.
*
*/
#define ERROR_BYTE_NUMBER   0x20


/**
* \brief Any successful USB Data Transfer is indicated
* by the Toggle Bit. The value this bit had before a USB
* Data Transfer action must appear as inverted afterwards.
* If inversion did not happen, the action failed.
* A number of erroneous situations is covered by this
* Error Code. In example:
* - The USB device K8055 was unpluged or switched off,
* - an internal USB device error happened or
* - a USB Parameter Packet was badly composed or wrongly
* used during development.
* Caution: USB Data Transfer actions are never carried out
* via Endpoint 0 (EP0). Setup data which travel via EP0 do
* not and cannot make use of the Toggle Bit!
*
*/
#define ERROR_TOGGLE_BIT   0x40


/**
* \brief If a parameter exceedes its allowed limit, this
* Error Code shows such a situation.
*
*/
#define ERROR_RANGE   0x80


/**
* \brief If an exported function calls subroutines of
* another origin like 'DosRead()' or selfmade routines,
* this Error Code can indicate that such a routine
* went well or failed.
*
*/
#define ERROR_FROM_CALL   0x100


/**
* \brief This Error Code has got no specific meaning.
* It serves as a neutral helper value during development.
*
*/
#define ERROR_TESTERROR   0x200
//
//-- Error values used in exported functions ------ END --#




//---- Values concerning buffers ---------------- BEGIN --;
/**
* \brief Number of bytes to be sent to the usb driver
*/
#define SIZEUSBHEADER 8

/**
* \brief Number of Bytes for write operations to K8055
*/
#define SIZEPUTBYTES 16

/**
* \brief Number of Bytes for read operations to K8055
*/
#define SIZEGETBYTES 16

/**
* \brief Buffer size for read and write actions
*/
#define SIZEBUFFERMAX 8
//
//---- Values concerning buffers ------------------ END --;




//-- Values belonging to a function ------------- BEGIN --!
//
/**
* \brief Maximum number of Info Strings
*/
#define INFO_STR_MAX_NUM 6
/**
* \brief Maximum number of Info String characters
*/
#define INFO_STR_CHARS_MAX 33
//
//-- Values belonging to a function --------------- END --!





// -- Functions that are exported ------------- * -- BEGIN ----
//


//--- K8055_Open ----------------------------------------------
//                                             Export Index 1
/**
* \brief 'K8055_Open()' will open one USB Device K8055 for
* later read or write operations.
* Those later operations include initialisation (Setup) and
* data exchange.
*
* \param   'pcaDeviceName'
*          - Pointer to the character array that keeps the
*          Device Name.
*          In the 'CONFIG.SYS' statement it follows the
*          N-Parameter.
*          ( ..\usbecd.sys /D:10CF:5500:0000 /N:KDEV01 )
*
* \param   'pulFileDesc'
*          - After 'K8055_Open()' was called successfully,
*          it contains a Device ID (File ID) that represents
*          that particular device within the OS.
*          The opened K8055 remains open untill it will
*          be closed by 'K8055_Close()'.
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Open( CHAR *pcaDeviceName,
                  ULONG *pulFileDesc   );
// --------------------------------------1



//--- K8055_init ----------------------------------------------
//                                             Export Index 2
/**
* \brief 'K8055_init()' performs all steps that are needed to
* initialise K8055. First, device descriptors are read,
* then the configuration is set, and finally vendor requests
* are done. If this is done, K8055 is usable.
*
* \param   'pulFileDesc'
*          - An application must consider, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be initialised.
*
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x001  ERROR_INIT       Initialisation was not successful.
*                          Possible reasons:
*                          - Incorrect entry in 'CONFIG.SYS',
*                          - K8055 not working,
*                          - Any kind of hardware problem,
*                          - K8055 not plugged in.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*  010  ERROR_BUFFER     Indicating a bad communication
*                          via Endpoint 0 (EP0) during
*                          initialisation.
*
*/
ULONG K8055_Init( ULONG *pulFileDesc );
// ------------------------------------2



//--- K8055_Read ----------------------------------------------
//                                             Export Index 3
/**
* \brief 'K8055_Read()' is for reading data from K8055 after
* its initialisation was done by 'K8055_Init()'.
* Every 10 miliseconds a new set of 8 data bytes can be
* claimed from the device.
*
* The position of the data bytes and their meaning are:
*
*  { Ix xx A1 A2 I1cL I1cH I2cL I2cH }
*
*     Ix - Digital Input
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used)
*     xx - unknown
*     A1 - Analog Input 1 (0..255)
*     A2 - Analog Input 2 (0..255)
*     I1cL - Digital Input I1 Event Counter (Lower Byte)
*     I1cH - dito (Higher Byte)
*             (0..65535)
*     I2cL - Digital Input I2 Event Counter (Lower Byte)
*     I2cH - dito (Higher Byte)
*             (0..65535)
*
*
* 'K8055_Read()' will not extract or decode single data bytes
* for convinience.
* Applications that want to make use of them have to do
* this as appropriate for their special needs.
*
* The function 'K8055_Read()' reads 8 data bytes from a buffer
* How big that buffer is and where it is, must be told by
* the parameters 'byNumberOfBytes' and 'pbyaGetData'.
*
* Caution!
* Using this function is risky, due to its weak error
* handling. Furthermore, an unsuited parameter can lead to
* unwanted consequences !
*
* \param   'pulFileDesc'
*          - An application must know, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be addressed.
*
* \param   'byNumberOfBytes'
*          - For K8055, the buffer that will hold the data
*          after reading, is allways 8 bytes long.
*
* \param   'pbyaGetData'
*          - Where to find the data after reading was done,
*          is given with this parameter. 'pbyaGetData' points
*          at the address of the first byte xx.
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Read( ULONG *pulFileDesc,
                  BYTE byNumberOfBytes,
                  BYTE *pbyaGetData     );
// ---------------------------------------3



//--- K8055_Write ---------------------------------------------
//                                             Export Index 4
/**
* \brief 'K8055_Write()' is for sending data to an initialised
* K8055. Initialisation can be done with 'K8055_Init()' before.
* Then, a (new) set of 8 data bytes can be written.
*
* The timespan between two calls of 'K8055_Write()' must
* be longer than 10 milliseconds.
*
* Within that set of 8 data bytes the positions
* and their meaning are:
*
*   { 0x05 DO DAC1 DAC2 xx xx xx xx }
*
*     0x05 - First byte is allways 0x05 - meaning unknown
*     DO   - Digital Output (0..255)
*     DAC1 - Analog Output 1 (0..255)
*     DAC2 - Analog Output 2 (0..255)
*     xx   - Unknown
*
* Caution!
* Using this function is risky, due to its weak error
* handling. Furthermore, an unsuited parameter can lead to
* unpredictable consequences !
*
* \param   'pulFileDesc'
*          - An application must know, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be addressed.
*
* \param   'byNumberOfBytes'
*          - 8 is the number of data bytes which will be put
*          into the data byte section of that buffer.
*          Any value different from 8 will lead to unforeseen
*          results.
*
* \param   'pbyaPutData'
*          - Where to find the data before writing,
*          is given with this parameter. 'pbyaPutData' points
*          at the address of the first byte. It is allway
*          0x05 as mentioned above.
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*/
ULONG K8055_Write( ULONG *pulFileDesc,
                   BYTE byNumberOfBytes,
                   BYTE *pbyaPutData     );
// ----------------------------------------4



//--- K8055_Close ---------------------------------------------
//                                             Export Index 5
/**
* \bief The function 'K8055_Close()' will close a K8055
* which has been opened before. If the action was
* error free, no further interaction is possible with that
* particular K8055 any longer.
* If initialisation or data exchange are needed again,
* the correspondent function 'K8055_Open()' must be called
* before.
*
* \param   'pcaDeviceName'
*          - Pointer to the character array that keeps the
*          Device Name.
*          In the 'CONFIG.SYS' statement it follows the
*          N-Parameter. In example:
*          ( ..\usbecd.sys /D:10CF:5500:0000 /N:K8055_$ )
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*/
ULONG K8055_Close( ULONG* pulFileDesc );
// -------------------------------------5



//--- K8055_DummyFunc -----------------------------------------
//                                             Export Index 6
/**
* \brief The function 'K8055_DummyFunc()' is for testing
* purposes only. It provides a kind of Function Skeleton
* with no particular meaning.
*
* \param   'pulDummyParam'
*          - A function without any parameter is no fun at all.
*          'K8055_DummyFunc' doesn't do anything at all.
*
* \return  - Return Code. This function has got no error
*          handling. The only meaning is mentioned below:
*
*  0x000  RET_OK
*
*
*/
ULONG K8055_DummyFnc( ULONG *pulDummyParam );
// ------------------------------------------6



//--- K8055_DummyFunc1 ----------------------------------------
//                                             Export Index 7
/**
* \brief The function 'K8055_DummyFunc1()' is just for testing
* purposes. It is not only a Function Skeleton, it is also
* for learning how variables can be changed.
*
* \param   'pulDummyParam1'
*          - This parameter points to the variable which is
*          to be changed within the function.
*          An input value will be incremented two times.
*
* \return  - Return Code.
*          Meaning listed below:
*
*  0x000  RET_OK            Call returned with no error.
*
*  0x002  ERROR_POINTER     Indicating parameter problems.
*
*
*/
ULONG K8055_DummyFnc1( ULONG *pulDummyParam1 );
// --------------------------------------------7



//--- K8055_ReadAllInputs -------------------------------------
//                                             Export Index 8
/**
* \brief The function 'K8055_ReadAllInputs()' will read the
* digital inputs I1, I2, I3, I4 and I5 (summarised as Ix).
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   'pulFileDesc'
*          - An application using this function must know,
*          that more than one K8055 may exist. With this
*          parameter a particular K8055 can be addressed.
*
* \param   'pulDigitalInputsIx'
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0).
*
* \param   'pulAnalogInputA1'
*          - This parameter points to a variable that contains
*          the state of the first analog input A1 (0..255).
*
* \param   'pulAnalogInputA2'
*          - This parameter points to another variable
*          containing the state of the second
*          analog input A2 (0..255).
*
* \return  - Return Code.
*          Meaning listed below:
*
*  0x000  RET_OK             Call returned with no error.
*
*  0x002  ERROR_POINTER      Indicating parameter problems.
*
*  0x020  ERROR_BYTE_NUMBER  Reading did not work at all.
*
*  0x040  ERROR_TOGGLE_BIT   Repetitative reading did not work,
*                            device may be unpluged or switched
*                            off.
*
*  0x100  ERROR_FROM_CALL    Further routines that were used
*                            inside 'K8055_ReadAllInputs'
*                            returned with an internal error.
*
*
*/
ULONG K8055_ReadAllInputs
                      ( ULONG *pulFileDesc,
                        ULONG *pulDigitalInputsIx,
                        ULONG *pulAnalogInputA1,
                        ULONG *pulAnalogInputA2    );
// --------------------------------------------------8



//--- K8055_DecodeDigitalInputs -------------------------------
//                                             Export Index 9
/**
* \brief The function 'K8055_DecodeDigitalInputs()' will
* convert raw data coming from Ix into comprahecable values.
*
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   'pulDigitalInputsIx'
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0).
*
* \param   'pulDigitalInputsRslt'
*          - Pointer to the variable that will contain the
*          result. (see item 'return')
*
* \return  'pulDigitalInputsRslt'
*          - After the function has done its job,
*          the meaning of the bits has changed:
*           (I1: 0x01, I2: 0x02, I3: 0x04, I4: 0x08, I5: 0x10,
*            I6, I7, I8: not used, set to 0).
*
* \return  - Return Code.
*          Meaning listed below:
*
*  0x000  RET_OK             Call returned with no error.
*
*  0x002  ERROR_POINTER      Indicating parameter problems.
*
*
*/
ULONG K8055_DecodeDigitalInputs
                             ( ULONG *pulDigitalInputsIx,
                               ULONG *pulDigitalInputsRslt );
// ----------------------------------------------------------9



//--- K8055_CheckDigitalInput ---------------------------------
//                                            Export Index 10
/**
* \brief If the state of a single digital input is to be
* checked, the function 'K8055_CheckDigitalInput()' is able to
* tell it.
* Raw data coming from Ix into comprahecable values.
* digital inputs I1, I2, I3, I4 and I5 (summarised as Ix).
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   'pulDigitalInputsIx'
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0).
*
* \param   'pulSingleDigitalInResult'
*          - Pointer to the variable that will contain the
*          result. (see item 'return')
*
* \param   'pulInputIndex'
*          - This parameter will select the particular
*          digital input. I1 is selected by the parameter
*          value 1 through I5 by the value 5.
*
*
* \return  'pulSingleDigitalInResult'
*          - After the function has done its job,
*          the result value 1 indicates a pressed button
*          (Low voltage). If the result value is 0, the
*          button was released (equ. to High voltage).
*          In order to indicate a malfunction the return
*          value can be 2.
*
* \return  - Return Code.
*          Meaning listed below:
*
*  0x000  RET_OK         Call returned with no error.
*
*  0x002  ERROR_POINTER  Indicating parameter problems.
*
*  0x080  ERROR_RANGE    The parameter 'pInputIndex'
*                        can only carry values in the range
*                        of 1..5. If not, this error code
*                        will show it.
*
*
*/
ULONG K8055_CheckDigitalInput
                        ( ULONG *pulDigitalInputsIx,
                          ULONG *pulSingleDigitalInResult,
                          ULONG *pulInputIndex             );
// ----------------------------------------------------------10



//--- K8055_SetAllOutputs -------------------------------------
//                                            Export Index 11
/**
* \brief The function 'K8055_SetAllOutputs()' will write 8
* bytes of data to the USB device K8055.
* Before the function can perform well, an array of 16 bytes
* must be prepaired. The name of this array is 'byaPutData[]'.
* It is not wise to change this array freely.
*
* To get its preparation done, two other exported functions
* 'K8055_PrepairDigitalOut()' and 'K8055_PrepairDACxOut()'
* must be used (see descriptions of them below).
*
*     ( Parameter Packet )
*     0xEC, 0x10, 0x00, 0x00, 0x01, 0x03, 0x08, 0x00,
*
*     ( 8 bytes of data trailing )
*     0x05, DO, DAC1, DAC2, xx, xx, xx, xx
*
*
* Every position within the 8 data bytes has got a meaning,
* listed here:
*
*     0x05 - First byte is allways 0x05 - meaning unknown
*
*     DO -   Digital Output (0..255)
*     DAC1 - Analog Output 1 (0..255)
*     DAC2 - Analog Output 2 (0..255)
*
*     xx - Not needed, if the first byte is 0x05.
*
* Attention!
* DO or DAC1 or DAC2 cannot be passed over to K8055
* separately. They must be treated as a group of 3 values!
*
*
* \param   'pulFileDesc'
*          - An application using this function must take in
*          account, that more than one K8055 may exist. With
*          this parameter an opened K8055 can be referred to.
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*   0x000  RET_OK            Call returned with no error.
*
*   0x002  ERROR_POINTER     Indicating parameter problems.
*
*   0x100  ERROR_FROM_CALL   The API-Call 'DosWrite'
*                            returned with error(s).
*
*
*/
ULONG K8055_SetAllOutputs( ULONG *pulFileDesc );
// ---------------------------------------------11



//--- K8055_PrepairDigitalOut ---------------------------------
//                                            Export Index 12
/**
* \brief The function 'K8055_PrepairDigitalOut()' will change
* one single byte on the DO-Position of the array
* 'byaPutData[]'.
* After 'K8055_PrepairDigitalOut()' is successfully over,
* data can be sent to K8055 using the exported function
* 'K8055_SetAllOutputs'.
*
* Attention!
* In many cases the developer should not forget to use
* the exported function 'K8055_PrepairDACxOut()' before
* 'K8055_SetAllOutputs()' can be called without provocing
* a K8055 functional hazard.
*
*
* \param   'pulDigitalOutValue'
*          - This parameter points to that value, which will
*          finally appear at the K8055 Digital Outputs.
*
*          K8055 Digital Outputs are low active. If 0x40
*          is sent, the output O7 will have approximately
*          0 Volts. A current of up to 100 mA can be handled.
*          in this situation.
*          At the same time Light Emitting Diode LD7 is lit.

*          Although this parameter transfers a variable
*          typed Unsigned Long, the range 0..255 (0x00..0xff)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*   0x000  RET_OK            Call returned with no error.
*
*   0x002  ERROR_POINTER     Indicating parameter problems.
*
*   0x080  ERROR_RANGE       This bit is set, if the value
*                            in 'ulDigitalOutValue' is not
*                            in the range of 0..255.
*
*   0x200  ERROR_TESTERROR   Only used under special
*                            debugging conditions during
*                            development.
*
*
*/
ULONG K8055_PrepairDigitalOut( ULONG *pulDigitalOutValue );
// --------------------------------------------------------12



//--- K8055_PrepairDACxOut ------------------------------------
//                                            Export Index 13
/**
* \brief The function 'PrepairDACxOut()' will change one single
* byte on the DAC1-Position or on the DAC2-Position of the
* array 'byaPutData[]'.
* After 'PrepairDACxOut()' is successfully over,
* data can be sent to K8055 using the exported function
* 'K8055_SetAllOutputs'.
*
* Caution!
* In many cases the developer should not forget to use
* the exported function 'K8055_PrepairDigitalOut()' before
* 'K8055_SetAllOutputs()' can be called without provocing
* a K8055 functional hazard.
*
* \param   'pulDACxOutValue'
*          - This parameter points to that value, which will
*          finally appear at the K8055 Analog Output DAC1 or
*          DAC2.
*          K8055 Analog Outputs can have an output voltage
*          ranging from 0 to app. 5 Volts, devided into 256
*          steps. Starting with 0x00 (0 * 0.0196 V = 0.0000 V)
*          and ending with 0xFF (255 * 0.0196 V = 4.9980 V).
*
*          Allthough this parameter transfers the variable
*          typed Unsigned Long, the range 0..255 (0x00..0xff)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
*          Caution! The analog output voltage is not very
*          accurate, so 4.9980 V is more of an ideal value.
*          It is only feasable if the USB Supply Voltage is
*          exactly 5.0000 V.
*          From PC to PC this supply voltage can vary
*          significantly. A K8055 user should think twice
*          before he or she may reley on the analog outputs
*          DAC1 and DAC2
*
*
* \param   'pulOutputIndex'
*          - Two analog outputs, DAC1 and DAC2 available on
*          K8055, need to be distinguished one from the other.
*          If this parameter transfers the value 1, DAC1 will
*          be selected, 2 will do the same with DAC2.
*              Allthough this parameter transfers the variable
*          type Unsigned Long, the range 1..2 (0x01..0x02)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*   0x000  RET_OK           Call returned with no error.
*
*   0x002  ERROR_POINTER    Indicating parameter problems.
*
*   0x080  ERROR_RANGE      This bit is set, if the value in
*                           'ulDigitalOutValue' is not
*                           in the range of 0..255.
*                           This bit is also set, if the value
*                           in 'ulOutputIndex' is not in the
*                           range of 1..2.
*
*   0x200  ERROR_TESTERROR  Not used inside this function,
*                           testing aid outside of
*                           'K8055_PrepairDACxOut()'.
*
*
*/
ULONG K8055_PrepairDACxOut( ULONG *pulDACxOutValue,
                            ULONG *pulOutputIndex   );
// ---------------------------------------------------13



//--- K8055_ReadIxConter --------------------------------------
//
//                                            Export Index 14
//
/**
* \brief Two Digital Inputs, I1 and I2, are connected to
* internal counters. One counter for impulses that come in
* via I1 and a second counter which will do the same with
* impulses on I2.
* Both counters can count from 0 up to 65535 (0x0000...0xffff).
*
* The location of the bytes that carry the counter values are
* already shown in the desription of the exported routine
* 'K8055_ReadAllInputs()'.
* Here is a short repetition just to focus on the
* counter bytes.
*
*  { Ix xx A1 A2 I1cL I1cH I2cL I2cH }
*
*     I1cL - Digital Input I1 Event Counter (Lower Byte)
*     I1cH - dito (Higher Byte)
*             (0..65535)
*     I2cL - Digital Input I2 Event Counter (Lower Byte)
*     I2cH - dito (Higher Byte)
*             (0..65535)
*
* \param   'pulIxCounterValue'
*          -  This parameter points the variable that
*          will holds one of the two counter results
*          After 'K8055_ReadIxConter()' has returned from
*          a call it contains what was read by the last
*          use of 'K8055_ReadAllInputs()'.
*          In order to be as close as possible to the
*          current counter values, an application should
*          call 'K8055_ReadAllInputs()' and
*           'K8055_ReadIxConter()' with the shortest delay
*          possible.
*
*
* \param   'pulCounterIndex'
*          - This parameter helps to select one of the two
*          internal counters. In order to select the
*          I1 Impuls Counter, its value must be set to 1.
*          If the value 2 is chosen, the I2 Impuls Counter
*          is addressed.
*          Any value different from 1 and 2 will produce
*          the error code ERROR_RANGE.
*
* \return  - Return Code.
*          Meaning of bits listed below:
*
*   0x000  RET_OK           Call returned with no error.
*
*   0x002  ERROR_POINTER    Indicating parameter problems.
*
*   0x080  ERROR_RANGE      This bit is set, if the value in
*                           'ulCounterIndex' is not
*                           in the range of 1..2.
*
*/
ULONG K8055_CheckIxCounter( ULONG *pulIxCounterValue,
                            ULONG *pulCounterIndex    );
// -----------------------------------------------------14



//--- K8055_InfoStr -------------------------------------------
//
//                                            Export Index 15
/**
* \brief The DLL 'K8055DD.dll' has got some properties
* that can become usefull for additional information.
* Especially versioning and author names are just two of
* them.
*
* ' CHAR  szString1[INFO_STR_MAX_NUM][INFO_STR_CHARS_MAX] =  '
* '                                                          '
* '              { "1 abcdefghijklmnopqrstuvwxyzABCD\0",     '
* '                "2 Authors: B. Hennig , U. Hinz  \0",     '
* '                "3 Date: 2010-10-09              \0",     '
* '                "4 DLL-Version: 1.0.12           \0",     '
* '                "5 Licence: BSD                  \0",     '
* '                "6 DLL-Name: K8055DD             \0"  };  '
*
*  'INFO_STR_MAX_NUM'   equ  6    and
*  'INFO_STR_CHARS_MAX' equ  33,  both included from 'func.h'.
*
* 'szString1[]' is the array that holds six info strings.
* Info string element 0 is just a length template and does
* not carry any message. All remaining info string elements
* are usable in future applications.
* Any info string shows an internal structure, explained below:
*
*
*    +------------------- Auxiliary Index (2 characters long)
*    |
*    |                                +---- String Delimiter
*    |                                |     ( 1 byte )
*    --                               -
*   "4 DLL-Version: 1.0.12           \0"
*
*      ------------------------------
*      1           ^                3
*                  |                0
*                  |
*                  |
*
*                  The actual info string (30 characters long)
*
*
* \param   'pszInfoString'
*          - This parameter must contain a pointer that
*          points to the first element of a character array.
*          Caution:
*          The array must have the size of 33 characters due
*          to its task as a result buffer.
*
*          Returning from call, the said character array will
*          have a changed content ! An application can then
*          work with it as a result string.
*
* \param   'pulStringIndex'
*          - Six info strings are available, but just one can
*          be selected. This parameter tells the function
*          which out of them is wanted. Index values can be
*          in the range of 1..6. If not, the function will
*          return with the error code ERROR_RANGE.
*
*
* \return  - Return Code. Meaning of bits listed below:
*
*   0x000  RET_OK           Call returned with no error.
*
*   0x002  ERROR_POINTER    Indicating parameter problems.
*
*   0x080  ERROR_RANGE      This bit is set, if the value in
*                           'ulStringIndex' is not
*                           in the range of 1..6.
*
*   0x200  ERROR_TESTERROR  The function 'K8055_GetInfoStr'
*                           uses the C runtime function
*                           'memcpy()'. During testing and
*                           debugging this runtime function
*                           can be disabled for carefulness.
*                           If so, this bit indicates this
*                           short-time state.
*
*
*/
ULONG K8055_GetInfoStr( CHAR *pszInfoString,
                        ULONG *pulStringIndex );
// ---------------------------------------------15



//--- K8055_InfoBytes -----------------------------------------
//
//                                            Export Index 16
/**
* \brief The DLL 'K8055DD.dll' has got some properties
* that may become usefull for additional information.
* In most cases, this information can be obtained by the
* previous function 'K8055_GetInfoStr()' (Export Index 15).
*
* In some cases it turned out to be better to obtain an
* info string as single characters, for 'Sibyl' (Pascal)
* in particular.
*
* Any info string shows an internal structure, explained below:
*
*
*    +------------------- Auxiliary Index (2 characters long)
*    |
*    |                                +---- String Delimiter
*    |                                |     ( 1 byte )
*    --                               -
*   "4 DLL-Version: 1.0.12           \0"
*
*      ------------------------------
*      1           ^                3
*                  |                0
*                  |
*                  |
*
*                  The actual info string (30 characters long)
*
* Every single character must be transfered via the parameter
* 'pbyInfoByte'. Done thirty times, the calling application
* can recompose the complete info string again.
*
*
*
* \param   'pbyInfoByte'
*          - This parameter must contain a pointer that can
*          points to a BYTE variable. It works like a transfer
*          buffer for one character.
*
* \param   'pulSectionIndex'
*          - Six info strings are available, but just one can
*          be selected. This parameter tells the function
*          which out of them is wanted. Index values can be
*          in the range of 1..6. If not, the function will
*          return with the error code ERROR_RANGE.
*
* \param   'pulByteIndex'
*          - One info strings consists of 33 characters. In
*          order to pick just one out of them, another index
*          is necessary.
*          For this byte index values between 1 and 33 are
*          allowed ( see INFO_STR_MAX_NUM ).
*          If not in this range the function will return with
*          the error code ERROR_RANGE.
*
* \return  - Return Code. Meaning of bits listed below:
*
*   0x000  RET_OK           Call returned with no error.
*
*   0x002  ERROR_POINTER    Indicating parameter problems.
*
*   0x080  ERROR_RANGE      This bit is set, if the value in
*                           'ulSectionIndex' is not
*                           in the range of 1..6.
*
*                           It will be also set, if the value
*                           in 'ulByteIndex' is not in its
*                           span of 1..33.
*
*   0x200  ERROR_TESTERROR  The function 'K8055_GetInfoByte'
*                           uses the C runtime function
*                           'memcpy()'. During testing and
*                           debugging this runtime function
*                           needs to be disabled from time
*                           to time for thoroughness.
*                           If made use of it, this bit is
*                           an indicator.
*
*/
ULONG K8055_GetInfoByte( BYTE *pbyInfoByte,
                         ULONG *pulSectionIndex,
                         ULONG *pulByteIndex      );
// -------------------------------------------------16

//
// -- Functions that are exported --------------- * -- END ----





// -- Functions that are not exported --------- + -- BEGIN ----
//
//--- Setup routines, used by K8055_Init ----------------------
//    - Reading descriptors
//
ULONG GetDeviceDescriptor( ULONG ulDevpointer );
ULONG GetConfigurationDescriptor( ULONG ulDevpointer );
ULONG GetLanguageDescriptor( ULONG ulDevpointer );
ULONG Get4thStringDescriptor( ULONG ulDevpointer );
ULONG GetString2Descriptor( ULONG ulDevpointer );
ULONG DoUnknown21( ULONG ulDevpointer );
ULONG DoUnknown30Bytes( ULONG ulDevpointer );

//--- Setup routines, used by K8055_Init ----------------------
//    - Setting configuration
//
ULONG SetConfiguration( ULONG ulDevpointer );

//--- Data routines, used by K8055_Read and K8055_Write -------
//
ULONG ReadNumberOfBytes( ULONG ulDevpointer,
                         BYTE byNumberOfBytes,
                         BYTE *toData          );

ULONG WriteNumberOfBytes( ULONG ulDevpointer,
                          BYTE byNumberOfBytes,
                          BYTE *toData          );

ULONG ReadNumberOfByteIgnor( ULONG ulDevpointer,
                             BYTE byNumberOfBytes );

ULONG Read_8_Bytes( ULONG ulDevpointer );

//
// -- Functions that are not exported ----------- + -- END ----

#endif


