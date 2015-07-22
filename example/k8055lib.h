
#ifndef __K8055DDLIB_FUNC_
#define __K8055DDLIB_FUNC_

//
// application lib for K8055
//
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
* - a USB Parameter Packet was wrongly composed or used
* during development.
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


//--- K8055_Open ----------------------------------------------
//                                             Import Index 1
/**
* The function 'K8055_Open' will open one USB Device K8055 for
* later read and write operations.
* Those operations include initialisation (Setup) and
* data exchange.
*
* \param   pcaDeviceName
*          - Pointer to the character array that keeps the
*          Device Name.
*          It is the same device name, that follows the
*          N-Parameter in the 'CONFIG.SYS' statement.
*          (device = ..\usbecd.sys /D:10CF:5500:0000 /N:KDEV01)
*
* \param   pulFileDesc
*          - After 'K8055_Open' was called successfully,
*          it contains a File ID that represents that
*          particular device within an application.
*          The opened K8055 remains open untill it will
*          be closed by 'K8055_Close'.
*
* \return  A very simple error code is returned.
*          If there was no error, the function will give back
*          'RET_OKAY'. Errors of any other kind are covered by
*          the constant 'ERROR_POINTER'.
*/
APIRET APIENTRY K8055_Open( CHAR *pcaDeviceName,
                            ULONG *pulFileDesc   );
// ------------------------------------------------I1



//--- K8055_init ----------------------------------------------
//                                             Import Index 2
/**
* The function 'K8055_init' performs all steps that are needed
* to initialise K8055. First, device descriptors are read,
* then the configuration is set, and finally vendor requests
* are done. If this was done successfully, K8055 is usable.
*
* \param   pulFileDesc
*          - An application must consider, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be initialised.
*
* \return  The return value 'RET_OKAY' stands for 'no error'
*          if every single actions within 'K8055_Init' was OK.
*          Any other case will show 'ERROR_POINTER' instead.
*
*/
APIRET APIENTRY K8055_Init( ULONG *pulFileDesc );
// ----------------------------------------------I2



//--- K8055_Read ----------------------------------------------
//                                             Import Index 3
/**
* Reading data from a K8055 is possible after its
* initialisation was done by K8055_Init. Every 10 miliseconds,
* 8 new data bytes are ready for reading.
*
* The position of them and their meaning are:
*  { In xx A1 A2 I1cL I1cH I2cL I2cH }
*     In - Digital Input
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
* K8055_Read will not extract or decode single data bytes
* for convinience.
* Applications that want to make use of them have to do
* this as appropriate for their special needs.
*
* The function K8055_Read reads the 8 data bytes from
* a buffer. How big that buffer is and where it is, must be
* told by the parameters 'byNumberOfBytes' and '*pbyaGetData'.
*
* \param   pulFileDesc
*          - An application must know, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be addressed.
*
* \param   byNumberOfBytes
*          - For K8055, the buffer that will hold the data
*          after reading, is allways 8 bytes long.
*
* \param   pbyaGetData
*          - Where to find the data after reading was done,
*          is given with this parameter. 'pbyaGetData' points
*          at the address of the first byte xx.
*
* \return  RET_OKAY is the return value if reading the K8055
*          was error free. If it crashed, the return value
*          is different. ???
*/
APIRET APIENTRY K8055_Read( ULONG *pulFileDesc,
                            BYTE byNumberOfBytes,
                            BYTE *pbyaGetData     );
// -------------------------------------------------I3



//--- K8055_Write ---------------------------------------------
//                                             Import Index 4
/**
* Writing data to a K8055 is possible after its
* initialisation was carried out by 'K8055_Init'. Then,
* every 10 miliseconds, a new set of 8 bytes can be written.
*
* Within the set their positions and their meaning are:
*  { 05 DO DAC1 DAC2 xx xx xx xx }
*     05 - First byte is allways 0x05 - meaning unknown
*     DO - Digital Output (0..255)
*     DAC1 - Analog Output 1 (0..255)
*     DAC2 - Analog Output 2 (0..255)
*     xx - Unknown
*
* The function 'K8055_Write' transfers the 8 data bytes from
* a buffer to the USB Driver 'USBECD.SYS'. That this buffer
* has 8 bytes and where that buffer is, must be told by the
* parameters 'byNumberOfBytes' and '*pbyaPutData'.
*
* \param   pulFileDesc
*          - An application must know, that more than one
*          K8055 may exist. With this parameter a particular
*          K8055 can be addressed.
*
* \param   byNumberOfBytes
*          - 8 is the number of bytes which will be put into
*          that buffer.
*
* \param   pbyaPutData
*          - Where to find the data before writing,
*          is given with this parameter. '*pbyaPutData' points
*          at the address of the first byte. It is allway
+          0x05 as mentioned above.
*
* \return  RET_OKAY is the return value if writing to the
*          K8055 was error free. If it did not work, the return
*          value is different. ???
*/
APIRET APIENTRY K8055_Write( ULONG *pulFileDesc,
                             BYTE byNumberOfBytes,
                             BYTE *pbyaPutData     );
