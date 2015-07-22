//-------------------------------------------------------------
//  Demo Application 'k8055e.exe' for the
//
//  ' USB-Interface Board VELLEMAN K8055 '  aka  'VM110'
//
//  It provides tests as Reading and Writing to one K8055.
//
//  Preconditions:
//  - OS:
//    'eComStation 1.0' or 'eComStation 1.2'
//
//  - USB-Driver:
//    'usbecd.sys' by Wim Brul
//
//  - C Compiler:
//    'OpenWatcom C 1.6' or higher
//
//  - Device Specific Library:
//    'K8055DD.dll' by B.Hennig and U.Hinz
//
//  Author:
//    Uwe Hinz , 2011-06-06  ( ed.enilno-t@blerdhu )
//
//  How to compile:
//  ( Command Line turned vertical -Begin- )
//
//   wcc386 ........... OpenWatcom C/C++ Compiler (32-bits)
//
//   main ................. File 'main.c' and
//                          file 'main.h' are
//                          input files.
//                          The output file will
//                          be 'main.obj'.
//
//   -i=D:\WATCOM17\h;
//      D:\WATCOM17\h\os2 ... Location of Includes
//                            ( Look up the Watcom Path )
//
//   -w4  ................. Warning level
//   -e25 ................. Stop of compiling after 25 errors
//   -zq  ................. Quiet operation
//   -od  ................. No optimisation
//   -d2  ................. Debugging fully symbolic
//
//   -6s  ................. Pentium Pro Stack Calls
//   -bt=os2  ............. Build Target: OS/2
//   -mf  ................. Flat Memory
//
//  ( Command Line turned vertical -End- )
//
//  How to link:
//  ( Command Line turned vertical -Begin- )
//
//    wlink ............ OpenWatcom Linker
//
//    SYSTEM os2v2 ..... The executable will be built as a
//                       OS/2 Console Application, 32-bit.
//
//    NAME k8055e ...... Name of the resulting executable.
//                       ( '.exe' will be added automatically )
//
//    FILE ............. 'file' starts a list of
//                       Object Files.
//
//     main.obj ........ Just one object file on the list
//                       ( Created before by a compiler run )
//
//    LIBPATH .   ...... ' . ' stands for the current directory.
//
//    LIBRARY .......... 'library' starts a list of
//                       Library Files.
//
//      K8055DD.lib .... Here only one library is used.
//                       ( Import Library for all exported
//                         functions that are provided
//                         by 'K8055DD.dll' )
//
//  ( Command Line turned vertical -End- )
//
//-------------------------------------------------------------



#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define INCL_DOS
#include <os2.h>
#include <dos.h>

#include "k8055lib.h"  // Contains error codes and
                       // 'K8055DD.dll' functions prototypes.


// -- Function Prototypes ------------------------ BEGIN -----!

ULONG PollingForMenuKeyByCharArray( CHAR *pszMenuKeyCode,
                                    ULONG *pulKeyCode,
                                    ULONG *pulKeyIdx      );

ULONG PrintingGoodByePhrase();

ULONG PrintingInfoHeadLine( CHAR *pszTheDevName );

ULONG PrintingInfo();

ULONG PrintingHeadLine();

ULONG PrintingHeadLineRead( INT iTableCounter,
                            CHAR *pszTheDevName );

ULONG PrintingHeadLineWrite( CHAR *pszTheDevName );

ULONG PrintingCommandLineOptions();

ULONG PrintingDisclaimer();

ULONG PrintingDisclaimerHint();

ULONG PrintingInfoPref();

ULONG PrintingReadPref();

ULONG PrintingWritePref();

// -- Function Prototypes ------------------------ END -------!


// == Main ============================== BEGIN ===============

