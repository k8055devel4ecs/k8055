//======================================== func.c === BEGIN ===
/**
 * \file  'func.c'
 *
 * \author U. Hinz   , ed.enilno-t@blerdhu
 * \author B. Hennig , ed.gmx@ginneh.b
 *
 * \brief This file contains the fundamental source code
 * of the 'K8055DD.dll' project and aims exclusively at the
 * operational system 'eComStation' (OS/2) !
 *
 * The basics of this project are:
 *  - Accessing the
 *     ' VELLEMAN K8055 USB Experimental Interface Board '
 *    is done via the genecic USB driver 'usbecd.sys'
 *    by Wim Brul.
 *  - All functionality needed by a K8055 board must be put
 *    into a Device Specific Library.
 *  - Because up to three K8055 boards can be connected to a
 *    PC, such a library must allow code sharing and this
 *    is done best with a DLL.
 *
 * Compiling:
 *  |-------- input files ----------|      |-- output file --|
 *
 *  'func.c' + 'func.h' + 'includes'  ---->    'func.obj'
 *
 * How to use the 'OpenWatcom C 1.7' compiler and its options:
 * ( One command line turned vertical )
 *
 * wcc386 ----------- OpenWatcom C/C++ 1.7 Compiler (32-bits)
 *
 * func -------------------------------- File 'func.c' and
 *                                       file 'func.h' are
 *                                       input files.
 *                                       The output file will
 *                                       be 'func.obj'.
 *
 * -i=D:\WATCOM17\h;D:\WATCOM17\h\os2 -- Location of includes
 *
 * -w4  ------------------------------ Warning level
 * -e25 ------------------------------ Stop after 25 errors
 * -zq  ------------------------------ Quiet operation
 * -od  ------------------------------ No Optimisation
 * -d2  ------------------------------ Debug, fully symbolic
 * -bd  ------------------------------ Build a DLL
 * -6s  ------------------------------ Pentium Pro Stack calls
 * -bt=os2 --------------------------- Build target is OS/2
 * -mf  ------------------------------ Flat Memory
 *
 *
 * Linking:
 *  |------ input files ------|      |-- output file --|
 *
 *    'func.obj' + 'K8055.def'  --->    'K8055DD.dll'
 *
 * How to use the 'OpenWatcom C 1.7' linker and its options:
 * ( One command line turned vertical )
 *
 * wlink -------------------- OpenWatcom C/C++ 1.7
 *
 * @K8055.def --------------- If a '@' is used, the following
 *                            name belongs to a file with
 *                            Linker Directives.
 *
 * system os2v2_dll dll ----- This is for an OS/2-DLL
 *
 * initi termi                : ??? : ToDo :
 *
 * file --------------------- 'file' is to mark the beginning
 *                            of an Object File List.
 *  func.obj                  Here just one file: 'func.obj'
 *
 * library ------------------ 'library' shows the beginning
 *                            of a Library File List.
 *
 *  os2386.lib                Containing the OS/2 Dos Calls
 *  ,                         File name separator
 *  clib3s.lib                Open Watcom C Runtime Library
 *
 *
 *
 * \version 1.0.16 -
 * 2011-07-12 string output K8055_InfoStr improved
 * \version 1.0.15 -
 * 2010-11-02 extended error codes partly rolled back
 * \version 1.0.14 -
 * 2010-10-25 error codes extended
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
 * 2010-03-25 new function K8055_InfoStr, info strings,
 * commenting
 * \version 1.0.7 -
 * 2010-02-07 new function K8055_PrepairDACxOut, commenting
 * \version 1.0.5 -
 * 2010-01-22 new function K8055_PrepairDigitalOut(),
 * commenting
 * \version 1.0.4 -
 * 2010-12-14 new function K8055_DecodeDigitalInputs()
 * \version 1.0.3 -
 * 2010-12-13 new function K805_ReadAllInputs()
 * \version 1.0.2 -
 * 2009-12-10 new function K8055_DummyFnc1()
 * \version 1.0.1 -
 * 2009-08-23 new defines and docu rework
 * \version 1.0.0 -
 * 2009-07-01 Init
 */

// --- Includes, not OS/2 specific,  located in '..\h' ---
//
#include <stdio.h>
#include <process.h>
#include <string.h>
#include <i86.h>
#include <dos.h>


#define INCL_DOS


// --- Includes, special for OS/2, located in '..\os2' ---
//
#include <os2.h>

// --- Includes, definitions for this project, -----------
//     constants, error codes and function prototypes
//
#include "func.h"


//-----------------------------------------------------------//
//--- Byte arrays, used by K8055_Init -----------------------//
//    - Used for reading descriptors
//
// --- Setup Paket for the Device Descriptor-------------------
BYTE byGetDevDscr[ SIZEUSBHEADER + 18 ] =
                                      { 0x80,6,0,1,0,0, 18,0 };

// --- Setup Paket for the Configuration Descriptor------------
BYTE byGetConfDscr[ SIZEUSBHEADER + 48 ] =
                                      { 0x80,6,0,2,0,0, 48,0 };

// --- Setup Paket for the Language String Descriptor----------
BYTE byGetLangStrDscr[ SIZEUSBHEADER + 4 ] =
                                       { 0x80,6,0,3,0,0, 4,0 };

// --- Setup Paket for the String2 Descriptor------------------
BYTE byGetString2Dscr[ SIZEUSBHEADER + 20 ] =
                                      { 0x80,6,2,3,9,4, 20,0 };

// --- Setup Pakets for 4 different String Descriptors---------
BYTE byGet1stStrDscr[ SIZEUSBHEADER + 24 ] =
                                      { 0x80,6,1,3,0,0, 24,0 };
BYTE byGet2ndStrDscr[ SIZEUSBHEADER + 24 ] =
                                      { 0x80,6,2,3,0,0, 24,0 };
BYTE byGet3rdStrDscr[ SIZEUSBHEADER + 24 ] =
                                      { 0x80,6,3,3,0,0, 24,0 };
BYTE byGet4thStrDscr[ SIZEUSBHEADER + 4 ] =
                                      { 0x80,6,4,3,0,0,  4,0 };

// --- Setup Paket for the Unknown21 --------------------------
BYTE by21unknown[ SIZEUSBHEADER + 1 ] =
                                    { 0x21,0x0a,0,0,0,0, 0,0 };

// --- Setup Paket for the Unknown5D --------------------------
BYTE by30unknown[ SIZEUSBHEADER + 30 ] =
                                   { 0x81,6,0,0x22,0,0, 30,0 };


//-----------------------------------------------------------//
//--- Byte array, used by K8055_Init ------------------------//
//    - Used for setting configuration
//
// --- Setup Paket for Setting Configuration Number 1------
BYTE bySetConfigu[ SIZEUSBHEADER ] = { 0x00,9,1,0,0,0,0,0 };



//-----------------------------------------------------------//
//--- ULONG array, used by K8055_Init ------------------------//
//    - Used for storing initialisation error codes
//
ULONG ulInitErrorStore[ 10 ] = { 0,0,0,0,0,0,0,0,0,0 };


//-----------------------------------------------------------//
//--- Byte arrays, used by K8055_Read and K8055_Write -------//
//
// --- Parameter Packet for Reading Data from Endpoint 81--
BYTE byaGetData[ SIZEGETBYTES ] =
                                { 0xEC,0x10,0,0,0x81,3, 8,0 ,
                                   0, 0, 0, 0, 0, 0, 0, 0    };