// --------------------------------------------------I4



//--- K8055_Close ---------------------------------------------
//                                             Import Index 5
/**
* The function 'K8055_Close' will close exactly that
* K8055 which has been opened before. If the action was
* error free, no further interaction is possible with that
* particular K8055 any longer.
* If Initialisation (Setup) or data exchange are needed again,
* the correspondent function 'K8055_Open' must be called.
*
* \param   pcaDeviceName
*          - Pointer to the character array that keeps the
*          Device Name.
*          In the 'CONFIG.SYS' statement it follows the
*          N-Parameter.
*          ( ..\usbecd.sys /D:10CF:5500:0000 /N:KDEV01 )
*
* \return  A very simple error code is returned.
*          If there was no error, the function will give back
*          'RET_OKAY'. Errors of any other kind are given by
*          the constant 'ERROR_POINTER'.
*/
APIRET APIENTRY K8055_Close( ULONG* pulFileDesc );
// -----------------------------------------------I5



//--- K8055_DummyFunc -----------------------------------------
//                                             Import Index 6
/**
* The function 'K8055_DummyFunc' is for testing purposes only.
* It provides a kind of Function Skeleton with no particular
* meaning.
*
* \param   pulDummyParam
*          A function without any parameter is no fun at all.
*          It is just for fun.
*
* \return  Every function must have a return value.
*          Its reason is to have one.
*
*/
APIRET APIENTRY K8055_DummyFnc( ULONG *pulDummyParam );
// ----------------------------------------------------I6



//--- K8055_DummyFunc1 -----------------------------------------
//                                             Import Index 7
/**
* The function 'K8055_DummyFunc1' is for testing purposes only.
* It is not only Function Skeleton, is is also ment to learn,
* how variables can be changed.
*
* \param   pulDummyParam1
*          This parameter points to the variable which is
*          to be changed within the function.
*
* \return  The return value is just the error code as.
*          usual.
*
*/
APIRET APIENTRY K8055_DummyFnc1( ULONG *pulDummyParam1 );
// ------------------------------------------------------I7



//--- K8055_ReadAllInputs -------------------------------------
//                                             Import Index 8
/**
* \brief The function 'K8055_ReadAllInputs' will read the
* digital inputs I1, I2, I3, I4 and I5 (summarised as Ix).
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   pulFileDesc
*          - An application using this function must know,
*          that more than one K8055 may exist. With this
*          parameter a particular K8055 can be addressed.
*
* \param   pulDigitalInputsIx
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0)
*
* \param   pulAnalogInputA1
*          - This parameter points to a variable that contains
*          the state of the first analog input A1 (0..255).
*
* \param   pulAnalogInputA2
*          - This parameter points to another variable
*          containing the state of the second
*          analog input A2 (0..255).
*
* \return  The return value is just the error code as.
*          usual.
*
*/
APIRET APIENTRY K8055_ReadAllInputs
                              ( ULONG *pulFileDesc,
                                ULONG *pulDigitalInputsIx,
                                ULONG *pulAnalogInputA1,
                                ULONG *pulAnalogInputA2    );
// ----------------------------------------------------------I8



//--- K8055_DecodeDigitalInputs -------------------------------
//                                             Import Index 9
/**
* \brief The function 'K8055_DecodeDigitalInputs' will convert
* raw data coming from Ix into comprahecable values.
* digital inputs I1, I2, I3, I4 and I5 (summarised as Ix).
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   pulDigitalInputsIx
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0).
*
* \param   pulDigitalInputsRslt
*          - Pointer to the variable that will contain the
*          result. (see item 'return')
*
* \return  rc
*          - The return value is just the error code as.
*          usual.
*
* \return  pulDigitalInputsRslt
*          - After the function has done its job,
*          the meaning of the digital inputs has changed:
*           (I1: 0x01, I2: 0x02, I3: 0x04, I4: 0x08, I5: 0x10,
*            I6, I7, I8: not used, set to 0).
*
*/
APIRET APIENTRY K8055_DecodeDigitalInputs
                            ( ULONG *pulDigitalInputsIx,
                              ULONG *pulDigitalInputsRslt );
// ---------------------------------------------------------I9



