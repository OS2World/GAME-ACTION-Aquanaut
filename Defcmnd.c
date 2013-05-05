/***************************************************************************
*																									*
*  FILE  : DEFCMND.C	  																		*
*																									*
*	DATE  : March 94																			*
*																									*
*	NAME  : Paul Stanley																		*
*																									*
*	This file contains the dialog	procedures for all of the system dialog   *
*  boxes.																						*
*																									*
***************************************************************************/


/*********************
  Included files
*********************/

#include "def.h"
#include "defid.h"
#include "defproto.h"
#include "deftypes.h"
#include "defgbls.h"
#include "defmacro.h"
#include "scancode.h"

#define SLOW_LABEL "Slow"
#define FAST_LABEL "Fast"
#define TINY_LABEL "Tiny"
#define HUGE_LABEL "Huge"
#define NORMAL_LABEL "Normal"
#define HISCORE_TEXT "        HIGH SCORE %d\n \n" \
							"    Name:\t%s\n    Score:\t%06d\n    Date:\t%s" \
							"\n    Nasties:\t%d\n    Hit rate:\t%d%%\n    Wave:\t%d" \
							"\n \n" \
							"    Subs:\t%d\n    S. Bombs:\t%d\n    Bullets:\t%s"
#define CHECK_RECORD "Recording a demo will overwrite the previously " \
                     "recorded game.  Do you wish to continue?"

/* Crude encryption scheme for password checking */
#define ENCODE(c)  (((c - 'A' + 13) % 26) + 'A')

/* Typedef used for unregistration screen */
typedef struct
{
   ULONG ulID;
	BOOL  fClicked;
} CBOXES;

VOID CenterDialog (HWND hwnd);

MRESULT EXPENTRY ProcessKeyDialog (HWND hwnd, 
                                   ULONG msg,
                                   MPARAM mp1, 
                                   MPARAM mp2);
MRESULT EXPENTRY ProcessSpeedDialog (HWND hwnd, 
                                     ULONG msg,
                                     MPARAM mp1, 
                                     MPARAM mp2);
MRESULT EXPENTRY ProcessSizeDialog (HWND hwnd, 
                                    ULONG msg,
                                    MPARAM mp1, 
                                    MPARAM mp2);
MRESULT EXPENTRY ProcessProdInfoDialog (HWND hwnd, 
                                        ULONG msg,
                                        MPARAM mp1, 
                                        MPARAM mp2);
MRESULT EXPENTRY ProcessHiScoreDialog (HWND hwnd, 
                                       ULONG msg,
                                       MPARAM mp1, 
                                       MPARAM mp2);
MRESULT EXPENTRY ProcessRegisterDialog (HWND hwnd, 
                                        ULONG msg,
                                        MPARAM mp1, 
                                        MPARAM mp2);
VOID HideFrameControls (VOID);
VOID ShowFrameControls (VOID);
BOOL CheckPassword (PCHAR szName, PCHAR szID);

/* Handle to the help instance */
HWND hwndHelpInstance;