void main( INT argc, CHAR* *argv )
{

  // -- Option derived variables ----

  INT    iOptChar;   // ASCII value of a
                     // Command Line Option Character

  BOOL   blTestsExplained;
  BOOL   blTestScreens;
  BOOL   blDisclaimerLock;

  // -- Variables concerning the USB Device Driver ----

  ULONG  hDevice;
  ULONG  *phDevice;
  CHAR   pszName[9] = "$"; // Refers to USBECD.SYS /N:$

  // -- K8055 input variables ----

  ULONG  ulInpIxValue;
  ULONG  ulInpA1value, ulInpA2value;
  ULONG  ulDecodedInpIxValue;
  ULONG  ulCounterValue;
  ULONG  ulCounterIndex;

  // -- K8055 output variables ----

  ULONG  ulDigitalOutValue;
  ULONG  ulDAC1value;
  ULONG  ulDAC2value;
  ULONG  ulDACidx;

  // -- Variables for user interaction ----

  BOOL   blDoTheMainMenuAgain;
  CHAR   szCmdKeys[33];
  ULONG  ulKeyCode;
  ULONG  ulKeyIdx;

  // -- Variables used in the Read-Test ----

  INT    iTableIdx;
  INT    iBlckCycl;
  ULONG  ulDisplayedValuesCount;

  // -- Variables used in the Read-Test ----

  INT    iByteCycl;

  // -- Variables for specific purposes ----
  //
  ULONG ulrc;       // Return Code for functions



  // -- Initialising Variables ---------- BEGIN ====

  blTestsExplained = FALSE;
  blTestScreens = TRUE;
  blDisclaimerLock = FALSE;

  blDoTheMainMenuAgain = TRUE;

  // -- Initialising Variables ------------ END ====


  // -- Processing Command Line Options ------------- BEGIN --/
  //
  while( ( iOptChar = getopt( argc, argv, "dihn:p?" ) ) != -1 )
  {
    switch( iOptChar )
    {
      case 'd':
        blDisclaimerLock = TRUE;
        break;

      case 'i':
        // -- Basic information on 'K8055DD.dll'.
        //
        blTestScreens = FALSE;
        printf("\n");
        ulrc = PrintingInfo();
        printf("\n");
        break;

      case 'h':
        // -- Printing list of options.
        //
        blTestScreens = FALSE;
        ulrc = PrintingCommandLineOptions();
        break;

      case 'n':
        // -- Passing the Device Name to the application.
        //
        strcpy( pszName, optarg );
        // -- Testaid -----
        // printf( "Device Name Option: '%s' ", pszName );
        blTestScreens = TRUE;
        break;

      case 'p':
        // -- Explaining every test in form of an intro.
        //
        blTestsExplained = TRUE;
        break;

      case '?':
        blTestScreens = FALSE;
        ulrc = PrintingCommandLineOptions();
        break;
    }
  }

  // -- Resolving Option Inconsistancy ------------

  // -- Testaid -----
  printf( "\n  Number of Cmd Line Options: '%#02d' ", argc );

  if ( argc == 1 )
  {
    blTestScreens = FALSE;
    ulrc = PrintingCommandLineOptions();
  }

  // -- 'exit(0)' Testaid for argument scanning ---
  //exit(0);


  // -- Processing Command Line Options --------------- END --/



  // -- Main Menu Key Section ----------------------- BEGIN --|

  if ( blTestScreens == TRUE )
  {

    // -- Main Menu Key Loop --------------------------- BL -))
    do
    {

      ulrc = PrintingHeadLine();

      if ( blDisclaimerLock == FALSE )
      {
        ulrc = PrintingDisclaimer();
        ulrc = PrintingDisclaimerHint();

        ulrc = PrintingGoodByePhrase();

        exit(0);

      }

      // -- The Main Menu -----------------------------------
      //
      printf("   Main menu \n\n");

      printf("\n     Reading from K8055.....................");
      printf("............. r ");
      printf("\n     Writing to K8055.......................");
      printf("............. w ");
      printf("\n ");
      printf("\n     Info on the Device Specific Library ");
      printf("K8055DD.dll .... i ");
      printf("\n     Disclaimer.............................");
      printf("............. d ");
      printf("\n     Quit...................................");
      printf("............. q ");
      printf("\n\n");
      printf("\n      Press one of the listed keys !");
      printf("\n");

      strcpy( szCmdKeys, "iIdDqQrRwW" );
      ulKeyCode = 0;
      ulKeyIdx = 0;
      ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                           &ulKeyCode,
                                           &ulKeyIdx   );

      // -- Testaid ---
      //    to check all return conditions of
      //    'PollingForMenuKeyByCharArray()'
      //
      //printf( "%hu , ", ulrc );
      //printf( "%hu , ", ulKeyCode );
      //printf( "%hu", ulKeyIdx );
      //exit(0);

      // -- Main Menu Function Selector --------- BEGIN ------&
      //
      switch( ulKeyCode )
      {
        //-- Showing Disclaimer -----------------------------
        //
        case 'd':
        case 'D':

          ulrc = PrintingHeadLine();
          ulrc = PrintingDisclaimer();

          printf("\n\n  Press  m  ");
          printf("to return to the Main Menu \n");
          strcpy( szCmdKeys, "mM" );
          ulKeyCode = 0;
          ulKeyIdx = 0;
          ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                               &ulKeyCode,
                                               &ulKeyIdx   );

          blDoTheMainMenuAgain = TRUE;

          break; //-- Showing Disclaimer, ready -------------

        //-- Showing info on 'K8055DD.dll' ------------------
        //
        case 'i':
        case 'I':

          if ( blTestsExplained == TRUE )
          {

            ulrc = PrintingHeadLine();
            ulrc = PrintingInfoPref();

            printf( "\n\n  Press  i  to start test \n" );
            strcpy( szCmdKeys, "iI" );
            ulKeyCode = 0;
            ulKeyIdx = 0;
            ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                                 &ulKeyCode,
                                                 &ulKeyIdx   );
          }

          ulrc = PrintingHeadLine();
          ulrc = PrintingInfoHeadLine( &pszName );

          // This function will call 'K8055_GetInfoStr()'
          ulrc = PrintingInfo();

          printf( "\n\n  Press  m  " );
          printf( "to return to the Main Menu \n" );
          strcpy( szCmdKeys, "mM" );
          ulKeyCode = 0;
          ulKeyIdx = 0;
          ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                               &ulKeyCode,
                                               &ulKeyIdx   );

          blDoTheMainMenuAgain = TRUE;

          break; //-- Showing info on 'K8055DD.dll', ready --

        //-- Test: Reading from K8055 -----------------------
        //
        case 'r':
        case 'R':

          if ( blTestsExplained == TRUE )
          {

            ulrc = PrintingHeadLine();
            ulrc = PrintingReadPref();

            printf( "\n\n  Press  r   to start the test \n" );

            strcpy( szCmdKeys, "rR" );
            ulKeyCode = 0;
            ulKeyIdx = 0;
            ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                                 &ulKeyCode,
                                                 &ulKeyIdx   );

          }

          phDevice = &hDevice;

          ulrc = K8055_Open( &pszName[0], phDevice );
          if (ulrc != 0)
          {
            printf( "\n\n Cannot open device " );
            printf( " '%s' ! ", pszName );

            ulrc = PrintingGoodByePhrase();
            exit(0);
          }
          else
          {

            // ----------------------------------------------
            // 'K8055_Init' reads descriptors and sets
            // the configuration as a K8055 needs it.
            //
            ulrc = K8055_Init( phDevice );
            if ( ulrc == 0 )
            {


              // -- Displays 5 tables of 5 value lines ---LB-
              //
              for( iTableIdx = 1;
                   iTableIdx <= 5;
                   iTableIdx++     )
              {

                // -- Extra time between two value tables.
                //
                sleep(1);

                ulrc = PrintingHeadLine();
                ulrc = PrintingHeadLineRead( iTableIdx,
                                             &pszName   );

                // -- Displays 5 value lines -------------LB-
                //
                for( iBlckCycl = 0;
                     iBlckCycl <= 5;
                     iBlckCycl++     )
                {

                  ulInpIxValue = 1;
                  ulInpA1value = 0;  ulInpA2value = 0;
                  ulrc = K8055_ReadAllInputs( phDevice,
                                              &ulInpIxValue,
                                              &ulInpA1value,
                                              &ulInpA2value  );

                  printf( "\n      " );
                  printf( "%#04x,   ", ulInpIxValue );

                  ulrc = K8055_DecodeDigitalInputs
                                      ( &ulInpIxValue,
                                        &ulDecodedInpIxValue );

                  printf( "%#04x  ",   ulDecodedInpIxValue );
                  printf( "  ( %#06b )", ulDecodedInpIxValue );
                  printf( " �     " );
                  printf( "%#03d", ulInpA1value );

                  printf( "    " );
                  printf( "%#03d", ulInpA2value );

                  // -- Time between two value lines is app.
                  //    the same time as between two reading
                  //    operations.
                  //
                  sleep(1);

                }
                // -- Displays 5 value lines -------------LE-

              }
              // -- Displays 5 tables of 5 value lines ---LE-

              ulCounterValue = 0;
              // -- Obtaining impulses counted on I1 --
              ulCounterIndex = 1;
              ulrc = K8055_CheckIxCounter( &ulCounterValue,
                                           &ulCounterIndex );

              printf( "\n  Impulses counted on I1 =  " );
              printf( "%#06d ", ulCounterValue);

              // -- Obtaining impulses counted on I2 --
              ulCounterIndex = 2;
              ulrc = K8055_CheckIxCounter( &ulCounterValue,
                                           &ulCounterIndex );

              printf( "\n  Impulses counted on I2 =  " );
              printf( "%#06d ", ulCounterValue);

              ulrc = K8055_Close( phDevice );
              printf( " \n");
              printf( " Device '%s' closed.", pszName );

            }
            printf( "\n" );
            printf( " Test -Read- completed !" );

          }

          printf( "\n\n  Press  m  " );
          printf( "to return to the Main Menu \n" );
          strcpy( szCmdKeys, "mM" );
          ulKeyCode = 0;
          ulKeyIdx = 0;
          ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                               &ulKeyCode,
                                               &ulKeyIdx   );

          blDoTheMainMenuAgain = TRUE;

          break; //-- Test: Reading from K8055, ready -------

        //-- Test: Writing to K8055 -------------------------
        //
        case 'w':
        case 'W':

          if ( blTestsExplained == TRUE )
          {

            ulrc = PrintingHeadLine();
            ulrc = PrintingWritePref();

            printf( "\n\n  Press  w  to start the test \n" );

            strcpy( szCmdKeys, "wW" );
            ulKeyCode = 0;
            ulKeyIdx = 0;
            ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                                 &ulKeyCode,
                                                 &ulKeyIdx   );

          }

          ulrc = PrintingHeadLine();
          ulrc = PrintingHeadLineWrite( &pszName );

          phDevice = &hDevice;

          ulrc = K8055_Open( &pszName[0], phDevice );
          if ( ulrc != 0 )
          {
            printf( "\n\n Cannot open device " );
            printf( " '%s' ! ", pszName );

            ulrc = PrintingGoodByePhrase();
            exit(0);
          }
          else
          {

            // ----------------------------------------------
            // 'K8055_Init' reads descriptors and sets the
            // configuration as a K8055 needs it.
            //
            ulrc = K8055_Init( phDevice );
            if ( ulrc == 0 )
            {

              ulDisplayedValuesCount = 0;
              for( iByteCycl = 0;
                   iByteCycl <= 255;
                   iByteCycl++ )
              {

                delay(5);

                ulDigitalOutValue = iByteCycl;
                ulrc = K8055_PrepairDigitalOut
                                        ( &ulDigitalOutValue );

                // -- Selecting DAC1, going upwards -----
                ulDACidx = 1;
                ulDAC1value = iByteCycl;
                ulrc = K8055_PrepairDACxOut( &ulDAC1value,
                                             &ulDACidx     );

                // -- Selecting DAC2, going downwards ---
                ulDACidx = 2;
                ulDAC2value = 255 - iByteCycl;
                ulrc = K8055_PrepairDACxOut( &ulDAC2value,
                                             &ulDACidx     );

                ulrc = K8055_SetAllOutputs( phDevice );

                if ( ulDisplayedValuesCount == 24 )
                {
                  ulDisplayedValuesCount = 0;
                  printf( "\n" );
                }
                ++ulDisplayedValuesCount;
                printf( " %02X", iByteCycl );

                delay(5);

              }

              ulrc = K8055_Close( phDevice );
              printf( " \n");
              printf( " Device '%s' closed.", pszName );

            }
            printf( "\n" );
            printf( " Test -Write- completed !" );
          }

          printf("\n\n  Press  m  ");
          printf("to return to the Main Menu \n");
          strcpy( szCmdKeys, "mM" );
          ulKeyCode = 0;
          ulKeyIdx = 0;
          ulrc = PollingForMenuKeyByCharArray( &szCmdKeys,
                                               &ulKeyCode,
                                               &ulKeyIdx   );

          blDoTheMainMenuAgain = TRUE;

          break; //-- Test: Writing to K8055, ready ---------

        //-- Qitting Main Menu ------------------------------
        //
        case 'q':
        case 'Q':

          printf( " \n");
          printf( " -Quit- selected !" );

          blDoTheMainMenuAgain = FALSE;

          break; //-- Quitting Main Menu, ready -------------


      }
      //
      // -- Main Menu Function Selector ----------- END ------&


    } while ( blDoTheMainMenuAgain == TRUE );
    //
    // -- Main Menu Key Loop --------------------------- EL -))

  }
  else
  {
    //empty
  }
  // -- Main Menu Key Section ------------------------- END --|

  ulrc = PrintingGoodByePhrase();

  exit(0);

}
// == Main ================================ END ===============