// --- Parameter Packet for Writing Data to Endpoint 01----
BYTE byaPutData[ SIZEPUTBYTES ] =
                                { 0xEC,0x10,0,0,0x01,3, 8,0 ,
                                  05, 0, 0, 0,  0, 0, 0, 0   };

//-----------------------------------------------------------//
//--- Byte array for K8055 Digital Inputs Decoder -----------//
BYTE byaDigInput[ 6 ] = { 0,16,32,1,64,128 };


//-----------------------------------------------------------//
//--- General purpose variables -----------------------------//
//
ULONG cbTransfer;

//-----------------------------------------------------------//
//--- Info strings ------------------------------------------//
//
CHAR  szString1[INFO_STR_MAX_NUM][INFO_STR_CHARS_MAX] =
                    { "1 abcdefghijklmnopqrstuvwxyzABCD\0",
                      "2 Authors: B. Hennig , U. Hinz  \0",
                      "3 Date: 2011-07-12              \0",
                      "4 DLL-Version: 1.0.16           \0",
                      "5 Licence: BSD                  \0",
                      "6 DLL-Name: K8055DD             \0"  };



//-- ef ------------------------------------------- Begin ---//
//   - Functions to be exported -                            //

//-----------------------------------------------------------1-
//
//
// Export Index 1
//
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
* \return  'ulrc'
*          - Return Code. Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Open( CHAR *pcaDeviceName, ULONG *pulFileDesc )
{
  ULONG ulrc;
  ULONG ulrcDosCall;
  ULONG ulAction;
  //
  ulrc = RET_OKAY;

  ulAction = 0;
  //
  if( (NULL == pulFileDesc)   ||
      (NULL == pcaDeviceName)    )
  {
    ulrc = ulrc | ERROR_POINTER;
    return ulrc;
  }
  //
  ulrcDosCall = DosOpen( pcaDeviceName,
                         pulFileDesc,
                         &ulAction,
                         0, 0, 1, 18, 0 );

  /*
       0 NO_ERROR
       2 ERROR_FILE_NOT_FOUND
       3 ERROR_PATH_NOT_FOUND
       4 ERROR_TOO_MANY_OPEN_FILES
       5 ERROR_ACCESS_DENIED
       12 ERROR_INVALID_ACCESS
       26 ERROR_NOT_DOS_DISK
       32 ERROR_SHARING_VIOLATION
       36 ERROR_SHARING_BUFFER_EXCEEDED
       82 ERROR_CANNOT_MAKE
       87 ERROR_INVALID_PARAMETER
       99 ERROR_DEVICE_IN_USE
      108 ERROR_DRIVE_LOCKED
      110 ERROR_OPEN_FAILED
      112 ERROR_DISK_FULL
      206 ERROR_FILENAME_EXCED_RANGE
      231 ERROR_PIPE_BUSY
  */

  if ( ulrcDosCall != 0 )
  {
    ulrc = ulrc | ERROR_FROM_CALL;
  }

  //
  // printf("\nDosOpen ulrc=%hu ulAction=%hu",ulrc,ulAction);
  return ulrc;
}
//----------1-