/**************************************************************************
*
* FUNCTION NAME: HandleCommand
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY HandleCommand (HWND hwnd, 
										  ULONG msg, 
										  MPARAM mp1, 
										  MPARAM mp2)
{
   HWND 					 hwndDlg;
   MCI_GENERIC_PARMS  mciGenericParms;

   switch (SHORT1FROMMP (mp1))
   {
		/*
		** Game menu items...
		*/
		case MENU_START_ID :
		   eDemoMode = DEMO_NONE;
		   if (gAction != PAUSED)
			{
		   	gAction = START;
			}
			break;

		case MENU_STOP_ID :
		   eDemoMode = DEMO_NONE;
		   gAction = STOP;
			break;

		case MENU_EXIT_ID :
		   gAction = EXIT;
			break;

	   case MENU_PAUSE_ID :
		   /* Toggle the pause action */
		   if ((gAction = (gAction == PAUSED) ? NONE : PAUSED) == PAUSED)
			{
				PauseGame ();
			}
			else
			{
				ResumeGame ();
			}
			break;

		case MENU_DEMO_PLAY_ID :
		   eDemoMode = DEMO_PLAY;
		   if (gAction != PAUSED)
			{
		   	gAction = START;
			}
			break;

		case MENU_DEMO_RECORD_ID :
		   /* Display a message box to verify they really want to do this */
			if (!gfDemoLoaded ||
				  WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, CHECK_RECORD, 
				    			     "Demo Record", 0, 
									  MB_YESNO | MB_WARNING | MB_MOVEABLE) == MBID_YES)
			{
		   	eDemoMode = DEMO_RECORD;
		    	if (gAction != PAUSED)
				{
		   		gAction = START;
				}
			}
		   WinSetFocus (HWND_DESKTOP, hwndDefClient);
			break;

		/*
		** Options menu items...
		*/
      case MENU_KEY_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessKeyDialog,
            		             0L,
                  		       KEY_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

      case MENU_SPEED_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessSpeedDialog,
            		             0L,
                  		       SPEED_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

      case MENU_OBJECT_SIZE_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessSizeDialog,
            		             0L,
                  		       SIZE_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

      case MENU_HISCORE_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessHiScoreDialog,
            		             0L,
                  		       HISCORE_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

      case MENU_REGISTER_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessRegisterDialog,
            		             0L,
                  		       REG_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

		case MENU_BULLET_ID :
		   /* Toggle the bullet menu item */
		   gfBullets = gfBullets ? FALSE : TRUE;
         WinCheckMenuItem (hwndMenu, MENU_BULLET_ID, gfBullets);
		   break;

		case MENU_BUBBLES_ID :
		   /* Toggle the bubbles menu item */
		   gfBubbles = gfBubbles ? FALSE : TRUE;
         WinCheckMenuItem (hwndMenu, MENU_BUBBLES_ID, gfBubbles);
		   break;

		case MENU_SOUND_ID :
		   /* Toggle the sound menu item */
         if (gfSoundEnabled)
         {
		      gfSoundOn = gfSoundOn ? FALSE : TRUE;
            WinCheckMenuItem (hwndMenu, MENU_SOUND_ID, gfSoundOn);

				if (gfSoundOn)
				{
               mciGenericParms.hwndCallback = hwnd;
               (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusWaveDeviceID,
                                                    MCI_ACQUIREDEVICE,
                                                    (ULONG) MCI_NOTIFY,
                                                    (PVOID) &mciGenericParms,
                                                    (USHORT) 0);
				}
         }
		   break;

		case MENU_MUSIC_ID :
		   /* Toggle the menu menu item */
		   gfMusicEnabled = gfMusicEnabled ? FALSE : TRUE;
         WinCheckMenuItem (hwndMenu, MENU_MUSIC_ID, gfMusicEnabled); 
         if (gfMusicEnabled)
			{
            (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusMidiDeviceID,
                                                 MCI_ACQUIREDEVICE,
                                                 (ULONG) MCI_NOTIFY,
                                                 (PVOID) &mciGenericParms,
                                                 (USHORT) 0);
			}

         if (gsGameOn)
         {
            /* Play or stop the music */
            gsMusicAction = gfMusicEnabled ? START : STOP;
     	      DosPostEventSem (hSoundEventSem);
         }
		   break;

		case MENU_MOUSE_ID :
		   /* Toggle the mouse menu item */
		   gfMouseEnabled = gfMouseEnabled ? FALSE : TRUE;
         WinCheckMenuItem (hwndMenu, MENU_MOUSE_ID, gfMouseEnabled);
		   EnableMouse (gfMouseEnabled);
			break;

		case MENU_3SHIPS_ID :
			gsStartShips = 3;
         WinCheckMenuItem (hwndMenu, MENU_3SHIPS_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_4SHIPS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_5SHIPS_ID, FALSE);
			break;

		case MENU_4SHIPS_ID :
			gsStartShips = 4;
         WinCheckMenuItem (hwndMenu, MENU_3SHIPS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_4SHIPS_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_5SHIPS_ID, FALSE);
			break;

		case MENU_5SHIPS_ID :
			gsStartShips = 5;
         WinCheckMenuItem (hwndMenu, MENU_3SHIPS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_4SHIPS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_5SHIPS_ID, TRUE);
			break;


		case MENU_3SMARTS_ID :
			gsStartSmarts = 3;
         WinCheckMenuItem (hwndMenu, MENU_3SMARTS_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_5SMARTS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_7SMARTS_ID, FALSE);
			break;

		case MENU_5SMARTS_ID :
			gsStartSmarts = 5;
         WinCheckMenuItem (hwndMenu, MENU_3SMARTS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_5SMARTS_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_7SMARTS_ID, FALSE);
			break;

		case MENU_7SMARTS_ID :
			gsStartSmarts = 7;
         WinCheckMenuItem (hwndMenu, MENU_3SMARTS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_5SMARTS_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_7SMARTS_ID, TRUE);
			break;

		case MENU_SONAR_FADE_ID :
			stScanner.sMode = SCANNER_SWEEP;
         WinCheckMenuItem (hwndMenu, MENU_SONAR_FADE_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_PERSIST_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_NO_ID, FALSE);
			break;

		case MENU_SONAR_PERSIST_ID :
			stScanner.sMode = SCANNER_ON;
         WinCheckMenuItem (hwndMenu, MENU_SONAR_FADE_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_PERSIST_ID, TRUE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_NO_ID, FALSE);
			break;

		case MENU_SONAR_NO_ID :
			stScanner.sMode = SCANNER_OFF;
         WinCheckMenuItem (hwndMenu, MENU_SONAR_FADE_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_PERSIST_ID, FALSE);
         WinCheckMenuItem (hwndMenu, MENU_SONAR_NO_ID, TRUE);
			break;

		case MENU_FRAME_CONTROLS_ID :
			if (gfShowFrame)
			{
				HideFrameControls ();
			}
			else
			{
				ShowFrameControls ();
			}

         gfShowFrame = gfShowFrame ? FALSE : TRUE;
         WinCheckMenuItem (hwndMenu, MENU_FRAME_CONTROLS_ID, gfShowFrame);
			break;

		/*
		** Help menu items...
		*/
		case MENU_HELPINDEX_ID	:
         WinSendMsg (hwndHelpInstance, HM_HELP_INDEX, NULL, NULL);
			break;

		case MENU_HELPEXTENDED_ID :
         WinSendMsg(hwndHelpInstance, HM_EXT_HELP, NULL, NULL);
			break;

		case MENU_HELPHELPFORHELP_ID :
         WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP, NULL, NULL);
			break;

		case MENU_HELPKEYSHELP_ID :
         WinSendMsg (hwndHelpInstance, HM_KEYS_HELP, NULL, NULL);
		   break;

		case MENU_HELPABOUT_ID :
	      hwndDlg = WinLoadDlg (HWND_DESKTOP,
   	                         hwndDefClient,
									  	 (PFNWP) ProcessProdInfoDialog,
            		             0L,
                  		       PROD_INFO_DLG_ID,
                        		 NULL);

	      /* Process the dialog and then destroy it */
   	   WinProcessDlg (hwndDlg);
   		WinDestroyWindow (hwndDlg);
			break;

		default :
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));
   }
}