// ++++++++++++ Routine collection +++++++++++++++ BEGIN ++++++


// -- Polling Menu Key Routine ------------------ BEGIN --_
//
ULONG PollingForMenuKeyByCharArray( CHAR  *pszMenuKeyCode,
                                    ULONG *pulKeyCode,
                                    ULONG *pulKeyIdx       )
{
  ULONG  ulRcPCA;  // Error Code

  INT    iAuxPCA;
  INT    iCountPCA;
  CHAR   szCmdKeys[33];
  INT    iKeyIdxPCA;
  //
  ulRcPCA = 0;
  ulRcPCA = RET_OKAY;

  // -- Parameter check (simple and basic) ---
  if ( ( NULL == pszMenuKeyCode ) ||
       ( NULL == pulKeyCode) ||
       ( NULL == pulKeyIdx )          )
  {
    ulRcPCA = ulRcPCA | ERROR_POINTER;
    return ulRcPCA;
  }

  strcpy( szCmdKeys, pszMenuKeyCode );

  // --- Endless For-Loop (polling) --- LB ---
  //
  for ( iCountPCA = 0; iCountPCA < 1; )
  {
    iAuxPCA = getch();

    // --- Loop, Scanning Allowed Keys -- LB --
    //
    for ( iKeyIdxPCA = 0;
          iKeyIdxPCA <= strlen( szCmdKeys );
          ++iKeyIdxPCA                       )
    {

      // -- Not the right key, back to polling.
      //
      if ( szCmdKeys[ iKeyIdxPCA ] == 0 )
      {
        break;
      }

      if ( iAuxPCA == szCmdKeys[ iKeyIdxPCA ] )
      {
        // ------- Allowed key found !
        //         Leaving the Scanning Loop
        //         and the Polling Loop.
        //
        iCountPCA = 1;
        *pulKeyCode = iAuxPCA;
        *pulKeyIdx = iKeyIdxPCA;
        break;
      }

    }
    // --- Loop, Scanning Allowed Keys -- LE --

  }
  // --- Endless For-Loop ------------- LE ---

  return ulRcPCA;
}
// -- Polling Menu Key Routine -------------------- END --_