//--- K8055_CheckDigitalInput ---------------------------------
//                                            Import Index 10
/**
* \brief If the state of a single digital input is to be
* checked, the function 'K8055_CheckDigitalInput' is able to
* tell it.
* raw data coming from Ix into comprahecable values.
* digital inputs I1, I2, I3, I4 and I5 (summarised as Ix).
* '..AllInputs' means the function will also read both analog
* inputs A1 and A2 at the same time.
*
* \param   pulDigitalInputsIx
*          - This parameter points to a variable containing
*          the state of the digital inputs in undecoded form.
*           (I1: 0x10, I2: 0x20, I3: 0x01, I4: 0x40, I5: 0x80,
*            I6, I7, I8: not used, set to 0).
*
* \param   pulSingleDigitalInResult
*          - Pointer to the variable that will contain the
*          result. (see item 'return')
*
* \param   pInputIndex
*          - This parameter will select the particular
*          digital input. I1 is selected by the parameter
*          value 1 through I5 by the value 5.
*
* \return  rc
*          - The return value is just the error code as.
*          usual.
*
* \return  pulSingleDigitalInResult
*          - After the function has done its job,
*          the result value 1 indicates a pressed button
*          (Low voltage). If the result value is 0, the
*          button was released (equ. to High voltage).
*          In order to indicate a malfunction the return
*          value can be 2.
*
*/
APIRET APIENTRY K8055_CheckDigitalInput
                       ( ULONG *pulDigitalInputsIx,
                         ULONG *pulSingleDigitalInResult,
                         ULONG *pInputIndex               );
// ---------------------------------------------------------I10



//--- K8055_SetAllOutputs -------------------------------------
//                                            Import Index 11
/**
* \brief The function 'K8055_SetAllOutputs()' will write 16
* bytes consisting of the Parameter Packet and the data
* to the USB device K8055.
*
*     ( Parameter Packet )
*     0xEC, 0x10, 0x00, 0x00, 0x01, 0x03, 0x08, 0x00,
*
*     ( 8 bytes of data )
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
*     xx - Not needed for output, if the first byte has 0x05.
*
* Before one of the K8055 Outputs will finally change, the
* bytes DO or DAC1 or DAC2 must have their exact values tuned
* by some other function!
* This is unavoidable because the data bytes cannot be sent
* separately, they must be written as a complete set of eight.
*
* \param   pulFileDesc
*          - An application using this function must take in
*          account, that more than one K8055 may exist. With
*          this parameter an opened K8055 can be referred to.
*
* ??? result as an item like \param ???
*
* \return  rc
*          - Return Code. Meaning of bits listed below:
*
*   0x000  RET_OK            Call returned with no error.
*
*   0x001  ERROR_INIT         not used
*   0x002  ERROR_POINTER     Indicating parameter problems.
*   0x004  ERROR_INTERN       not used
*
*   0x008   no name          Unpredictable !
*
*   0x010  ERROR_BUFFER       not used
*   0x020  ERROR_BYTE_NUMBER  not used
*   0x040  ERROR_TOGGLE_BIT   not used
*   0x080  ERROR_RANGE        not used
*
*   0x100  ERROR_FROM_CALL   The API-Call 'DosWrite'
*                            returned with error(s).
*
*   0x200  ERROR_TESTERROR   Not used inside this function,
*                            testing aid outside of
*                            'K8055_SetAllOutputs()'.
*
*/
APIRET APIENTRY K8055_SetAllOutputs( ULONG *pulFileDesc );
// -------------------------------------------------------I11



//--- K8055_PrepairDigitalOut ---------------------------------
//                                            Import Index 12
/**
* \brief The function 'K8055_PrepairDigitalOut()' will write
* one single byte onto the DO-Position of the 8 trailing data
* bytes.
* It is not needed to know where the DO-Position is located.
*     Before a K8055 digital output operation can be done,
* that particular byte on the DO-Position must contain its
* desired value. After DO is finally set, another function
* 'K8055_SetAllOutputs()' must be called. Subsequently the
* complete set of 8 databytes is written to the USB device
* K8055.
*
* \param   pulDigitalOutValue
*          - This parameter points to that value, which will
*          appear at the K8055 Digital Outputs.
*
*          K8055 Digital Outputs are low active. If 0x40
*          is sent, the output O7 will have approximately
*          0 Volts. A current of up to 100 mA can be handled.
*          in this situation.
*          At the same time Light Emitting Diode LD7 is on.
*              Allthough this parameter transfers the variable
*          type Unsigned Long, the range 0..255 (0x00..0xff)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
* \return  rc
*          - Return Code. Meaning of bits listed below:
*
*   0x000  RET_OK            Call returned with no error.
*
*   0x001  ERROR_INIT         not used
*   0x002  ERROR_POINTER     Indicating parameter problems.
*   0x004  ERROR_INTERN       not used
*
*   0x008   no name          Unpredictable !
*
*   0x010  ERROR_BUFFER       not used
*   0x020  ERROR_BYTE_NUMBER  not used
*   0x040  ERROR_TOGGLE_BIT   not used
*
*   0x080  ERROR_RANGE       This bit is set, if the value,
*                            pointed to by
*                            'pulDigitalOutValue' is not
*                            in the range 0..255.
*
*   0x100  ERROR_FROM_CALL    not used
*
*   0x200  ERROR_TESTERROR    Not used inside this function,
*                             testing aid outside of
*                             'K8055_PrepairDigitalOut()'.
*
*/
APIRET APIENTRY K8055_PrepairDigitalOut
                             ( ULONG *pulDigitalOutValue );