/**************************************************************************
*
* FUNCTION NAME: ProcessKeyDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessKeyDialog (HWND hwnd, 
                                   ULONG msg,
                                   MPARAM mp1, 
                                   MPARAM mp2)
{
	USHORT         usScanCode;
	static ULONG   ulButton = UP_TEXT_ID;
	static KEYDEFS stLocalDefs;
	static PUSHORT pusKeyDef;

   switch (msg)
   {
      case WM_INITDLG :
		   /* Store the current key settings */
			stLocalDefs = gstKeyDefs;
			pusKeyDef = &(stLocalDefs.usUpKey);

			/* Set the text for each field in the dialog box */
			WinSetDlgItemText (hwnd, UP_TEXT_ID, 
										 	 szScanCode[stLocalDefs.usUpKey]);
			WinSetDlgItemText (hwnd, DOWN_TEXT_ID, 
											 szScanCode[stLocalDefs.usDownKey]);
			WinSetDlgItemText (hwnd, TURN_TEXT_ID,
											 szScanCode[stLocalDefs.usTurnKey]);
			WinSetDlgItemText (hwnd, THRUST_TEXT_ID,
											 szScanCode[stLocalDefs.usThrustKey]);
			WinSetDlgItemText (hwnd, FIRE_TEXT_ID,
											 szScanCode[stLocalDefs.usFireKey]);
			WinSetDlgItemText (hwnd, SMART_TEXT_ID,
											 szScanCode[stLocalDefs.usSmartKey]);
			WinSetDlgItemText (hwnd, HYPER_TEXT_ID,
											 szScanCode[stLocalDefs.usHyperKey]);

		   /* Set the UP button to the clicked state */
         WinPostMsg (WinWindowFromID (hwnd , UP_ID), BM_CLICK, 
			             MPFROMSHORT (TRUE), MPVOID);

			/* Set the appropriate button toggle */
			if (stLocalDefs.fTurnThrust)
			{
         	WinPostMsg (WinWindowFromID (hwnd , TURN_THRUST_ID), BM_CLICK, 
			   	          MPFROMSHORT (TRUE), MPVOID);
				WinSetDlgItemText (hwnd, TURN_ID, "Turn");
				WinSetDlgItemText (hwnd, THRUST_ID, "Thrust");
			}
			else
			{
         	WinPostMsg (WinWindowFromID (hwnd , LEFT_RIGHT_ID), BM_CLICK, 
			   	          MPFROMSHORT (TRUE), MPVOID);
				WinSetDlgItemText (hwnd, TURN_ID, "Left");
				WinSetDlgItemText (hwnd, THRUST_ID, "Right");
			}

		   /* Center the dialog box in the frame window of the parent */
			CenterDialog (hwnd);
         return (0);

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
				   /* Save the key selections in the global list and save them */
					gstKeyDefs = stLocalDefs;
               WinDismissDlg (hwnd, DID_OK);
               return (0);

            case DID_CANCEL:
				   /* Dismiss the dialog without saving the values */
               WinDismissDlg (hwnd, DID_CANCEL);
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_KEYS, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);
         }
			break;

      case WM_CONTROL :
         switch (SHORT1FROMMP (mp1))
			{
				case UP_ID:
					ulButton = UP_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usUpKey);
					break;

				case DOWN_ID:
					ulButton = DOWN_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usDownKey);
					break;

				case TURN_ID:
					ulButton = TURN_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usTurnKey);
					break;

				case THRUST_ID:
					ulButton = THRUST_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usThrustKey);
					break;

				case FIRE_ID:
					ulButton = FIRE_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usFireKey);
					break;

				case SMART_ID:
					ulButton = SMART_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usSmartKey);
					break;

				case HYPER_ID:
					ulButton = HYPER_TEXT_ID;
					pusKeyDef = &(stLocalDefs.usHyperKey);
					break;

				case TURN_THRUST_ID:
					WinSetDlgItemText (hwnd, TURN_ID, "Turn");
					WinSetDlgItemText (hwnd, THRUST_ID, "Thrust");
					stLocalDefs.fTurnThrust = TRUE;
					break;

				case LEFT_RIGHT_ID:
					WinSetDlgItemText (hwnd, TURN_ID, "Left");
					WinSetDlgItemText (hwnd, THRUST_ID, "Right");
					stLocalDefs.fTurnThrust = FALSE;
					break;

				default:
					break;
			}

			/* Take the focus away from the radiobuttons */
			WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, UP_TEXT_ID));
			break;

	   case WM_CHAR :
		   /* Get the scancode and store it in the appropriate place in array */
			if (!(SHORT1FROMMP(mp1) & KC_KEYUP))
			{
		   	usScanCode = CHAR4FROMMP(mp1);
				WinSetDlgItemText (hwnd, ulButton, szScanCode[usScanCode]);
				*pusKeyDef = usScanCode;
			}
			return (0);

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}