// -- Good Bye Phrase ------------ BEGIN --
//
//
ULONG PrintingGoodByePhrase()
{

  printf( "\n End of K8055e.exe \n" );

  return 0;
}
// -- Good Bye Phrase -------------- END --

// -- Info Head Line Routine --------------------- BEGIN --
//
//
ULONG PrintingInfoHeadLine( CHAR *pszTheDevName )
{

  printf( "  -Info- selected,             " );
  printf( "    USB Device Name: '%s' ", pszTheDevName );
  printf("\n");

  return 0;
}
// -- Info Head Line Routine ----------------------- END --


// -- Info Routine --------------------- BEGIN --
//
//
ULONG PrintingInfo()
{
  ULONG  ulRcPI;

  CHAR   szAuxIR[33] = "0........1.........2.........3..\0";
  CHAR*  pszAuxIR;

  ULONG  iIR;
  ULONG  iAuxIR;
  ULONG  aiAuxIR[5] = { 6, 4, 3, 5, 2 };

  printf( "\n" );
  printf( "   Info on the Device Specific Library" );
  printf( " 'K8055DD.dll' " );
  printf( "\n\n" );

  pszAuxIR = &szAuxIR[ 0 ];

  for ( iIR = 0; iIR < 6; ++iIR )
  {

    iAuxIR = aiAuxIR[ iIR ];
    ulRcPI = K8055_GetInfoStr( pszAuxIR, &iAuxIR );
    if (ulRcPI == 0)
    {
      pszAuxIR++;
      printf( "\n       %s", pszAuxIR );
    }

  }

  return 0;
}
// -- Info Routine ----------------------- END --