//-----------------------------------------------------------2-
//
//
// Export Index 2
//
/**
* \brief 'K8055_Init()' performs all steps that are needed to
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
* \return  'ulrc'
*          - Return Code. Meaning of bits listed below:
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
*  0x010  ERROR_BUFFER     Indicating a bad communication
*                          via Endpoint 0 (EP0) during
*                          initialisation.
*
*
*/
ULONG K8055_Init( ULONG *pulFileDesc )
{
  ULONG ulrc;
  ULONG ulrcSubFunc;
  ULONG ulInitStepIdx;
  ULONG ulArraySize;
  //
  ULONG index;
  BOOL blTestSwitch;
  //
  ulrc = RET_OKAY;
  ulrcSubFunc = RET_OKAY;
  //
  if( NULL == pulFileDesc )
  {
    // --- Return in case of problems.
    //     If the K8055 is not open,
    //     if the parameter is not valid.
    //
    return ERROR_POINTER;
  }

  // -- 1st Step: Reading Device Descriptor ------------
  //             ( from K8055 to PC via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 1;

    ulrcSubFunc = GetDeviceDescriptor( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check --- 8+18 ----
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 18;
              PrintArray56Byte( &byGetDevDscr[0],
                                &ulArraySize      );
            }

    // --- Number of read bytes to be checked: 0018 ---
    if ( ! ( (byGetDevDscr[ 6 ] == 18) &&
             (byGetDevDscr[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }
  //
  // -- 2nd Step: Reading Configuration Descriptor -----
  //              ( from K8055 to PC via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 2;

    ulrcSubFunc = GetConfigurationDescriptor( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check --- 8+41 ----
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 41;
              PrintArray56Byte( &byGetConfDscr[0],
                                &ulArraySize       );
            }

    // --- Number of read bytes to be checked: 0041 ---
    if ( ! ( (byGetConfDscr[ 6 ] == 41) &&
             (byGetConfDscr[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }
  //
  // -- 3rd Step: Reading Language Descriptor ----------
  //              ( from K8055 to PC via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 3;

    ulrcSubFunc = GetLanguageDescriptor( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check --- 8+4 -----
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 4;
              PrintArray56Byte( &byGetLangStrDscr[0],
                                &ulArraySize          );
            }

    // --- Number of read bytes to be checked: 0004 ---
    if ( ! ( (byGetLangStrDscr[ 6 ] == 4) &&
             (byGetLangStrDscr[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }
  //
  // -- 4th Step: Reading 4th String Descriptor --------
  //              ( from K8055 to PC via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 4;

    ulrcSubFunc = Get4thStringDescriptor( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check --- 8+4 -----
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 4;
              PrintArray56Byte( &byGet4thStrDscr[0],
                                &ulArraySize         );
            }

    // --- Number of read bytes to be checked: 0004 ---
    if ( ! ( (byGet4thStrDscr[ 6 ] == 4) &&
                     (byGet4thStrDscr[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }
  //
  // -- 5th Step: Reading 2nd String Descriptor --------
  //              ( from K8055 to PC via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 5;

    ulrcSubFunc = GetString2Descriptor( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check  --- 8+20 ---
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 20;
              PrintArray56Byte( &byGetString2Dscr[0],
                                &ulArraySize          );
            }

    // --- Number of read bytes to be checked: 0020 ---
    if ( ! ( (byGetString2Dscr[ 6 ] == 20) &&
             (byGetString2Dscr[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }
  //
  // -- 6th Step: Setting Configuration ----------------
  //              ( from PC to K8055 via EP0 )
  //    K8055 has got only one (1) configuration.
  //    Nevertheless, K8055 must be activated this way.
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 6;

    ulrcSubFunc = SetConfiguration( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check  --- 8+0 ----
            //   SetConfiguration() has got no answer!
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 0;
              PrintArray56Byte( &bySetConfigu[0],
                                &ulArraySize      );
            }

    // --- Number of read bytes to be checked: 0000 ---
    if ( ! ( (bySetConfigu[ 6 ] == 0) &&
             (bySetConfigu[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(30);
  }
  //
  // -- 7th Step: Writing an unknown Setup Packet ------
  //              ( from PC to K8055 via EP0 )
  //
  blTestSwitch = FALSE;      //Blocked, unknown problem
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 7;

    ulrcSubFunc = DoUnknown21( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check  --- 8+0 ----
            //   DoUnknown21() has got no answer!
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 1;
              PrintArray56Byte( &by21unknown[0],
                                &ulArraySize     );
            }

    // --- Number of read bytes to be checked: 0000 ---
    if ( ! ( (by21unknown[ 6 ] == 1) &&
             (by21unknown[ 7 ] == 0)     ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(21);
  }
  //
  // -- 8th Step: Writing an unknown Setup Packet ------
  //              ( from PC to K8055 via EP0 )
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 8;

    ulrcSubFunc = DoUnknown30Bytes( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check  --- 8+30 ---
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 30;
              PrintArray56Byte( &by30unknown[0],
                                &ulArraySize     );
            }

    // --- Number of read bytes to be checked: 0029 ---
    if ( ! ( (by30unknown[ 6 ] == 29) &&
             (by30unknown[ 7 ] == 0)     ) )

    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    //--------
    delay(19);
  }


  // -- Extra time allowing K8055 to get ready with the
  //    internal initialisation processes.
  //    How long it takes in fact ? - No idea !
  //
  delay(80);


  // -- 9th Step: Reading data after initialisation ----
  //              ( from PC to K8055 )
  //    This step is not done with a Setup Packet.
  //    It is a normal data transfer operation, going
  //    via Endpoint 81 (EP81).
  //
  blTestSwitch = TRUE;
  if ( blTestSwitch == TRUE )
  {
    ulInitStepIdx = 9;

    ulrcSubFunc = Read_8_Bytes( *pulFileDesc );
    ulInitErrorStore[ ulInitStepIdx ] = ulrcSubFunc;

            // -- Testaid ---- Answer check  --- 8+8 ---
            blTestSwitch = FALSE;
            if ( blTestSwitch == TRUE )
            {
              ulArraySize = 8 + 8;
              PrintArray56Byte( &byaGetData[0],
                                &ulArraySize    );
            }

    // -- Number of read bytes to be checked: 0008 ---
    if ( ! ( (byaGetData[ 6 ] == 8) &&
             (byaGetData[ 7 ] == 0)    ) )
    {
      ulrc = ulrc | ERROR_BUFFER;
    }

    // ------
    sleep(1);
  }


  // -- If one of the initalisation steps
  //    did not work, ERROR_INIT is set.
  //
  for ( index = 1; index <= 9; index++ )
  {
    if ( ulInitErrorStore[ index ] != 0 )
    {
      ulrc = ulrc | ERROR_INIT;
    }

  }

  // -- If read bytes were missing, during the
  //    initialisation steps, ERROR_INIT is set.
  if ( ( ulrc & ERROR_BUFFER ) != 0 )
  {
    ulrc = ulrc | ERROR_INIT;
  }

  // -- Testaid ---- Error Code ckeck ---
  blTestSwitch = FALSE;
  if ( blTestSwitch == TRUE )
  {
    ulArraySize = 10;
    PrintArray10Ulong( &ulInitErrorStore, &ulArraySize );
    printf("\n Error Code check: ");
    printf( " %04X", ulrc );
  }

  // ToDo : ERROR_FROM_CALL

  return ulrc;
}
//----------------2-


//-----------------------------------------------------------3-
//
//
// Export Index 3
//
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
* \return  - Return Code. Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Read( ULONG *pulFileDesc,
                  BYTE byNumberOfBytes,
                  BYTE *pbyaData        )
{
  ULONG ulRc;
  ULONG ulRcInnerCall;
  //
  ulRc = RET_OKAY;
  //
  if( (NULL == pulFileDesc) || (NULL == pbyaData) )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  ulRcInnerCall = ReadNumberOfBytes( *pulFileDesc,
                                     byNumberOfBytes,
                                     pbyaData         );

  if ( ulRcInnerCall == ERROR_BUFFER )
  {
    ulRc = ulRcInnerCall;
    return ulRc;
  }


  if ( ulRcInnerCall != 0 )
  {
    ulRc = ulRc | ERROR_FROM_CALL;
  }

  return ulRc;
}
//----------3-


//-----------------------------------------------------------4-
//
//
// Export Index 4
//
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Write( ULONG *pulFileDesc,
                   BYTE byNumberOfBytes,
                   BYTE *pbyaPutData     )
{
  ULONG ulRc;
  ULONG ulRcInnerCall;
  //
  ulRc = RET_OKAY;
  //
  if( (NULL == pulFileDesc) || (NULL == pbyaPutData) )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  ulRcInnerCall = WriteNumberOfBytes( *pulFileDesc,
                                      byNumberOfBytes,
                                      pbyaPutData      );

  if ( ulRcInnerCall == ERROR_BUFFER )
  {
    ulRc = ulRcInnerCall;
    return ulRc;
  }


  if ( ulRcInnerCall != 0 )
  {
    ulRc = ulRc | ERROR_FROM_CALL;
  }

  return ulRc;
}
//----------4-


//-----------------------------------------------------------5-
//
//
// Export Index 5
//
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
* \return  'ulrc'
*          - Return Code. Meaning of bits listed below:
*
*  0x000  RET_OK           Call returned with no error.
*
*  0x002  ERROR_POINTER    Indicating parameter problems.
*
*
*/
ULONG K8055_Close( ULONG *pulFileDesc )
{
  ULONG ulrc;
  ULONG ulrcDosCall;
  //
  ulrc = RET_OKAY;

  if( NULL == pulFileDesc )
  {
    ulrc = ulrc | ERROR_POINTER;
    return ulrc;
  }

  ulrcDosCall = DosClose( *pulFileDesc );

  /* 'ulrcDosCall' can have the values listed here:

     0 NO_ERROR
     2 ERROR_FILE_NOT_FOUND
     5 ERROR_ACCESS_DENIED
     6 ERROR_INVALID_HANDLE

  */

  if ( ulrcDosCall != 0 )
  {
    ulrc = ulrc | ERROR_FROM_CALL;
  }


  return ulrc;
}
//----------5-


//-----------------------------------------------------------6-
//
//
// Export Index 6
//
/**
* \brief The function 'K8055_DummyFunc()' is for testing
* purposes only. It provides a kind of Function Skeleton
* with no particular meaning.
*
* \param   'pulDummyParam'
*          - A function without any parameter is no fun at all.
*          'K8055_DummyFunc' doesn't do anything at all.
*
* \return  'ulrc'
*          - Return Code. This function has got no error
*          handling. The only meaning is mentioned below:
*
*  0x000  RET_OK
*
*
*/
ULONG K8055_DummyFnc( ULONG *pulDummyParam )
{
  ULONG ulrc;
  //
  ulrc = RET_OKAY;

  return ulrc;
}
//----------6-


//-----------------------------------------------------------7-
//
// Export Index 7
//
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
* \return  'ulrc1'
*          - Return Code. Meaning listed below:
*
*  0x000  RET_OK            Call returned with no error.
*
*  0x002  ERROR_POINTER     Indicating parameter problems.
*
*
*/
ULONG K8055_DummyFnc1( ULONG *pulDummyParam1 )
{

  ULONG ulrc1;
  ULONG *pulDummyVar1;
  ULONG ulDummyVar1;
  BOOL blTestSwitch1;
  //
  ulrc1 = RET_OKAY;


  if( NULL == pulDummyParam1 )
  {
    ulrc1 = ulrc1 | ERROR_POINTER;
    return ulrc1;
  }
  //
  ulDummyVar1 = *pulDummyParam1;
  ++ulDummyVar1;
  ++ulDummyVar1;

  blTestSwitch1 = FALSE;
  blTestSwitch1 = TRUE;
  if ( blTestSwitch1 == FALSE )
  {
    printf("Debugging K8055_DummyFnc1: ");
    printf( "%ld", ulDummyVar1);
  }

  pulDummyVar1 = &ulDummyVar1;

  *pulDummyParam1 = ulDummyVar1;

  return ulrc1;
}
//-----------7-


//-----------------------------------------------------------8-
//
// Export Index 8
//
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
* \return  'ulRc'
*          - Return Code. Meaning listed below:
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
ULONG K8055_ReadAllInputs( ULONG *pulFileDesc,
                           ULONG *pulDigitalInputsIx,
                           ULONG *pulAnalogInputA1,
                           ULONG *pulAnalogInputA2    )
{

  ULONG ulRc;
  ULONG ulRcDOScall;
  BYTE  bOldToggleBit;
  BYTE  bNewToggleBit;
  //
  ulRc = RET_OKAY;
  //
  if (
       ( NULL == pulFileDesc ) ||
       ( NULL == pulDigitalInputsIx ) ||
       ( NULL == pulAnalogInputA1 ) ||
       ( NULL == pulAnalogInputA2 )
     )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }


  bOldToggleBit = byaGetData[1] & 0x08;

  ulRcDOScall = DosWrite( *pulFileDesc,
                          &byaGetData[0],
                          16,
                          &cbTransfer     );

       /*
              0 NO_ERROR
              5 ERROR_ACCESS_DENIED
              6 ERROR_INVALID_HANDLE
             19 ERROR_WRITE_PROTECT
             26 ERROR_NOT_DOS_DISK
             29 ERROR_WRITE_FAULT
             33 ERROR_LOCK_VIOLATION
            109 ERROR_BROKEN_PIPE

            0d  0000 0000 0000 0000b  NO_ERROR
            5d  0000 0000 0000 0101b  ERROR_ACCESS_DENIED
            6d  0000 0000 0000 0101b  ERROR_INVALID_HANDLE
           19d  0000 0000 0001 0011b  ERROR_WRITE_PROTECT
           26d  0000 0000 0001 1010b  ERROR_NOT_DOS_DISK
           29d  0000 0000 0001 1101b  ERROR_WRITE_FAULT
           33d  0000 0000 0010 0001b  ERROR_LOCK_VIOLATION
          109d  0000 0000 0110 1101b  ERROR_BROKEN_PIPE
       */

  bNewToggleBit = byaGetData[1] & 0x08;

  // -- Error code if DosWrite() did not work properly --
  //
  //   ToDo : error code ulRcDOScall to be ored !
  //
  if ( ulRcDOScall != NO_DOS_ERROR )
  {
    // -- YesBranch: A problem with 'DosWrite()' --
    //    Results are not valid !
    // ToDo : Last Error Code to be stored !

    ulRc = ulRc | ERROR_FROM_CALL;

    // ToDo : Error code from the DOS call has to be
    //        merged with the error codes of the
    //        project.

  }
  else
  {
    // -- NoBranch: No problem with 'DosWrite()' --
    //    Results are valid, can be used !

    if ( bOldToggleBit != bNewToggleBit )
    {
      // -- Toggle-Bit has changed. -------------
      //    Results are valid, could be used !
      *pulDigitalInputsIx = byaGetData[8];
      *pulAnalogInputA1 = byaGetData[10];
      *pulAnalogInputA2 = byaGetData[11];
    }
    else
    {
      ulRc = ulRc | ERROR_TOGGLE_BIT;
    }
  }


  // -- 'DosWrite()' was OK, but the number
  //    of read bytes may have been incorrect.
  //    This is the test for such a situation.
  if ( byaGetData[6] != 0x08 )
  {
    ulRc = ulRc | ERROR_BYTE_NUMBER;
  }

  return ulRc;
}
//------------8-


//-----------------------------------------------------------9-
//
// Export Index 9
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
* \return  'ulRc'
*          - Return Code. Meaning listed below:
*
*  0x000  RET_OK             Call returned with no error.
*
*  0x002  ERROR_POINTER      Indicating parameter problems.
*
*
*/
ULONG K8055_DecodeDigitalInputs( ULONG *pulDigitalInputsIx,
                                 ULONG *pulDigitalInputsRslt )
{

  ULONG ulRc;
  LONG ilDDIAux;
  LONG ilTempRes;
  //
  ulRc = RET_OKAY;
  //
  if ( (NULL == pulDigitalInputsIx ) ||
       (NULL == pulDigitalInputsRslt)   )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }


  ilTempRes = 0;
  ilDDIAux = *pulDigitalInputsIx;

  ilDDIAux = ilDDIAux - 128;
  if ( ilDDIAux >= 0 )
  {
    ilTempRes = ilTempRes + 16;
  }
  else
  {
    ilDDIAux = ilDDIAux + 128;
  }

  ilDDIAux = ilDDIAux - 64;
  if ( ilDDIAux >= 0 )
  {
    ilTempRes = ilTempRes + 8;
  }
  else
  {
    ilDDIAux = ilDDIAux + 64;
  }

  ilDDIAux = ilDDIAux - 32;
  if ( ilDDIAux >= 0 )
  {
    ilTempRes = ilTempRes + 2;
  }
  else
  {
    ilDDIAux = ilDDIAux + 32;
  }

  ilDDIAux = ilDDIAux - 16;
  if ( ilDDIAux >= 0 )
  {
    ilTempRes = ilTempRes + 1;
  }
  else
  {
    ilDDIAux = ilDDIAux + 16;
  }

  ilDDIAux = ilDDIAux - 8;
  if ( ilDDIAux >= 0 )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }
  else
  {
    ilDDIAux = ilDDIAux + 8;
  }

  ilDDIAux = ilDDIAux - 4;
  if ( ilDDIAux >= 0 )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }
  else
  {
    ilDDIAux = ilDDIAux + 4;
  }

  ilDDIAux = ilDDIAux - 2;
  if ( ilDDIAux >= 0 )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }
  else
  {
    ilDDIAux = ilDDIAux + 2;
  }

  ilDDIAux = ilDDIAux - 1;
  if ( ilDDIAux >= 0 )
  {
    ilTempRes = ilTempRes + 4;
  }
  else
  {
    ilDDIAux = ilDDIAux + 1;
  }

  *pulDigitalInputsRslt = ilTempRes;

  return ulRc;
}
//----------9-


//----------------------------------------------------------10-
//
// Export Index 10
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
* \return  'ulRc'
*          - Return Code. Meaning listed below:
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
ULONG K8055_CheckDigitalInput( ULONG *pulDigitalInputsIx,
                               ULONG *pulSingleDigitalInResult,
                               ULONG *puInputIndex )
{

  ULONG ulRc;
  LONG  ilCDIAux;
  ULONG iIndexAux;
  LONG  ilTempRes;
  //
  ulRc = RET_OKAY;
  //
  if ( ( NULL == pulDigitalInputsIx ) ||
       ( NULL == pulSingleDigitalInResult ) ||
       ( NULL == puInputIndex ) )
  {
    return ERROR_POINTER;
  }

  ilTempRes = 0;
  ilCDIAux = *pulDigitalInputsIx;
  iIndexAux = *puInputIndex;

  if ( (iIndexAux >= 1) && (iIndexAux <= 5) )
  {
    ilTempRes = ilCDIAux & byaDigInput[iIndexAux];

    if ( ilTempRes > 1 )
    {
      ilTempRes = 1;
    }
  }
  else
  {
    ilTempRes = 2;
    ulRc =  ulRc | ERROR_RANGE;
  }

  *pulSingleDigitalInResult = ilTempRes;

  return ulRc;
}
//---------10-


//----------------------------------------------------------11-
//
// Export Index 11
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
ULONG K8055_SetAllOutputs( ULONG *pulFileDesc )
{

  ULONG ulRc;
  ULONG ulRcDOScall;
  BOOL blTestAid;
  //
  ulRc = RET_OKAY;


  if ( NULL == pulFileDesc )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  // -- Testing K8055_SetAllOutputs() in a way that
  //    is not so risky.
  //
  blTestAid = TRUE;
  blTestAid = FALSE;
  if ( blTestAid == TRUE )
  {
    byaPutData[9] = 0x55;  // Digital Output Byte DO set
    //byaPutData[9] = 0xAA;  // Digital Output Byte DO set
    //byaPutData[9] = 0x01;  // Digital Output Byte DO set

    byaPutData[10] = 0x80; // Analog Output Byte DAC1 set
    byaPutData[11] = 0x80; // Analog Output Byte DAC2 set

    // 'ERROR_TESTERROR' not used here.
    // Not now, perhaps later!
  }
  else
  {
    //empty
  }

  ulRcDOScall = DosWrite( *pulFileDesc,
                          &byaPutData[0],
                          16,
                          &cbTransfer     );

  // -- In case DosWrite() did not work properly -----
  //
  //   ToDo : Error code ulRcDOScall must be merged !
  //
  if ( ulRcDOScall != 0 )
  {
    ulRc = ulRc | ERROR_FROM_CALL;
  }

  return ulRc;
}
//---------11-


//----------------------------------------------------------12-
//
// Export Index 12
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
ULONG K8055_PrepairDigitalOut( ULONG *pulDigitalOutValue )
{

  ULONG ulRc;
  ULONG ulTempValue;
  BOOL  blTestAid;
  BYTE  byaTempForAnUlong[ 8 ];
  //
  ulRc = RET_OKAY;


  if ( NULL == pulDigitalOutValue )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  // --- Eight bits of DOx cannot deal with values greater
  //    than 255 (0xFF).
  //
  ulTempValue = *pulDigitalOutValue;
  if  ( ulTempValue > 255 )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  // --- It is risky to use 'memcpy'. In case of doubts
  //     a swiching facility is needed.
  //
  blTestAid = FALSE;
  blTestAid = TRUE;
  if ( blTestAid == TRUE )
  {
    memcpy( &byaTempForAnUlong[0], pulDigitalOutValue, 1 );
    // ToDo : Size of ULONG = 4 to be considered ! Bug !
    // memcpy( &byaPutData[ 9 ], pulDigitalOutValue, 1 );
    byaPutData[ 9 ] = byaTempForAnUlong[ 0 ];
  }
  else
  {
    ulRc = ulRc | ERROR_TESTERROR;
  }

  return ulRc;
}
//---------12-


//----------------------------------------------------------13-
//
// Export Index 13
/**
* \brief The function 'K8055_PrepairDACxOut()' will change one
* single byte on the DAC1-Position or on the DAC2-Position of
* the array 'byaPutData[]'.
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
                            ULONG *pulOutputIndex   )
{

  ULONG ulRc;
  ULONG ulTempValue;
  ULONG ulIndex;
  BOOL  blTestAid;
  BYTE  byaTempForAnUlong[ 8 ];
  //
  ulRc = RET_OKAY;
  //
  if ( ( NULL == pulDACxOutValue ) ||
       ( NULL == pulOutputIndex )     )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  //--- DAC1 and DAC2 cannot deal with values greater
  //    than 255 (0xFF).
  //
  ulTempValue = *pulDACxOutValue;
  if  ( ulTempValue > 255 )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  //--- DAC1 is selected by OutputIndex = 1 and DAC2 is
  //    selected by OutputIndex = 2.
  //    Without this limiting feature, dangerous situations,
  //    crash including, are possible.
  //
  ulIndex = *pulOutputIndex;
  if ( ( ulIndex >= 1 ) && ( ulIndex <= 2 ) )
  {
    //empty
  }
  else
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  // --- It's risky to use 'memcpy'. In case of doubts
  //     a swiching facility is needed.
  //
  blTestAid = FALSE;
  blTestAid = TRUE;
  if ( blTestAid == TRUE )
  {

    memcpy( &byaTempForAnUlong[ 0 ], pulDACxOutValue, 1 );
    // ToDo : Size of ULONG = 4 to be considered ! Bug !
    //memcpy( &byaPutData[9 + ulIndex], pulDACxOutValue, 1 );
    byaPutData[ 9 + ulIndex ] = byaTempForAnUlong[ 0 ];

  }
  else
  {
    ulRc = ulRc | ERROR_TESTERROR;
  }

  return ulRc;
}
//---------13-


//----------------------------------------------------------14-
//
// Export Index 14
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
                            ULONG *pulCounterIndex   )
{

  ULONG ulRc;
  ULONG ulIndex;
  ULONG ulTempRes;
  //
  ulRc = RET_OKAY;

  if ( ( NULL == pulIxCounterValue ) ||
       ( NULL == pulCounterIndex )      )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  ulTempRes = 0;
  ulIndex = *pulCounterIndex;

  if ( (ulIndex >= 1) && (ulIndex <= 2) )
  {
    if ( ulIndex == 1 ) { ulIndex = 0; }

    ulTempRes = byaGetData[ 13 + ulIndex ];
    ulTempRes = ulTempRes * 256;
    ulTempRes = ulTempRes + byaGetData[ 12 + ulIndex ];

    *pulIxCounterValue = ulTempRes;
  }
  else
  {
    ulRc =  ulRc | ERROR_RANGE;
  }

  return ulRc;
}
//---------14-


//----------------------------------------------------------15-
//
// Export Index 15
//
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
                        ULONG *pulStringIndex )
{

  ULONG ulRc;
  ULONG ulIndex;
  BOOL blIndexInRange;
  BOOL blTestAid;
  //
  ulRc = RET_OKAY;


  if ( ( NULL == pszInfoString ) ||
       ( NULL == pulStringIndex )   )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  ulIndex = *pulStringIndex;

  blIndexInRange = FALSE;
  if ( (ulIndex >= 1) && (ulIndex <= INFO_STR_MAX_NUM) )
  {
    blIndexInRange = TRUE;
  }

  if ( blIndexInRange == FALSE )
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  // --- It is risky to use 'memcpy'. In case of doubts, a
  //     facility to disable it, is needed.
  //
  blTestAid = FALSE;
  blTestAid = TRUE;
  if ( blTestAid == TRUE )
  {
    memcpy( pszInfoString,
            &szString1[ulIndex - 1][0], INFO_STR_CHARS_MAX );
  }
  else
  {
    ulRc = ulRc | ERROR_TESTERROR;
  }

  return ulRc;
}
//---------15-


//----------------------------------------------------------16-
//
// Export Index 16
//
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
* \return  'ulRc'
*          - Return Code. Meaning of bits listed below:
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
                         ULONG *pulByteIndex     )
{

  ULONG ulRc;
  ULONG ulSecIndex;
  ULONG ulByIndex;
  BOOL blTestAid;
  //
  ulRc = RET_OKAY;

  if ( ( NULL == pbyInfoByte ) ||
       ( NULL == pulSectionIndex ) ||
       ( NULL == pulByteIndex )       )
  {
    ulRc = ulRc | ERROR_POINTER;
    return ulRc;
  }

  ulSecIndex = *pulSectionIndex;

  if ( (ulSecIndex >= 1) && (ulSecIndex <= INFO_STR_MAX_NUM) )
  {
    --ulSecIndex;
  }
  else
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  ulByIndex = *pulByteIndex;

  if ( (ulByIndex >= 1) && (ulByIndex <= INFO_STR_CHARS_MAX) )
  {
    --ulByIndex;
  }
  else
  {
    ulRc = ulRc | ERROR_RANGE;
    return ulRc;
  }

  // --- It is risky to use 'memcpy'. In case of doubts, a
  //     facility to disable it, is needed.
  //
  blTestAid = FALSE;
  blTestAid = TRUE;
  if ( blTestAid == TRUE )
  {
    memcpy( pbyInfoByte,
            &szString1[ulSecIndex][ulByIndex], 1 );
  }
  else
  {
    ulRc = ulRc | ERROR_TESTERROR;
  }

  return ulRc;
}
//---------16-



//-- ef --------------------------------------------- End ---//
//        - Functions to be exported -                       //
//-- ef --------------------------------------------- End ---//


//-------Subfunctions of K8055_Init-------------------Begin----


//-------------------------------------------------------------
//
//  1st step of K8055_Init
//
/**
*
* \brief    Reading the Device Descriptor from K8055
*           to PC via Endpoint 0 (EP0 ). This descriptor
*           ( bDescriptorType = 01 )
*           holds basic data obout any USB device.
*           Allways 18 bytes long, it is a kind of
*           'USB device business card'.
*           The most important information obtainable here
*           is 'bNumConfigurations'. Later it must be used
*           for setting the USB device configuration
*           ( see: 'SetConfiguration()' below ).
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcGetDevDrc'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*           0 NO_ERROR
*           5 ERROR_ACCESS_DENIED
*           6 ERROR_INVALID_HANDLE
*           19 ERROR_WRITE_PROTECT
*           26 ERROR_NOT_DOS_DISK
*           29 ERROR_WRITE_FAULT
*           33 ERROR_LOCK_VIOLATION
*           109 ERROR_BROKEN_PIPE
*/
ULONG GetDeviceDescriptor( ULONG devpointer )
{
  ULONG ulRcGetDevDrc;

  // --- Setup Paket for the Device Descriptor ------
  // byGetDevDscr[8+18] = { 0x80,6,0,1,0,0, 18,0 };
  //                                 |
  //             bDescriptorType ----+
  //
  ulRcGetDevDrc = DosWrite( devpointer,
                            &byGetDevDscr[0],
                            ( SIZEUSBHEADER + 18 ),
                            &cbTransfer             );
  //
  return  ulRcGetDevDrc;
}
//----------------------


//-------------------------------------------------------------
//
//  2nd step of K8055_Init
//
/**
*
* \brief    Reading the Configuration Descriptor from K8055
*           to PC via Endpoint 0 (EP0 ). This descriptor
*           ( bDescriptorType = 02 )
*           contains extended information as
*           USB device power consumtion.
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcGetConfDrc'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*             0 NO_ERROR
*             5 ERROR_ACCESS_DENIED
*             6 ERROR_INVALID_HANDLE
*             19 ERROR_WRITE_PROTECT
*             26 ERROR_NOT_DOS_DISK
*             29 ERROR_WRITE_FAULT
*             33 ERROR_LOCK_VIOLATION
*             109 ERROR_BROKEN_PIPE
*/
ULONG GetConfigurationDescriptor( ULONG devpointer )
{
  ULONG ulRcGetConfDrc;

  // --- Setup Paket for the Configuration Descriptor -----
  // byGetConfDscr[8+48] = { 0x80,6,0,2,0,0, 48,0 };
  //                                  |
  //              bDescriptorType ----+
  //
  ulRcGetConfDrc = DosWrite( devpointer,
                             &byGetConfDscr[0],
                             ( SIZEUSBHEADER + 48 ),
                             &cbTransfer             );

  return  ulRcGetConfDrc;
}
//-----------------------


//-------------------------------------------------------------
//
//  3rd step of K8055_Init
//
/**
*
* \brief    Reading the Language Descriptor from K8055
*           to PC via Endpoint 0 (EP0 ). This descriptor
*           ( bDescriptorType = 03 )
*           informs about the USB device language that is used
*           for the string type descriptors, which are also
*           available in the world of USB devices.
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcGet4thStrDrc'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*               0 NO_ERROR
*               5 ERROR_ACCESS_DENIED
*               6 ERROR_INVALID_HANDLE
*               19 ERROR_WRITE_PROTECT
*               26 ERROR_NOT_DOS_DISK
*               29 ERROR_WRITE_FAULT
*               33 ERROR_LOCK_VIOLATION
*               109 ERROR_BROKEN_PIPE
*/
ULONG GetLanguageDescriptor( ULONG devpointer )
{
  ULONG ulRcGetLangDrc;

  // --- Setup Paket for the Configuration Descriptor -------
  // byGetLangStrDscr[8+4] = { 0x80,6,0,3,0,0, 4,0 };
  //                                    |
  //                bDescriptorType ----+
  //
  ulRcGetLangDrc = DosWrite( devpointer,
                             &byGetLangStrDscr[0],
                             ( SIZEUSBHEADER + 4 ),
                             &cbTransfer            );

  return  ulRcGetLangDrc;
}
//-----------------------


//-------------------------------------------------------------
//
//  4th step of K8055_Init
//
/**
*
* \brief    Reading the 4th String Descriptor from K8055
*           to PC via Endpoint 0 (EP0 ). This descriptor
*           does the same thing as the language descriptor.
*           It just uses a different Setup Packet.
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcGet4thStrDrc'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*                 0 NO_ERROR
*                 5 ERROR_ACCESS_DENIED
*                 6 ERROR_INVALID_HANDLE
*                 19 ERROR_WRITE_PROTECT
*                 26 ERROR_NOT_DOS_DISK
*                 29 ERROR_WRITE_FAULT
*                 33 ERROR_LOCK_VIOLATION
*                 109 ERROR_BROKEN_PIPE
*/
ULONG Get4thStringDescriptor( ULONG devpointer )
{
  ULONG ulRcGet4thStrDrc;

  // --- Setup Paket for the Configuration Descriptor -------
  // byGet4thStrDscr[8+4] = { 0x80,6,4,3,0,0, 4,0 };
  //
  ulRcGet4thStrDrc = DosWrite( devpointer,
                               &byGet4thStrDscr[0],
                               ( SIZEUSBHEADER + 4 ),
                               &cbTransfer            );

  return  ulRcGet4thStrDrc;
}
//-------------------------


//-------------------------------------------------------------
//
//  5th step of K8055_Init
//
/**
*   ToDo :
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcGetStr2Drc'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*                   0 NO_ERROR
*                   5 ERROR_ACCESS_DENIED
*                   6 ERROR_INVALID_HANDLE
*                   19 ERROR_WRITE_PROTECT
*                   26 ERROR_NOT_DOS_DISK
*                   29 ERROR_WRITE_FAULT
*                   33 ERROR_LOCK_VIOLATION
*                   109 ERROR_BROKEN_PIPE
*/
ULONG GetString2Descriptor( ULONG devpointer )
{
  ULONG ulRcGetStr2Drc;

  // --- Setup Paket for the Configuration Descriptor -------
  // byGetString2Dscr[8+20] = { 0x80,6,2,3,9,4, 20,0 };
  //
  ulRcGetStr2Drc = DosWrite( devpointer,
                             &byGetString2Dscr[0],
                             ( SIZEUSBHEADER + 20 ),
                             &cbTransfer             );

  return  ulRcGetStr2Drc;
}
//-----------------------


//-------------------------------------------------------------
//
//  6th step of K8055_Init
/**
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcSetConf'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*                     0 NO_ERROR
*                     5 ERROR_ACCESS_DENIED
*                     6 ERROR_INVALID_HANDLE
*                     19 ERROR_WRITE_PROTECT
*                     26 ERROR_NOT_DOS_DISK
*                     29 ERROR_WRITE_FAULT
*                     33 ERROR_LOCK_VIOLATION
*                     109 ERROR_BROKEN_PIPE
*/
ULONG SetConfiguration( ULONG devpointer )
{
  ULONG ulRcSetConf;

  // --- Setup Paket for Setting Configuration Number 1 -----
  // bySetConfigu[8+0] = { 0x00,9,1,0,0,0, 0,0 };
  //
  ulRcSetConf = DosWrite( devpointer,
                          &bySetConfigu[0],
                          ( SIZEUSBHEADER + 0 ),
                          &cbTransfer            );

  return  ulRcSetConf;
}
//--------------------


//-------------------------------------------------------------
//
//  7th step of K8055_Init
//
/**
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcDoUn21'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*                       0 NO_ERROR
*                       5 ERROR_ACCESS_DENIED
*                       6 ERROR_INVALID_HANDLE
*                       19 ERROR_WRITE_PROTECT
*                       26 ERROR_NOT_DOS_DISK
*                       29 ERROR_WRITE_FAULT
*                       33 ERROR_LOCK_VIOLATION
*                       109 ERROR_BROKEN_PIPE
*/
ULONG DoUnknown21( ULONG devpointer )
{
  ULONG ulRcDoUn21;

  // --- Setup Paket for Setting Configuration Number 1 -----
  // by21unknown[8+1] = {0x21,0x0a,0,0,0,0, 1,0};
  //
  ulRcDoUn21 = DosWrite( devpointer,
                         &by21unknown[0],
                         ( SIZEUSBHEADER + 1 ),
                         &cbTransfer            );

  return  ulRcDoUn21;
}
//-------------------


//-------------------------------------------------------------
//
//  8th step of K8055_Init
//
/**
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as the device ID of a
*           previously opened K8055.
*
* \return   'ulRcDoUn30'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*                         0 NO_ERROR
*                         5 ERROR_ACCESS_DENIED
*                         6 ERROR_INVALID_HANDLE
*                         19 ERROR_WRITE_PROTECT
*                         26 ERROR_NOT_DOS_DISK
*                         29 ERROR_WRITE_FAULT
*                         33 ERROR_LOCK_VIOLATION
*                         109 ERROR_BROKEN_PIPE
*/
ULONG DoUnknown30Bytes( ULONG devpointer )
{
  ULONG ulRcDoUn30;
                                         // ToDo : Comment
  // --- Setup Paket for Setting Configuration Number 1 ----
  // by30unknown[8+30] = { 0x81,6,0,0x22,0,0, 30,0 };
  //
  ulRcDoUn30 = DosWrite( devpointer,
                         &by30unknown[0],
                         ( SIZEUSBHEADER + 30 ),
                         &cbTransfer             );

  return  ulRcDoUn30;
}
//-------------------


//-------Subfunctions of K8055_Init---------------------End----


// --- Read_8_Bytes -------------------------------------------
//
//
/**
*
* \brief    If 8 bytes must be read from Endpoint 81
*           this function will perform it via the
*           byte array 'byaGetData[]'. Eight leading
*           bytes form a USB Parameter Packet. The remaining
*           eight bytes belong to the data buffer (underlined).
*
*             0xEC,0x10,0,0,0x81,3,  8,0  ,0,0,0,0,0,0,0,0
*               |    |        |  |   |     ---------------
*               |    |        |  |   |       Data Buffer
*               |    |        |  |   |       for 8 bytes
*               |    |        |  |   |
*               |    |        |  |   +---  bytes to be read
*               |    |        |  |
*               |    |        |  +---- Transfer Type Interrupt
*               |    |        |
*               |    |        +---- Endpoint (EP) 81
*               |    |
*               |    +---- Byte that contains the Toggle Bit.
*               |          If a reading was OK, the bit will
*               |          be inverted.
*               |
*               +---- Signature of a USB Parameter Packet
*
* \param    'devpointer'
*           - This parameter is to specify a particular
*           K8055. It is the same as device ID of a
*           previously opened K8055.
*
* \return   'byaGetData[]'
*           - After error-free reading, this byte array will
*           have an inverted Toggle Bit and a changed buffer
*           content.
*                                   <--- buffer of 8 bytes --->
*                                   ---------------------------
*       0xEC,0x00,0,0,0x81,3, 8,0  ,0xf1,0, 126,23 , 21,0 , 0,1
*              |                      |  |   |  |    |      |
*              +---- Toggle Bit       |  |   |  |    |      |
*                                     |  |   |  |    |      |
*           All Digital Inputs LOW ---+  |   |  |    |      |
*                                        |   |  |    |      |
*                            unknown ----+   |  |    |      |
*                                            |  |    |      |
*                        Analog Input A1 ----+  |    |      |
*                                               |    |      |
*                           Analog Input A2 ----+    |      |
*                                                    |      |
*               21 impulses via Digital Input I1 ----+      |
*                                                           |
*                     256 impulses via Digital Input I2 ----+
*
*
*           'ulRcR8B'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*             0 NO_ERROR
*             5 ERROR_ACCESS_DENIED
*             6 ERROR_INVALID_HANDLE
*            19 ERROR_WRITE_PROTECT
*            26 ERROR_NOT_DOS_DISK
*            29 ERROR_WRITE_FAULT
*            33 ERROR_LOCK_VIOLATION
*           109 ERROR_BROKEN_PIPE
*
*
**/
ULONG Read_8_Bytes(ULONG devpointer)
{
  ULONG ulRcR8B;

  // ------------------------------------------------
  //   Parameter Packet for 8 bytes, to be read from
  //   Endpint 81 as Interrupt Transfer Type.
  //
  //   Thought as a debugging aid.
  //
  // byaGetData[8+8] = { 0xEC,0x10,0,0,0x81,3, 8,0 };
  //
  ulRcR8B = DosWrite( devpointer,
                      &byaGetData[0],
                      SIZEGETBYTES,
                      &cbTransfer     );

  return  ulRcR8B;
}
// -----

// --- ReadNumberOfBytes --------------------------------------
//
//  Read a number of Bytes from Endpoint 81
//  it is a read without later work on the data
//
//  This function is for the same purpose as 'Read_8_Bytes()'.
//  One major differences are to be mentioned:
//     The byte array in which the data buffer is located can
//  be announced freely by the parameter 'BYTE *toData'
//
ULONG ReadNumberOfBytes
            ( ULONG ulDevpointer, BYTE byNumbers, BYTE *toData)
{
  ULONG ulRet = RET_OKAY;
  //
  // we are at endpoint 0x81 and want bulk i
  //
  // BYTE byaGetData[8+8] = { 0xEC,0x10,0,0,0x81,3, 8,0 };
  //
  // we set the bytes to read
  //
  byaGetData[6] = byNumbers;
  //
  //  protect buffer owerflow
  //
  if( byNumbers > SIZEBUFFERMAX )
  {
    return ERROR_BUFFER;
  }
  //
  ulRet = DosWrite( ulDevpointer,
                    &byaGetData[0],
                    ( SIZEUSBHEADER + byNumbers),
                    &cbTransfer                   );
       /*
             0 NO_ERROR
             5 ERROR_ACCESS_DENIED
             6 ERROR_INVALID_HANDLE
            19 ERROR_WRITE_PROTECT
            26 ERROR_NOT_DOS_DISK
            29 ERROR_WRITE_FAULT
            33 ERROR_LOCK_VIOLATION
           109 ERROR_BROKEN_PIPE

           0d  0000 0000 0000 0000b  NO_ERROR
           5d  0000 0000 0000 0101b  ERROR_ACCESS_DENIED
           6d  0000 0000 0000 0101b  ERROR_INVALID_HANDLE
          19d  0000 0000 0001 0011b  ERROR_WRITE_PROTECT
          26d  0000 0000 0001 1010b  ERROR_NOT_DOS_DISK
          29d  0000 0000 0001 1101b  ERROR_WRITE_FAULT
          33d  0000 0000 0010 0001b  ERROR_LOCK_VIOLATION
         109d  0000 0000 0110 1101b  ERROR_BROKEN_PIPE

       */

  //
  // printf("err read %ld \n %s \n",ulRet, byaTheData);

  memcpy(toData, &byaGetData[8], byNumbers);

  return  ulRet;
}
// -----


// --- WriteNumberOfBytes -------------------------------------
//
//  Write up to 8 byte interrupt data to endpiont 2
//
/**
*
*  ToDo :
*
* \return   'ulRetval'
*           - The return value of this function is equivalent
*           to that returned by 'DosWrite()'. All possible
*           code is listed below:
*
*             0 NO_ERROR
*             5 ERROR_ACCESS_DENIED
*             6 ERROR_INVALID_HANDLE
*            19 ERROR_WRITE_PROTECT
*            26 ERROR_NOT_DOS_DISK
*            29 ERROR_WRITE_FAULT
*            33 ERROR_LOCK_VIOLATION
*           109 ERROR_BROKEN_PIPE
*
**/
ULONG WriteNumberOfBytes
           ( ULONG ulDevpointer, BYTE byToWrite, BYTE *toData )
{
  ULONG ulRetval = 0;
  //
  // endpoint 2
  //
  // BYTE byaTheData[255] = {0xEC,0,0,0,2,3,255-8,0};
  // BYTE byPutData[8+8] = { 0xEC,0x18,0,0,0x01,3, 8,0 };
  //
  if( byToWrite > SIZEBUFFERMAX )
  {
    return ERROR_BUFFER;
  }
  //
  // set the datalength to write at the config of the package
  //
  byaPutData[6] = byToWrite;
  //
  // copy the data to write buffer
  //
  memcpy( &byaPutData[8], toData, byToWrite);
  //
  // now write the data
  //
  ulRetval = DosWrite( ulDevpointer,
                       //&byaTheData[0],
                       &byaPutData[0],
                       ( SIZEUSBHEADER + byToWrite ),
                       &cbTransfer                    );
       /*
             0 NO_ERROR
             5 ERROR_ACCESS_DENIED
             6 ERROR_INVALID_HANDLE
            19 ERROR_WRITE_PROTECT
            26 ERROR_NOT_DOS_DISK
            29 ERROR_WRITE_FAULT
            33 ERROR_LOCK_VIOLATION
           109 ERROR_BROKEN_PIPE

           0d  0000 0000 0000 0000b  NO_ERROR
           5d  0000 0000 0000 0101b  ERROR_ACCESS_DENIED
           6d  0000 0000 0000 0101b  ERROR_INVALID_HANDLE
          19d  0000 0000 0001 0011b  ERROR_WRITE_PROTECT
          26d  0000 0000 0001 1010b  ERROR_NOT_DOS_DISK
          29d  0000 0000 0001 1101b  ERROR_WRITE_FAULT
          33d  0000 0000 0010 0001b  ERROR_LOCK_VIOLATION
         109d  0000 0000 0110 1101b  ERROR_BROKEN_PIPE

       */

  //
  // byaGetData[1] = byaPutData[1];
  /* printf( "err read %ld toggle %d\n %s \n",
           ulRetval,
           byaTheData[1],
           byaTheData                        ); */
  //
  return  ulRetval;
}
// -----


//-------------------------------------------------------------
//
// Debugging Tool, used to probe (display) BYTE typed
// variables.
// Restricted for byte arrays not larger than 56 bytes.
//
VOID PrintArray56Byte( BYTE *pbyTheByteArray,
                       ULONG *pulSizeOfByteArray )
{
  ULONG index;
  ULONG indexMax;
  BYTE byTheByte;

  indexMax = *pulSizeOfByteArray;

  if ( indexMax > 8 + 48  )
  {
    indexMax = 8 + 48;
  }

  printf( "\n" );
  for ( index = 0; index < indexMax; index++ )
  {
    byTheByte = *( pbyTheByteArray + index );
    printf( " %02X", byTheByte );
  }

}
// -----


//-------------------------------------------------------------
//
// Debugging Tool, used to probe (display) ULONG typed
// variables.
// Tuned for up to 10 values located in an array.
//
VOID PrintArray10Ulong( ULONG *pulTheUlongArray,
                        ULONG *pulSizeOfUlongArray )
{
  ULONG index;
  ULONG indexMax;
  ULONG ulTheUlongValue;

  indexMax = *pulSizeOfUlongArray;

  if ( indexMax > 10 )
  {
    indexMax = 10;
  }

  printf( "\n" );
  for ( index = 0; index < indexMax; index++ )
  {
    ulTheUlongValue = *( pulTheUlongArray + index );
    printf( " %04X", ulTheUlongValue );
  }

}
// -----

//========================================== func.c === END ===