/**************************************************************************
*
* FUNCTION NAME: ProcessSpeedDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessSpeedDialog (HWND hwnd, 
                                     ULONG msg,
                                     MPARAM mp1, 
                                     MPARAM mp2)
{
   USHORT i;
	static USHORT usPosition;

   switch (msg)
   {
      case WM_INITDLG :
			/* Set the marks on the slider */
			for (i = 0;  i < 10;  i++)
			{
            WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(i, 2), NULL);
			}

			/* Label the ends of the slider */
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(0, 5), NULL);
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(9, 5), NULL);
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSCALETEXT,
                                  MPFROMSHORT(0), MPFROMP(SLOW_LABEL));
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSCALETEXT,
                                  MPFROMSHORT(9), MPFROMP(FAST_LABEL));

			/* Set the slider position */
			usPosition = (gFramesPerSec / 5) - 1;
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,
											 				  SMA_INCREMENTVALUE),
											 MPFROMSHORT(usPosition));

		   /* Center the dialog box in the frame window of the parent */
			CenterDialog (hwnd);
         return (0);

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
				   /* Calculate the frame rate from the position of the arm */
					gFramesPerSec = (usPosition + 1) * 5;

					/* Reset the timer to tick at this rate */
   				WinStartTimer (hab,
               				   hwndDefClient,
           					      TIMER_ID,
                 					1000/gFramesPerSec);

               WinDismissDlg (hwnd, DID_OK);
               return (0);

            case DID_CANCEL:
				   /* Dismiss the dialog without saving the values */
               WinDismissDlg (hwnd, DID_CANCEL);
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_GAMESPEED, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);

            default:
               break;
         }
			break;

      case WM_CONTROL :
         switch (SHORT1FROMMP(mp1))
			{
				case SLIDER_ID:
				   if (SHORT2FROMMP(mp1) == SLN_CHANGE)
					{
                  usPosition = (USHORT) WinSendDlgItemMsg (hwnd, 
						                        SLIDER_ID,
                                          SLM_QUERYSLIDERINFO,
                                          MPFROM2SHORT (SMA_SLIDERARMPOSITION,
													                 SMA_INCREMENTVALUE),
                                          NULL);
					}
					break;

				default:
					break;
			}
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}