// -- Headline Routine ----------------- BEGIN --
//
//
ULONG PrintingHeadLine()
{
  printf( "\033[=3h" ); //EscSequcence40x25coloredText
  printf( "\033[2J" );  //EscSequcenceClearScreen
  printf( "  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�" );
  printf( "컴컴컴컴컴컴커\n" );
  printf( "  �           This is a Demo Application for the" );
  printf( "            �\n" );
  printf( "  �           USB-Interface Board VELLEMAN K8055" );
  printf( "            �\n" );
  printf( "  읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�" );
  printf( "컴컴컴컴컴컴켸\n" );

  return 0;
}
// -- Headline Routine ------------------- END --


// -- Read Test Headline Routine ---------- BEGIN --
//
//
ULONG PrintingHeadLineRead( INT iTableCounter,
                            CHAR *pszTheDevName )
{
  printf( "  Test -Read- selected,         " );
  printf( "   USB Device Name: '%s' ", pszTheDevName );
  printf( "\n\n");
  printf( "   Showing table %d / 5", iTableCounter );
  printf( "         Speed: app. 1 line per second" );
  printf( "\n" );
  printf( "\n   Digital Inputs                �" );
  printf( "  Analog Inputs" );
  printf( "\n                                 �" );
  printf( "\n    Ix: raw,  decoded ( binary ) �" );
  printf( "   A1:    A2:" );
  printf( "\n  컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴" );
  printf( "컴컴컴컴컴컴컴" );

  return 0;
}
// -- Read Test Headline Routine ------------ END --


