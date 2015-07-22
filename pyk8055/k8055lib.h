
#ifndef __K8055DDLIB_FUNC_
#define __K8055DDLIB_FUNC_

#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
//

//
// application lib for K8055
//
#define RET_OKAY 	 0x0

#define ERROR_INIT	 0x1
#define ERROR_POINTER    0x2
#define ERROR_INTERN     0x4

#define ERROR_BUFFER       0x10
#define ERROR_BYTE_NUMBER  0x20
#define ERROR_TOGGLE_BIT   0x40


//--- K8055_Open ----------------------------------------------
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
APIRET APIENTRY K8055_Open( CHAR *pcaDrivername,
                            ULONG *pulFileDesc   );


//--- K8055_init ----------------------------------------------
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


//--- K8055_Read ----------------------------------------------
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


//--- K8055_Write ---------------------------------------------
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
                             BYTE byToWrite,
                             BYTE *toData       );


//--- K8055_Close ---------------------------------------------
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


//--- K8055_DummyFunc -----------------------------------------
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


#endif