/**************************************************************************
*
* FUNCTION NAME: ProcessSizeDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessSizeDialog (HWND hwnd, 
                                    ULONG msg,
                                    MPARAM mp1, 
                                    MPARAM mp2)
{
   USHORT i;
	BITMAPINFOHEADER bmpData;
	static USHORT usPosition;

   switch (msg)
   {
      case WM_INITDLG :
			/* Set the marks on the slider */
			for (i = 0;  i < 10;  i++)
			{
            WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(i, 2), NULL);
			}

			/* Label the ends and default position of the slider */
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(0, 5), NULL);
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(5, 5), NULL);
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETTICKSIZE,
                                  MPFROM2SHORT(9, 5), NULL);
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSCALETEXT,
                                  MPFROMSHORT(0), MPFROMP(TINY_LABEL));
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSCALETEXT,
                                  MPFROMSHORT(5), MPFROMP(NORMAL_LABEL));
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSCALETEXT,
                                  MPFROMSHORT(9), MPFROMP(HUGE_LABEL));

			/* Set the slider position */
			usPosition = gsObjectSize - 1;
         WinSendDlgItemMsg (hwnd, SLIDER_ID, SLM_SETSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,
											 				  SMA_INCREMENTVALUE),
											 MPFROMSHORT(usPosition));

		   /* Center the dialog box in the frame window of the parent */
			CenterDialog (hwnd);
         return (0);

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
				   /* Scale the bitmaps according to the set size */
					gsObjectSize = usPosition + 1;
			   	for (i = 0;  i < NUM_BMPS;  i++)
					{
						LoadAndStretchBitmap (i);
					}

					/* Redraw the screen */
               WinInvalidateRect (hwndDefClient, NULL, FALSE);

               WinDismissDlg (hwnd, DID_OK);
               return (0);

            case DID_CANCEL:
				   /* Dismiss the dialog without saving the values */
               WinDismissDlg (hwnd, DID_CANCEL);
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_OBJECTSIZE, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);

            default:
               break;
         }
			break;

      case WM_CONTROL :
         switch (SHORT1FROMMP(mp1))
			{
				case SLIDER_ID:
				   if (SHORT2FROMMP(mp1) == SLN_CHANGE)
					{
                  usPosition = (USHORT) WinSendDlgItemMsg (hwnd, 
						                        SLIDER_ID,
                                          SLM_QUERYSLIDERINFO,
                                          MPFROM2SHORT (SMA_SLIDERARMPOSITION,
													                 SMA_INCREMENTVALUE),
                                          NULL);
					}
					break;

				default:
					break;
			}
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}


/**************************************************************************
*
* FUNCTION NAME: ProcessRegisterDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessRegisterDialog (HWND hwnd, 
                                    	 ULONG msg,
                                    	 MPARAM mp1, 
                                    	 MPARAM mp2)
{
	CHAR szID[50];

   switch (msg)
   {
      case WM_INITDLG :
		   /* Center the dialog box in the frame window of the parent */
			CenterDialog (hwnd);
         return (0);

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
				   WinQueryDlgItemText (hwnd, REG_NAME_ID, 
														  REGISTER_NAME_LEN, gszRegName);
				   WinQueryDlgItemText (hwnd, REG_ID_ID, sizeof(szID), szID);
					if (CheckPassword (gszRegName, szID))
					{
						/* The password is cool!  Save the settings now */
						WinDlgBox (HWND_DESKTOP, hwndDefClient, 
										  (PFNWP) ProcessProdInfoDialog,
										  0L, CONGRATS_DLG_ID, NULL);
						gfRegistered = TRUE;
						WriteIniFile ();

						/* Disable the register menu item now we're registered */
   					WinEnableMenuItem (hwndMenu, MENU_REGISTER_ID, !gfRegistered);

               	WinDismissDlg (hwnd, DID_OK);
					}
					else
					{
						WinDlgBox (HWND_DESKTOP, hwndDefClient, 
										  (PFNWP) ProcessProdInfoDialog,
										  0L, SORRY_DLG_ID, NULL);
					}
               return (0);

            case DID_CANCEL:
				   /* Dismiss the dialog without saving the values */
               WinDismissDlg (hwnd, DID_CANCEL);
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_REGISTER, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);

            default:
               break;
         }
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}