// -- Write Test Headline Routine --------- BEGIN --
//
//
ULONG PrintingHeadLineWrite( CHAR *pszTheDevName )
{
  printf( "  Test -Write- selected,        " );
  printf( "   USB Device Name: '%s' ", pszTheDevName );
  printf( "\n  Time between two I/O actions: app. " );
  printf( "10 milliseconds" );
  printf( "\n");
  printf( "\n Analog Outputs DAC1, DAC2=255-DAC1 and" );
  printf( " Digital Outputs Ox, all together " );
  printf( "\n 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴" );
  printf( "컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�" );

  printf("\n");

  return 0;
}
// -- Write Test Headline Routine ----------- END --


// -- Command Line Screen Routine --------- BEGIN --
//
//
ULONG PrintingCommandLineOptions()
{
  printf("\nK8055e.exe [option] [-n <DeviceName>] [option]");
  printf("\n    List of options:");
  printf("\n-d  Disclaimer switched off.");
  printf("\n-h  This screen, options and parameters listed.");
  printf("\n-?  The same option as -h.");
  printf("\n-n  Meaning that the following parameter is the ");
  printf("Device Name.");
  printf("\n    Device Name is the same as in the config.sys");
  printf(" statement,");
  printf("\n    needed by the Wim Brul driver instance");
  printf("\n    'device = C:<path>usbecd.sys /D:10CF:5500:");
  printf("0000 /N:K8055_$'.");
  printf("\n    Following '/N:', this string is the");
  printf(" Device Name.");
  printf("\n    Command Line Example: '[c:\\]K8055e.exe -n");
  printf(" K8055_$'.");
  printf("\n    If -n is omitted, the application will use");
  printf(" '$' as ");
  printf("\n    a Dummy Device Name instead.");
  printf("\n-p  Preface to every single test. It shows a");
  printf(" little");
  printf("\n    list with DLL functions that are called.");
  printf("\n-i  Basic information concerning the device ");
  printf("\n    specific library K8055DD.dll.");
  printf("\n");

  return 0;
}
// -- Command Line Screen Routine ----------- END --