// --------------------------------------------------------I12



//--- K8055_PrepairDACxOut ------------------------------------
//                                            Import Index 13
/**
* \brief The function 'PrepairDACxOut()' will write one single
* byte onto the DAC1-Position or onto the DAC2-Position of the
* 8 trailing data bytes.
*
* It is not needed to know where the DAC-Positions are located.
*       Before a K8055 digital output operation can be
* performed, these particular bytes must contain their desired
* values. After DAC1 or DAC2 are finally set, another function
* 'K8055_SetAllOutputs()' must be called. Subsequently the
* complete set of 8 databytes is written to the USB device
* K8055.
*
* \param   pulDACxOutValue
*          - This parameter points to that value, which will
*          finally appear at the K8055 Analog Output DAC1 or
*          DAC2.
*          K8055 Analog Outputs can have an output voltage
*          ranging from 0 to app. 5 Volts, devided into 256
*          steps. Starting with 0x00 (0 * 0.0196 V = 0.0000 V)
*          and ending with 0xFF (255 * 0.0196 V = 4.9980 V)
*              Allthough this parameter transfers the variable
*          type Unsigned Long, the range 0..255 (0x00..0xff)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
* \param   pulOutputIndex
*          - Two analog outputs, DAC1 and DAC2 available on
*          K8055, need to be distinguished one from the other.
*          If this parameter transfers the value 1, DAC1 will
*          be selected, 2 will do the same with DAC2.
*              Allthough this parameter transfers the variable
*          type Unsigned Long, the range 1..2 (0x01..0x02)
*          is compulsory. Going over the limit will set the
*          return code bit ERROR_RANGE to 1 (rc = 0x80 = 128).
*
* \return  rc
*          - Return Code. Meaning of bits listed below:
*
*   0x000  RET_OK            Call returned with no error.
*
*   0x001  ERROR_INIT         not used
*   0x002  ERROR_POINTER     Indicating parameter problems.
*   0x004  ERROR_INTERN       not used
*
*   0x008   no name          Unpredictable !
*
*   0x010  ERROR_BUFFER       not used
*   0x020  ERROR_BYTE_NUMBER  not used
*   0x040  ERROR_TOGGLE_BIT   not used
*
*   0x080  ERROR_RANGE      This bit is set, if the value,
*                            pointed to by
*                            'pulDigitalOutValue' is not
*                            in the range 0..255.
*                           This bit is also set, if the value
*                            pointed to by
*                            'pulOutputIndex' is not in the
*                            range 1..2.
*
*   0x100  ERROR_FROM_CALL    not used
*
*   0x200  ERROR_TESTERROR    Not used inside this function,
*                             testing aid outside of
*                             'K8055_PrepairDACxOut()'.
*
*/
APIRET APIENTRY K8055_PrepairDACxOut
                            ( ULONG *pulDACxOutValue,
                              ULONG *pulOutputIndex   );
// -----------------------------------------------------I13



//--- K8055_ReadIxConter --------------------------------------
//
//                                            Import Index 14
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
APIRET APIENTRY K8055_CheckIxCounter
                            ( ULONG *pulIxCounterValue,
                              ULONG *pulCounterIndex    );
// -------------------------------------------------------I14



//--- K8055_InfoStr -------------------------------------------
//
//                                            Import Index 15
// ???
APIRET APIENTRY K8055_GetInfoStr
                                ( CHAR *pszInfoString,
                                  ULONG *pulStringIndex );
// -------------------------------------------------------I15



//--- K8055_InfoBytes -----------------------------------------
//
//                                            Import Index 16
// ???
APIRET APIENTRY K8055_GetInfoByte
                                ( BYTE  *pbyInfoByte,
                                  ULONG *pulSectionIndex,
                                  ULONG *pulTheIndex      );
// ---------------------------------------------------------I16



#endif