/**************************************************************************
*
* FUNCTION NAME: ProcessProdInfoDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessProdInfoDialog (HWND hwnd, 
                                        ULONG msg,
                                        MPARAM mp1, 
                                        MPARAM mp2)
{
   switch (msg)
   {
		case WM_INITDLG:
			CenterDialog (hwnd);
			if (gfRegistered)
			{
				WinSetDlgItemText (hwnd, PROD_REG_NAME_ID, gszRegName);
			}
			else
			{
				WinSetDlgItemText (hwnd, PROD_REG_TO_ID, "Unregistered");
				WinSetDlgItemText (hwnd, PROD_REG_NAME_ID, "Please register the game.");
			}
         return (0);

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
               WinDismissDlg (hwnd, DID_OK);
               return (0);

            default:
               break;
         }
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}


/**************************************************************************
*
* FUNCTION NAME: ProcessUnregisteredDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessUnregisteredDialog (HWND hwnd, 
                                            ULONG msg,
                                            MPARAM mp1, 
                                            MPARAM mp2)
{
   /* Array to track the click boxes */
   static CBOXES aCBoxes[] = {{UNREG_CB0, FALSE},
										{UNREG_CB1, FALSE},
										{UNREG_CB2, FALSE},
										{UNREG_CB3, FALSE},
										{UNREG_CB4, FALSE},
										{UNREG_CB5, FALSE},
										{UNREG_CB6, FALSE},
										{UNREG_CB7, FALSE},
										{UNREG_CB8, FALSE},
										{UNREG_CB9, FALSE}};
	#define NUM_CBOXES (sizeof(aCBoxes)/sizeof(CBOXES))

	static USHORT usClicked = 0;
	INT           i;

   switch (msg)
   {
		case WM_INITDLG:
			CenterDialog (hwnd);
         return (0);

      case WM_CONTROL :
         switch (SHORT1FROMMP (mp1))
         {
			   case UNREG_CB0:
			   case UNREG_CB1:
			   case UNREG_CB2:
			   case UNREG_CB3:
			   case UNREG_CB4:
			   case UNREG_CB5:
			   case UNREG_CB6:
			   case UNREG_CB7:
			   case UNREG_CB8:
			   case UNREG_CB9:
					/* Find the entry in the array for this cbox */
				   for (i = 0;  i < NUM_CBOXES; i++)
					{
						if (aCBoxes[i].ulID == SHORT1FROMMP (mp1))
						{
						   break;
						}
					}

					if (!aCBoxes[i].fClicked)
					{
						aCBoxes[i].fClicked = TRUE;
						usClicked++;
					}
					else
					{
						aCBoxes[i].fClicked = FALSE;
						usClicked--;
					}
				   break;

				default:
               return (0);
         }
			break;

      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case UNREG_OK:
				   if (usClicked == NUM_CBOXES)
					{
               	WinDismissDlg (hwnd, DID_OK);
					}
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_REGISTER, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);

            default:
               return (0);
         }
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}