// -- Disclaimer Screen Routine --------- BEGIN --
//
//
ULONG PrintingDisclaimer()
{
  printf("\n   Disclaimer ");
  printf("\n    ");
  printf("\n    This program is a snapshot of the author's ");
  printf("\n    learning curve. Sharing knowledge ");
  printf("is the only ");
  printf("\n    reason, why it was published.");
  printf("\n    Anyone, who wants to make use of it, ");
  printf("will have ");
  printf("\n    to modify it for individual purposes. ");
  printf("\n\n    Hereby the authors declare themselves ");
  printf("unable to  ");
  printf("\n    provide any warranty concerning any subject ");
  printf("\n    covered by this program. ");
  printf("\n    ");
  printf("\n                     Authors: B. Hennig and ");
  printf("U. Hinz, 2010 ");
  printf("\n    ");

  return 0;
}
// -- Disclaimer Screen Routine ----------- END --


// -- Disclaimer Command Line Hint --------- BEGIN --
//
//
ULONG PrintingDisclaimerHint()
{
  printf("\n    Please use command line option -d");
  printf(" to switch off ");
  printf("\n    the disclaimer.");
  printf("\n    Command Line Example: '[c:\\]K8055e.exe -n");
  printf(" <devname> -d'.");

  return 0;
}
// -- Disclaimer Command Line Hint ----------- END --


// -- Info Preface Screen Routine -------------------- BEGIN --
//
ULONG PrintingInfoPref()
{
  printf("  ");
  printf("-Info- selected ");
  printf("\n  -------------------------------------------");
  printf("----\n");
  printf("\n      Test -Info- will display basic information");
  printf("\n      concerning the");
  printf("\n      Device Specific Library 'K8055DD.dll'.");
  printf("\n\n       DLL function used in this test:");
  printf("\n       - 'K8055_GetInfoStr()' ");

  return 0;
}
// -- Info Preface Screen Routine ---------------------- END --


// -- Read Preface Screen Routine -------------------- BEGIN --
//
ULONG PrintingReadPref()
{

  printf("  ");
  printf("-Read- selected ");
  printf("\n  ----------------------------------------------");
  printf("-----\n");
  printf("\n      Test -Read- will demonstrate the reading of");
  printf("\n      the Digital Inputs 'Ix' ( I1, I2, I3, I4,");
  printf(" I5 ),");
  printf("\n      the I1-Counter and the I2-Counter,");
  printf("\n      the Analog Inputs 'A1' and 'A2' of K8055.");
  printf("\n\n       DLL functions used in this test:");
  printf("\n       - 'K8055_Open()' ");
  printf("\n       - 'K8055_Init()' ");
  printf("\n       - 'K8055_ReadAllInputs()' ");
  printf("\n       - 'K8055_DecodeDigitalInputs()' ");
  printf("\n       - 'K8055_CheckIxCounter()' ");
  printf("\n       - 'K8055_Close()' ");

  return 0;
}
// -- Read Preface Screen Routine ---------------------- END --


// -- Write Preface Screen Routine ------------------- BEGIN --
//
ULONG PrintingWritePref()
{

  printf("  ");
  printf("-Write- selected ");
  printf("\n  ---------------------------------------------");
  printf("----\n");
  printf("\n    Test -Write- will demonstrate the writing of");
  printf("\n    the byte values 0...255");
  printf("\n    to the Digital Outputs 'Ox' (LEDs LD1...");
  printf("LD8),");
  printf("\n    to the Analog Output 'DAC1' and");
  printf("\n    to the Analog Output 'DAC2' of K8055.");
  printf("\n\n     DLL functions used in this test:");
  printf("\n     - 'K8055_Open()' ");
  printf("\n     - 'K8055_Init()' ");
  printf("\n     - 'K8055_PrepairDigitalOut()' ");
  printf("\n     - 'K8055_PrepairDACxOut()' ");
  printf("\n     - 'K8055_SetAllOutputs()' ");
  printf("\n     - 'K8055_Close()' ");

  return 0;
}
// -- Write Preface Screen Routine --------------------- END --


// ++++++++++++ Routine Collection +++++++++++++++++ END ++++++