/**************************************************************************
*
* FUNCTION NAME: ProcessHiScoreDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
MRESULT EXPENTRY ProcessHiScoreDialog (HWND hwnd, 
                                       ULONG msg,
                                       MPARAM mp1, 
                                       MPARAM mp2)
{
   HWND  hwndPage;
   ULONG ulPageId;
   ULONG ipt = 0;
	INT   i;
   CHAR  szMleBuffer[512];

   switch (msg)
   {
		case WM_INITDLG:
			for (i = 0;  i < NUM_HISCORES;  i++)
			{
   			/*
    			* Insert a page of the notebook
    			*/
   			ulPageId = (LONG) WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
        									  BKM_INSERTPAGE, NULL,
        									  MPFROM2SHORT((BKA_STATUSTEXTON | 
											  					 BKA_AUTOPAGESIZE | BKA_MAJOR),
        									  BKA_LAST));

				sprintf (szMleBuffer, "Page %d of %d", i + 1, NUM_HISCORES);
   			WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
        							    BKM_SETSTATUSLINETEXT, MPFROMLONG(ulPageId),
        								 MPFROMP(szMleBuffer));
				

				sprintf (szMleBuffer, "%d", i + 1);
   			WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
        								 BKM_SETTABTEXT, MPFROMLONG(ulPageId),
        								 MPFROMP(szMleBuffer));
   			WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
        								 BKM_SETDIMENSIONS, MPFROM2SHORT(40,25),
        								 MPFROMSHORT(BKA_MAJORTAB));
				
   			hwndPage = WinCreateWindow (hwnd,
													 WC_MLE,
      											 NULL,
      											 WS_VISIBLE | MLS_READONLY,
         	                            0,
         	                            0,
         	                            0,
         	                            0,
         	                            NULLHANDLE,
         	                            HWND_TOP,
         	                            0,
         	                            NULL,
         	                            NULL);

   			WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
         							 BKM_SETPAGEWINDOWHWND, MPFROMLONG(ulPageId),
         							 MPFROMHWND(hwndPage));
				
   			WinSendMsg (hwndPage, MLM_SETIMPORTEXPORT,
         					MPFROMP(szMleBuffer),
         					MPFROMSHORT(sizeof(szMleBuffer)));
				
     			sprintf (szMleBuffer, 
				         HISCORE_TEXT,
							i + 1,
							stHiscore[i].szName,
							stHiscore[i].lScore,
							stHiscore[i].szDate,
							stHiscore[i].sAliens,
							stHiscore[i].sHitRate,
							stHiscore[i].sWave + 1,
							stHiscore[i].sShips,
							stHiscore[i].sSmarts,
							(stHiscore[i].fBullets) ? "ON" : "OFF");
				
   			WinSendMsg (hwndPage, MLM_IMPORT, &ipt,
         				   MPFROMSHORT(strlen(szMleBuffer)));
				
   			WinSendDlgItemMsg (hwnd, NOTEBOOK_ID,
         							 BKM_SETPAGEWINDOWHWND, MPFROMLONG(ulPageId),
         							 MPFROMHWND(hwndPage));
			}

			CenterDialog (hwnd);
			return (0);
				
      case WM_COMMAND :
         switch (SHORT1FROMMP (mp1))
         {
            case DID_OK:
				   /* Dismiss the dialog */
               WinDismissDlg (hwnd, DID_OK);
               return (0);

			   case DID_HELP:
				   /* Display the keys help panel */
    				WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP,
                            MPFROM2SHORT(PANEL_HIGHSCORE, NULL), 
									 MPFROMSHORT(HM_RESOURCEID));
               return (0);
         }
			break;

      default:
		   break;
   }

   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
}

/**************************************************************************
*
* FUNCTION NAME: CenterDialog
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID CenterDialog (HWND hwnd)
{
   SHORT ix, iy;
   SWP   swp, swpDlg;
 
   /* Query width and height of dialog box */
   WinQueryWindowPos (hwnd, &swpDlg);
 
	/* Query position of frame window */
	WinQueryWindowPos (hwndDefFrame, &swp);

   /* Center dialog box within the Screen */
   ix = (SHORT)(swp.x + (glWindowX - swpDlg.cx ) / 2);
   iy = (SHORT)(swp.y + (glWindowY - swpDlg.cy ) / 2);
   WinSetWindowPos (hwnd, HWND_TOP, ix, iy, 0, 0, SWP_MOVE);
}

/**************************************************************************
*
* FUNCTION NAME: InitHelp
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID InitHelp (VOID)
{
    HELPINIT hini;

    /* inititalize help init structure */
    hini.cb = sizeof (HELPINIT);
    hini.ulReturnCode = 0L;
    hini.pszTutorialName = (PSZ) NULL;
    hini.phtHelpTable = (PHELPTABLE) MAKELONG (DEF_HELP_TABLE, 0xFFFF);
    hini.hmodHelpTableModule = (HMODULE) 0;
    hini.hmodAccelActionBarModule = (HMODULE) 0;
    hini.idAccelTable = 0;
    hini.idActionBar = 0;
    hini.pszHelpWindowTitle = "Aquanaut! Help";
    hini.fShowPanelId = CMIC_HIDE_PANEL_ID;
    hini.pszHelpLibraryName = "aquanaut.hlp";

    /* creating help instance */
    hwndHelpInstance = WinCreateHelpInstance (hab, &hini);

    /* associate help instance with main frame */
    WinAssociateHelpInstance (hwndHelpInstance, hwndDefFrame);
}

/**************************************************************************
*
* FUNCTION NAME: HideFrameControls
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID HideFrameControls (VOID)
{
    RECTL rclTitlebar, rclMenu;

	 /* Make the parent of the frames HWND_OBJECT to effectively remove them */
    WinSetParent (hwndTitlebar, HWND_OBJECT, FALSE);
    WinSetParent (hwndSysmenu, HWND_OBJECT, FALSE);
    WinSetParent (hwndMinmax, HWND_OBJECT, FALSE);
    WinSetParent (hwndMenu, HWND_OBJECT, FALSE);
    WinSendMsg (hwndDefFrame, WM_UPDATEFRAME,
                MPFROMLONG(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU),
                MPVOID);

    /* Reduce the height of the frame once menu and titlebar are gone */
	 WinQueryWindowRect (hwndMenu, &rclMenu);
	 WinQueryWindowRect (hwndTitlebar, &rclTitlebar);
	 gsFrameHeight -= (SHORT)((rclTitlebar.yTop - rclMenu.yBottom) +
	 							  	(rclMenu.yTop - rclMenu.yBottom));
    WinSetWindowPos (hwndDefFrame, HWND_TOP, 0, 0,
                     gsFrameWidth,
                     gsFrameHeight,
                     SWP_SIZE);
}

/*********************************************************************
*
* FUNCTION NAME: ShowFrameControls
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
*********************************************************************/
VOID ShowFrameControls (VOID)
{
    RECTL rclTitlebar, rclMenu;

    /* Increase the height of the frame now and titlebar are back */
	 WinQueryWindowRect (hwndMenu, &rclMenu);
	 WinQueryWindowRect (hwndTitlebar, &rclTitlebar);
	 gsFrameHeight += (SHORT)((rclTitlebar.yTop - rclMenu.yBottom) +
	 							  	(rclMenu.yTop - rclMenu.yBottom));
    WinSetWindowPos (hwndDefFrame, HWND_TOP, 0, 0,
                     gsFrameWidth,
                     gsFrameHeight,
                     SWP_SIZE);

	 /* Make the parent of the windows the frame to reenable them */
    WinSetParent (hwndTitlebar, hwndDefFrame, FALSE);
    WinSetParent (hwndSysmenu, hwndDefFrame, FALSE);
    WinSetParent (hwndMinmax, hwndDefFrame, FALSE);
    WinSetParent (hwndMenu, hwndDefFrame, FALSE);

    WinSendMsg (hwndDefFrame, WM_UPDATEFRAME,
                MPFROMLONG(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU),
                MPVOID);
    WinInvalidateRect (hwndDefFrame, NULL, TRUE);
}

/**************************************************************************
*
* FUNCTION NAME: CheckPassword
*
* DESCRIPTION:
*		Encrypts the name and compares the resulting ID against the supplied one
*
* INPUT PARAMETERS:
*     PCHAR szName and szID
*
* OUTPUT PARAMETERS:
*     TRUE if password is good.
*
**************************************************************************/
BOOL CheckPassword (PCHAR szRegName, PCHAR szID)
{
	INT  i, j, iLen;
	CHAR c;
	CHAR szName[REGISTER_NAME_LEN];

	/*
	** Get the name into an upper case string - removing spaces and only
	** using every other character 
	*/
	for (i = 0, j = 0;  i < strlen (szRegName);  i += 2)
	{
	   if (szRegName[i] != '\"' && szRegName[i] != ' ' && szRegName[i] != '\t')
		{
			szName[j++] = toupper (szRegName[i]);
		}
	}
	szName[j] = '\0';

	/* Append AQUA to the name */
	strcat (szName, "AQUA");
	iLen = strlen (szName);

	/* Now switch the characters, encoding as we go along */
	for (i = 0;  i < iLen - 1;  i += 2)
	{
		c = szName[i];
		szName[i] = ENCODE (szName[i+1]);
		szName[i+1] = ENCODE (c);
	}

	/* If there are an odd number of characters then encode the last char */
	if (iLen & 1)
	{
		szName[iLen - 1] = ENCODE (szName[iLen - 1]);
	}

	/* Now make the supplied ID uppercase */
	for (i = 0;  i < strlen (szID);  i++)
	{
		szID[i] = toupper (szID[i]);
	}

	/* If the two are now the same the password is OK! */
	return ((strcmp (szID, szName) == 0) ? TRUE : FALSE);
}
