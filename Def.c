/***************************************************************************
*																									*
*  FILE  : DEF.C																				*
*																									*
*	DATE  : March 94																			*
*																									*
*	NAME  : Paul Stanley																		*
*																									*
*	This file contains the main routines for the def game.  It includes 		*
*  pretty much everything apart from the Move* functions which are in		*
*  defmove.c.																					*
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

#define  WAVE_OVER  100
VOID GetMouseMoves (BOOL fClear);

/* Array used for storing the current extent of the logo in attract mode */
POINTL ptlLogo[4];

/* Filename for INI file */
CHAR szIniFile[80];

/* Define the array to store the demo data */
#define MAX_DEMO_FRAMES 10000
CHAR acDemo[MAX_DEMO_FRAMES];
ULONG ulDemoFrame;

/**************************************************************************
*
* FUNCTION NAME: Main
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
VOID main (INT argc, CHAR *argv[])
{
	ULONG  ulThreadID;
	POINTL ptl;
	PCHAR  pc;
	HMQ    hmq;


   /* Set the initial action */
	gAction = NONE;
	gsGameOn = FALSE;
	eDemoMode = DEMO_NONE;

	/* Get the current path and append the name of the INI file */
	strcpy (gszPath, argv[0]);
	if ((pc = strrchr (gszPath, '\\')) != NULL)
	{
		*(pc + 1) = '\0';
	}
	sprintf (szIniFile, "%s%s", gszPath, INI_FILE);

	/* Read in the INI file if one exists */
	ReadIniFile ();

	gfDemoLoaded = LoadDemoData (acDemo, sizeof(acDemo));

	/* Create an event semaphore for the PM thread */
	if (DosCreateEventSem (NULL, &hEventSem, 0, FALSE) != 0)
	{
   	exit (0);
	}

	/* Create an event semaphore for painting */
	if (DosCreateEventSem (NULL, &hPaintEventSem, 0, FALSE) != 0)
	{
   	exit (0);
	}

	/* Create a thread to handle the screen updates and msg processing */
   if (DosCreateThread (&ulThreadID,
                        (PFNTHREAD) ScreenThread,
                        0L,
                        (ULONG) 0,
                        (ULONG) (STACK_SIZE)) != 0)
   {
      /* Failed to start thread */
      exit (0);
   }

	/* Wait for thread to complete its initialization */
	DosWaitEventSem (hEventSem, SEM_INDEFINITE_WAIT);
	DosResetEventSem (hEventSem, &gulCount);

	/* Clear the paint sem */
   DosResetEventSem (hPaintEventSem, &gulCount);

   /* Create an event semaphore to sync the sound thread */
	if (DosCreateEventSem (NULL, &hSoundEventSem, 0, FALSE) != 0)
	{
   	exit (0);
	}

	/* See if we can enable sounds by looking for MMPM/2 */
	if (gfSoundEnabled = EnableSound ())
	{
 		/* Create a thread to handle the sound - it sets up playlists */
   	if (DosCreateThread (&ulThreadID,
      	                  (PFNTHREAD) HandleSounds,
         	               0L,
            	            (ULONG) 0,
               	         (ULONG) (STACK_SIZE)) != 0)
	   {
   	   /* Failed to start thread */
      	exit (0);
   	}

		/* Wait for thread to complete its initialization */
		DosWaitEventSem (hSoundEventSem, SEM_INDEFINITE_WAIT);
		DosResetEventSem (hSoundEventSem, &gulCount);
	}

  	/* 
   ** Create a message queue for this thread - just want it briefly 
   ** because sound won't work otherwise (don't know why!).  The queue
   ** is needed for the following Win calls and the WinDlgBox
   */
   hmq = WinCreateMsgQueue (hab, 1000);

   /* Check the sound/music items accordingly */
   WinCheckMenuItem (hwndMenu, MENU_SOUND_ID, gfSoundOn);
   WinCheckMenuItem (hwndMenu, MENU_MUSIC_ID, gfMusicEnabled);
   WinEnableMenuItem (hwndMenu, MENU_SOUND_ID, gfSoundEnabled);
   WinEnableMenuItem (hwndMenu, MENU_MUSIC_ID, gfSoundEnabled);
   WinEnableMenuItem (hwndMenu, MENU_DEMO_PLAY_ID, gfDemoLoaded);

   /* Display the unregistered box if we're not yet registered */
	if (!gfRegistered)
	{
		WinDlgBox (HWND_DESKTOP, hwndDefClient, 
										  (PFNWP) ProcessUnregisteredDialog,
										  0L, UNREG_DLG_ID, NULL);
	}
   WinDestroyMsgQueue (hmq);

	/* Start the clock! The timer controls the pace of the game */
   WinStartTimer (hab,
                  hwndDefClient,
                  TIMER_ID,
                  1000/gFramesPerSec);

	while (gAction != EXIT)
	{
	   /* Reset any action which may have occurred (eg START, STOP) */
	   gAction = NONE;

      /* If called with a -D (or /D) parameter then go into demo mode */
      if (argc == 2 && (argv[1][1] == 'd' || argv[1][1] == 'D'))
      {
         eDemoMode = DEMO_PLAY;
      }

	   /* Display the attract screens */
	   AttractMode ();

		if (gAction != EXIT)
		{
	   	/* Initialize game variables */
      	Initialize ();

			/* Game is being played */
			gsGameOn = TRUE;

	   	/* Clear the screen */
			PAINT (REFRESH_SCREEN);

			/* Enable the mouse */
			EnableMouse (TRUE);

      	/* Play the music */
      	if (gfMusicEnabled)
      	{
         	gsMusicAction = START;
     	   	DosPostEventSem (hSoundEventSem);
      	}
		}

		/* Main loop */
		while (gAction != EXIT && gAction != STOP && gsLives)
		{
			/* Nothing to be drawn or plotted so far */
		   usDrawItems = 0;
			usMarkerItems = 0;
			usBoltItems = 0;

			if (gAction != PAUSED)
			{
				/* Effectively scroll the screen based upon ship's movement */
		      if ((gsWorld = (gsWorld + gsWorldShift) % PLANET_SIZE) < 0)
				{
					gsWorld += PLANET_SIZE;
				}
				
				/* Calculate the radar's left hand edge */
				if ((gsRadar = gsWorld - RADAR_START) < 0)
				{
					gsRadar += PLANET_SIZE;
				}
				
				/* Move the land and the stars */
				MoveStars (FALSE, FALSE);
				MoveLand (FALSE);
				MoveScanner ();
				MoveExplosions ();
				
				/* Move all the sceen objects */
				MoveLanders ();
				MoveHumans ();
				MoveMutants ();
				MovePods ();
				MoveSwarmers ();
				MoveSkimmers ();
				MoveBombers ();
				MoveBombs ();
				MoveBullets ();

				/* Process those features only present in the registered game */
				if (gfRegistered)
				{
					MoveMermaid ();
					MoveRings ();
				}
				
				/* Look for any mouse movement */
				if (gfMouseEnabled)
				{
					GetMouseMoves (FALSE);
				}

				/*
				** If we're in demo play mode then use the saved keystrokes to
				** control the ship
				*/
				if (eDemoMode == DEMO_PLAY)
				{
					UseDemoKeys (acDemo, &ulDemoFrame);
				}
				else if (eDemoMode == DEMO_RECORD)
				{
					/*
					** Prevent the key strokes being changed until after the
					** MoveShip call has processed the key structure.  This flag
					** causes WM_CHAR's to be ignored.
					*/
					gfDemoKeyProtect = TRUE;
					SaveDemoKeys (acDemo, &ulDemoFrame);
				}

				/* 
				** These must be done after aliens have moved since draw list used 
				*/
				FireBullet ();
				MoveShip ();
				MoveLasers ();
				
				/* Handle any event in the game (new lander drop etc.) */
				ProcessEvent ();

				/* Now draw the screen */
		   	DosResetEventSem (hPaintEventSem, &gulCount);
			   DrawScreen (hps);
	     	   DosPostEventSem (hPaintEventSem);
			}
				
			/* Wait for the next loop - the semaphore is posted by a timer msg */
		   DosWaitEventSem (hEventSem, SEM_INDEFINITE_WAIT);
		   DosResetEventSem (hEventSem, &gulCount);
		}
		
		/* Game over */
		gsGameOn = FALSE;

      /* Stop the music */
      if (gfMusicEnabled)
      {
         gsMusicAction = STOP;
     	   DosPostEventSem (hSoundEventSem);
      }

		if (eDemoMode == DEMO_RECORD)
		{
      	StoreDemoData (acDemo, ulDemoFrame);
			gfDemoLoaded = TRUE;
		   hmq = WinCreateMsgQueue (hab, 1000);
		   WinEnableMenuItem (hwndMenu, MENU_DEMO_PLAY_ID, gfDemoLoaded);
         WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                        "The demo has been successfully recorded.",
     			            "Demo Record", 0, 
						      MB_OK | MB_MOVEABLE);
		   WinSetFocus (HWND_DESKTOP, hwndDefClient);
  			WinDestroyMsgQueue (hmq);
		}

		EnableMouse (FALSE);

		if (gAction != EXIT && gAction != STOP)
		{
	      CheckScore ();
		}
	}

	/* Write the INI file */
	WriteIniFile ();
}


/**************************************************************************
*
* FUNCTION NAME: ScreenThread
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
VOID ScreenThread (VOID)
{
   HAB   hab;          /* Anchor block handle for this thread.*/
   HMQ   hmq;          /* Message queue handle */
   QMSG  qmsg;         /* Queue message structure. */
   CHAR  acDefClass[] = "Def Window";
	CHAR  acLogoClass[] = "LogoClass";
   ULONG ulDefFlags    = FCF_SIZEBORDER | FCF_MINBUTTON |
                         FCF_SYSMENU | FCF_TITLEBAR | FCF_TASKLIST |
								 FCF_MENU | FCF_ACCELTABLE;

   RECTL  rcl;

   /* Set up stuff for this thread */
   hab = WinInitialize (0);

   hmq = WinCreateMsgQueue (hab, 1000);

   /* Register the main window procedure  */
   WinRegisterClass (hab,
                     acDefClass,
                     (PFNWP) DefWindowProc,
                     CS_SIZEREDRAW | CS_SYNCPAINT | CS_MOVENOTIFY,
                     0);

	/* Register a window procedure to draw the bitmap logo in dialogs */
   WinRegisterClass (hab,
                     acLogoClass,
                     (PFNWP) LogoWindowProc,
                     CS_SYNCPAINT | CS_MOVENOTIFY,
                     0);

   /* Create the standard window */
   hwndDefFrame = WinCreateStdWindow (HWND_DESKTOP,
                                      WS_VISIBLE,
                                      &ulDefFlags,
                                      acDefClass,
                                      NULL,
                                      0L,
                                      0L,
                                      MENU_ID,
                                      (PHWND) &hwndDefClient);

	/* Save the frame handles */
   hwndTitlebar = WinWindowFromID (hwndDefFrame, FID_TITLEBAR);
   hwndSysmenu = WinWindowFromID (hwndDefFrame, FID_SYSMENU);
   hwndMinmax = WinWindowFromID (hwndDefFrame, FID_MINMAX);
   hwndMenu = WinWindowFromID (hwndDefFrame, FID_MENU);

	/* Load the icon */
   WinSendMsg (hwndDefFrame,
               WM_SETICON,
               (MPARAM) WinLoadPointer (HWND_DESKTOP, 0L, 1),
               (MPARAM) FALSE);

	/* Subclass the frame window to intercept sizing messages */
	pfnOldFrameProc = WinSubclassWindow (hwndDefFrame, (PFNWP)SizeWindowProc);

	/* Check the menu items accordingly */
   WinCheckMenuItem (hwndMenu, MENU_BULLET_ID, gfBullets);
   WinCheckMenuItem (hwndMenu, MENU_3SHIPS_ID, (gsStartShips == 3));
   WinCheckMenuItem (hwndMenu, MENU_4SHIPS_ID, (gsStartShips == 4));
   WinCheckMenuItem (hwndMenu, MENU_5SHIPS_ID, (gsStartShips == 5));
   WinCheckMenuItem (hwndMenu, MENU_3SMARTS_ID, (gsStartSmarts == 3));
   WinCheckMenuItem (hwndMenu, MENU_5SMARTS_ID, (gsStartSmarts == 5));
   WinCheckMenuItem (hwndMenu, MENU_7SMARTS_ID, (gsStartSmarts == 7));
   WinCheckMenuItem (hwndMenu, MENU_FRAME_CONTROLS_ID, gfShowFrame);
   WinCheckMenuItem (hwndMenu, MENU_MOUSE_ID, gfMouseEnabled);
   WinCheckMenuItem (hwndMenu, MENU_SONAR_FADE_ID, 
										(stScanner.sMode == SCANNER_SWEEP));
   WinCheckMenuItem (hwndMenu, MENU_SONAR_PERSIST_ID, 
										(stScanner.sMode == SCANNER_ON));
   WinCheckMenuItem (hwndMenu, MENU_SONAR_NO_ID, 
										(stScanner.sMode == SCANNER_OFF));
   WinCheckMenuItem (hwndMenu, MENU_BUBBLES_ID, gfBubbles);

	/* Disable the register menu item if we're already registered */
   WinEnableMenuItem (hwndMenu, MENU_REGISTER_ID, !gfRegistered);

	/* Initialize the help instance */
	InitHelp ();

   WinSetFocus (HWND_DESKTOP, hwndDefClient);

	/* Post the event semaphore to tell the main thread to get going */
	DosPostEventSem (hEventSem);

   /* Handle the message queue - dispatch messages as they arrive */
   while (WinGetMsg (hab, &qmsg, 0, 0, 0))
   {
      WinDispatchMsg (hab, &qmsg);
   }

   /* Clean up - destroy windows, queue and terminate */
   WinDestroyWindow (hwndDefFrame);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);

	/* Set the event flag and set the exit flag so the other thread exits */
	DosPostEventSem (hEventSem);
	gAction = EXIT;
}

/**************************************************************************
*
* FUNCTION NAME: Initialize
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
* PSEUDOCODE:
*
**************************************************************************/
VOID Initialize (VOID)
{
	INT i;
   LONG seed;
	CHAR cSeed;

	/* Set the world position to be the origin */
   gsWorld = 0;
   gsWorldShift = 0;
   gsRadar = 0;

	/* Get the time for a random seed */
  	time ((time_t *) &seed);

   /* Set the seed for the random generator */
	if (eDemoMode == DEMO_NONE)
	{
   	srand (seed);
	}
	else if (eDemoMode == DEMO_RECORD)
	{
		ulDemoFrame = 0;
		SaveSeed (acDemo, &ulDemoFrame, seed);
	}
	else
	{
		/* srand with the stored random number */
		ulDemoFrame = 0;
		UseSeed (acDemo, &ulDemoFrame);
	}

   /* Create the landscape */
   gusLandPoints = CreateLandscape ();

	/* Fill in the altitude array which is used by landers and humans */
   CalculateAltitudes ();

	/* initialize the land draw data */
	memset (&stLandDraw, 0, sizeof(stLandDraw));
   gsLandIndex = HEADER_POINTS;
   stLandDraw.pDrawData = stLandDraw.array1;
   stLandDraw.pEraseData = stLandDraw.array2;
	stLandDraw.lDrawPts = 0;
	stLandDraw.lErasePts = 0;
	stLandDraw.fActive = TRUE;

	/* Call MoveLand to set up the draw arrays ready for drawing later on */
	MoveLand (TRUE);

	/* Make all the humans inactive */
	for (i = 0;  i < NUM_HUMANS;  i++)
	{
		memset (&stHuman[i], 0, sizeof (stHuman[i]));
	}

	/* Make all the stars inactive */
	for (i = 0;  i < NUM_STARS;  i++)
	{
		memset (&stStar[i], 0, sizeof (stStar[i]));
	}

	/* Make all the rings inactive */
	for (i = 0;  i < NUM_RINGS;  i++)
	{
		memset (&stRing[i], 0, sizeof (stRing[i]));
	}

	/* Make all the pods inactive */
	for (i = 0;  i < NUM_PODS;  i++)
	{
		memset (&stPod[i], 0, sizeof (stPod[i]));
	}

	/* Make all the bombers inactive */
	for (i = 0;  i < NUM_BOMBERS;  i++)
	{
		memset (&stBomber[i], 0, sizeof (stBomber[i]));
	}

	/* Make all the lasers inactive */
	for (i = 0;  i < NUM_LASERS;  i++)
	{
		memset (&stLaser[i], 0, sizeof (stLaser[i]));
	}

	/* Make all the explosions inactive */
	for (i = 0;  i < NUM_EXPLOSIONS;  i++)
	{
		memset (&stExplosion[i], 0, sizeof (stExplosion[i]));
	}

	/* Make all the landers inactive */
	for (i = 0;  i < NUM_LANDERS;  i++)
	{
		memset (&stLander[i], 0, sizeof (stLander[i]));
	}

	/* Make all the mutants inactive */
	for (i = 0;  i < NUM_MUTANTS;  i++)
	{
		memset (&stMutant[i], 0, sizeof (stMutant[i]));
	}

	/* Make all the swarmers inactive */
	for (i = 0;  i < NUM_SWARMERS;  i++)
	{
		memset (&stSwarmer[i], 0, sizeof (stSwarmer[i]));
	}

	/* Make all the skimmers inactive */
	for (i = 0;  i < NUM_SKIMMERS;  i++)
	{
		memset (&stSkimmer[i], 0, sizeof (stSkimmer[i]));
	}

	/* Make all the bombs inactive */
	for (i = 0;  i < NUM_BOMBS;  i++)
	{
		memset (&stBomb[i], 0, sizeof (stBomb[i]));
	}

	/* Make all the bullets inactive */
	for (i = 0;  i < NUM_BULLETS;  i++)
	{
		memset (&stBullet[i], 0, sizeof (stBullet[i]));
	}

	/* There is no text to display */
	for (i = 0;  i < NUM_TEXTS;  i++)
	{
		memset (&stText[i], 0, sizeof (stText[i]));
	}

	InitHumans (TRUE);

	/* Set up the mermaid if we're registered */
	if (gfRegistered)
	{
		InitMermaid ();
	}

	/* Reset the bullet counts */
	gusNumBullets = 0;
	gusBulletTimer = 0;

	/* Set up the game tallies */
	glScore = 0;
	gsSmarts = gsStartSmarts;
	gsLives = gsStartShips;
	gsAliens = 0;
	gsShots = 0;

	/* Set the wave number to 0 */
	gsWave = 0;

	/* Set up the ship parameters */
	InitShip ();

   /* Set up the star positions */
   InitStars (FALSE);

   /* Set up the scanner */
   InitScanner ();

	/* Tell this routine that the stars havent been drawn yet */
	MoveStars (FALSE, TRUE);

	/* Set up wave */
	SetupWave (gsWave);
}
   

/**************************************************************************
*
* FUNCTION NAME: SetupWave
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
VOID SetupWave (USHORT usWave)
{
   INT i;
	CHAR szString[20];
	LONG seed;

	/* There are no baddies at the start of the game */
   gusBadGuys = 0;

	/* Copy the event data into an array that we can then modify on the fly */
	for (i = 0;  i < NUM_EVENTS;  i++)
	{
		stEvent[i] = WaveData[usWave].stEvent[i];
	}

	/* Set the wave timer and event index to zero */
   gulWaveTimer = 0;
   gusNextEvent = 0;

	/* Set up one or more pods */
	InitPods (WaveData[usWave].usNumPods);

	/* Set up one or more rings */
   if (gfRegistered)
   {
	   InitRings (WaveData[usWave].usNumRings);
   }

	/* Set up one or more bombers */
	InitBombers (WaveData[usWave].usNumBombers);

	/* 
	** Go back to a full complement of humans if this is wave 4, 7, ...
	** (ie usWave == 3, 6, 9...) 
	*/
	if (usWave % 3 == 0)
	{
		InitHumans (FALSE);
		stLandDraw.fActive = TRUE;

		/* This is a bit of a fiddle to ensure the land gets redrawn */
		if (gsWorldShift == 0)
		{
	   	gsWorldShift = stShip.fFacingRight ? 1 : -1;
		}
		MoveLand (TRUE);
	}

	/* Make sure no one shoots for a little while */
	gusBulletTimer = BULLET_DELAY;

	/* Set up the wave text which is displayed for a while */
	sprintf (szString, "Wave %d", (gsWave + 1));
	DisplayText (szString, 0, (PLAY_HEIGHT/2), CLR_YELLOW, TRUE, FALSE);

	/* Get the time for a random seed */
  	time ((time_t *) &seed);

   /* Set the seed for the random generator */
	if (eDemoMode == DEMO_NONE)
	{
   	srand (seed);
	}
	else if (eDemoMode == DEMO_RECORD)
	{
		SaveSeed (acDemo, &ulDemoFrame, seed);
	}
	else
	{
		/* srand with the stored random number */
		UseSeed (acDemo, &ulDemoFrame);
	}

}


/**************************************************************************
*
* FUNCTION NAME: RestartAfterShipHit
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
VOID RestartAfterShipHit (VOID)
{
	INT i;

	/* Restart the ship */
	InitShip ();

	/* For all landers alive, start them at the top of the screen again */
	for (i = 0;  i < NUM_LANDERS;  i++)
	{
		if (stLander[i].fActive)
		{
			stLander[i].eState = APPEARING;
			stLander[i].sAppearCnt = APPEAR_SPEED * APPEAR_CYCLES;
			stLander[i].y = PLAY_HEIGHT - stBitmap[LANDERL].cy;
			stLander[i].sBitmap = (stLander[i].dx > 0) ? LANDERR : LANDERL;
		}
	}

	/* Distance the mutants from where the ship is to appear */
	for (i = 0;  i < NUM_MUTANTS;  i++)
	{
		if (stMutant[i].fActive && stMutant[i].fDraw)
		{
			stMutant[i].x = gsWorld + SCREEN_WIDTH + 
													(rand()	% (PLANET_SIZE / 2));
		}
	}

	/* If any humans were in the process of being captured, settle them */
	for (i = 0;  i < NUM_HUMANS;  i++)
	{
		if (stHuman[i].eState != LANDED)
		{
			stHuman[i].eState = LANDED;
			stHuman[i].y = height[stHuman[i].x] + HUMAN_YOFFSET;
			stHuman[i].fDraw = FALSE;
			stHuman[i].fTargetted = FALSE;
		}
	}

	/* Remove all depth charges */
	for (i = 0;  i < NUM_BOMBS;  i++)
	{
		if (stBomb[i].fActive)
		{
			/* This will cause the bomb to expire on the next move */
			stBomb[i].sDuration = 1;
		}
	}

	/* Make sure no one shoots as soon as the ship reappears */
	gusBulletTimer = BULLET_DELAY;

	/* Decrement the number of lives */
	gsLives--;

	/* Reset the mouse move counts so ship is stationary at start */
	if (gfMouseEnabled)
	{
		GetMouseMoves (TRUE);
	}

	/* Redraw the screen */
	PAINT (REFRESH_SCREEN);
}

/**************************************************************************
*
* FUNCTION NAME: InitHumans
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
VOID InitHumans (BOOL fGameStart)
{
	INT  i, j;
	BOOL fTooClose;

	/*
	** If this is not the start of game, then some humans may already exist -
	** move them down in the array
	*/
   for (i = 0, j = 0;  !fGameStart && j < NUM_HUMANS;  j++)
	{
		if (stHuman[j].fActive)
		{
			stHuman[i++] = stHuman[j];
		}
	}

	for (;  i < NUM_HUMANS;  i++)
	{
		/* Ensure that the humans are not on top of each other */
		fTooClose = TRUE;
		while (fTooClose)
		{
         /* Find a peak to put the treasure on */
			j = rand() % gusLandPoints;
			if (land[j].y < land[j+1].y)
			{
				j++;
			}

			/* Make sure the x coordinate is positive */
         if ((stHuman[i].x = land[j].x - stBitmap[HUMAN].cx / 2) < 0)
			{
				stHuman[i].x += PLANET_SIZE;
			}

			for (j = 0;  j < i;  j++)
			{
				if (abs (stHuman[i].x - stHuman[j].x) < MIN_SEPARATION)
				{
					break;
				}
			}

			/* No nearby human found */
			if (i == j)
			{
				fTooClose = FALSE;
			}
		}

		stHuman[i].y = height[stHuman[i].x] + HUMAN_YOFFSET;
		stHuman[i].rx = NO_OLD_RADAR;
		stHuman[i].fDraw = FALSE;
		stHuman[i].fTargetted = FALSE;
		stHuman[i].fActive = TRUE;
		stHuman[i].eState = LANDED;
	}

	usNumHumans = NUM_HUMANS;
}

/**************************************************************************
*
* FUNCTION NAME: InitShip
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
VOID InitShip (VOID)
{
	/* Set up the start position and data for the ship */
	memset (&stShip, 0, sizeof(stShip));
	stShip.x = SHIP_MIN_X;
	stShip.y = PLAY_HEIGHT / 2;
	stShip.rx = NO_OLD_RADAR;
	stShip.dx = stShip.sFlame = 0;
	stShip.fActive = TRUE;
	stShip.fFacingRight = TRUE;
	stShip.sCarrying = NO_CARRY;
}

/**************************************************************************
*
* FUNCTION NAME: InitStars
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
VOID InitStars (BOOL fTitle)
{
   INT i;

   /* Set up the start position and data for the bubbles */
   for (i = 0;  i < NUM_STARS;  i++)
   {
		memset (&stStar[i], 0, sizeof (stStar[i]));
		if (fTitle)
		{
			/* For the title screen - bubbles should just be on the edges */
      	stStar[i].x = rand() % (SCREEN_WIDTH / 4);
			if (rand() & 1)
			{
				stStar[i].x += (SCREEN_WIDTH * 3) / 4;
			}
		}
		else
		{
      	stStar[i].x = rand() % (SCREEN_WIDTH - stBitmap[BUBBLE].cx);
		}
	   stStar[i].y = (rand() % (PLAY_HEIGHT - LOWEST_LAND 
												- stBitmap[BUBBLE].cy)) + LOWEST_LAND;
      stStar[i].dy = (rand() % MAX_STAR_DY) + 1;
      stStar[i].dx = (rand() % MAX_STAR_DX) + 1;
		stStar[i].changeDx = (rand() % STAR_DX_CHANGE) + 1;
   }
}

/**************************************************************************
*
* FUNCTION NAME: InitScanner
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
VOID InitScanner (VOID)
{
   stScanner.rx = STATUS_WIDTH + 1;
   stScanner.ry1 = PLAY_HEIGHT + 3;
   stScanner.ry2 = PLAY_HEIGHT + STATUS_HEIGHT - 3;
}

/**************************************************************************
*
* FUNCTION NAME: AddLanders
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
VOID AddLanders (SHORT sNum)
{
	INT i, j, k;
	INT iOffset;

	/* Look for free spaces in the lander array and use these */
	for (i = 0;  i < NUM_LANDERS && sNum;  i++)
	{
		/* If this entry is not used then use it */
		if (!stLander[i].fActive)
		{
			/* Now set up the lander variables */
			stLander[i].y = PLAY_HEIGHT - stBitmap[LANDERL].cy;
			stLander[i].x = rand() % PLANET_SIZE;
			stLander[i].rx = NO_OLD_RADAR;
			stLander[i].eState = APPEARING;
			stLander[i].fDraw = FALSE;
			stLander[i].fActive = TRUE;
			stLander[i].sAppearCnt = APPEAR_SPEED * APPEAR_CYCLES;

			/* See if the lander is on screen */
			if (stLander[i].fDraw = ON_SCREEN(stLander[i].x))
			{
			   stLander[i].screenx = SCREENX(stLander[i].x);
			}
			
			/* Generate a random dx speed and direction */
			stLander[i].dx = (rand() % MAX_LANDER_DX) + 1;
			if (rand() & 1)
			{
				stLander[i].dx = -stLander[i].dx;
			}

			/* Use a left or right facing bitmap */
			stLander[i].sBitmap = (stLander[i].dx > 0) ? LANDERR : LANDERL;
			
			/* Set a human target for lander i */
      	SetLanderTarget (i);
			
			/* Increment the number of bad guys */
      	gusBadGuys++;

			/* Decrement the allocated count */
			sNum--;
		}
	}		
}

/**************************************************************************
*
* FUNCTION NAME: AddMutant
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     SHORT  x
*     SHORT  y
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID AddMutant (SHORT x, SHORT y, BOOL fNew)
{
	INT i;

	/* Look for a free space in the mutant array and use this */
	for (i = 0;  i < NUM_MUTANTS;  i++)
	{
		if (!stMutant[i].fActive)
		{
			/* Now set up the mutant variables */
			stMutant[i].y = y;
			stMutant[i].x = x;
			stMutant[i].rx = NO_OLD_RADAR;
			stMutant[i].fActive = TRUE;
			stMutant[i].sChangeDir = 1;  /* Causes direction to be set on move */

			if (fNew)
			{
			   /*
			   ** If adding a mutant when there is no existing lander (it's a 
				** lander drop event and we're in space) then set the guy up to
				** appear.
				*/
				stMutant[i].eState = APPEARING;
				stMutant[i].sAppearCnt = APPEAR_SPEED * APPEAR_CYCLES;

				/* See if the mutant is on screen */
				if (stMutant[i].fDraw = ON_SCREEN(stMutant[i].x))
				{
			   	stMutant[i].screenx = SCREENX(stMutant[i].x);
				}
			}
			else
			{
				stMutant[i].eState = MUTATE;
			   stMutant[i].fDraw = FALSE;
			}

			/* Increment the number of bad guys */
      	gusBadGuys++;
		   break;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: AddSkimmer
*
* DESCRIPTION:
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID AddSkimmer (VOID)
{
	INT i;

	/* Look for a free space in the skimmer array and use this */
	for (i = 0;  i < NUM_SKIMMERS;  i++)
	{
		if (!stSkimmer[i].fActive)
		{
			/* Now set up the skimmer to be a long way from the ship */
			stSkimmer[i].y = stShip.y;
			if ((stSkimmer[i].x = stShip.x - (PLANET_SIZE / 2)) < 0)
			{
				stSkimmer[i].x += PLANET_SIZE;
			}
			stSkimmer[i].rx = NO_OLD_RADAR;
			stSkimmer[i].fDraw = FALSE;
			stSkimmer[i].fActive = TRUE;
			stSkimmer[i].sHoverTimer = SKIMMER_HOVER_TIME;
			stSkimmer[i].sHits = SKIMMER_HITS;
			gusBadGuys++;
		   break;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: InitPods
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID InitPods (SHORT iNumPods)
{
	INT i;

	for (i = 0;  i < iNumPods;  i++)
	{
		/* Set up the pod variables */
		do
		{
			stPod[i].x = rand() % PLANET_SIZE;
			stPod[i].y = rand() % (PLAY_HEIGHT - stBitmap[JELLY1].cy);
		} while (CheckProximity (stPod[i].x, stPod[i].y));
		stPod[i].dx = (rand() & 1) ? MAX_POD_DX : -MAX_POD_DX;
		stPod[i].dy = (rand() % MAX_POD_DY) + 1;
		if (rand() & 1)
		{
			stPod[i].dy = -stPod[i].dy;
		}
		stPod[i].rx = NO_OLD_RADAR;
		stPod[i].fDraw = FALSE;
		stPod[i].fActive = TRUE;
		stPod[i].sChangeDir = 1;  /* Causes direction to be set on move */
		stPod[i].sFrame = 0;
		stPod[i].sChangeFrame = 0;

		/* Increment the number of bad guys */
      gusBadGuys++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: InitRings
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID InitRings (SHORT iNumRings)
{
	INT i;

	for (i = 0;  i < iNumRings;  i++)
	{
      /* Y value is low to ensure the burst path stays on screen */
	   stRing[i].x = rand() % PLANET_SIZE;
	   stRing[i].y = PLAY_HEIGHT / 4;
      stRing[i].fShot = FALSE;
		stRing[i].rx = NO_OLD_RADAR;
		stRing[i].fDraw = FALSE;
		stRing[i].fActive = TRUE;

		/* Increment the number of bad guys */
      gusBadGuys++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: InitMermaid
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID InitMermaid (VOID)
{
	/* Set up the mermaid variables */
	memset (&stMermaid, 0, sizeof(stMermaid));
	stMermaid.x = rand() % PLANET_SIZE;
	stMermaid.y = rand() % (PLAY_HEIGHT - stBitmap[MERMAID3].cy);
	stMermaid.dx = (rand() & 1) ? MAX_MERMAID_DX : -MAX_MERMAID_DX;
	stMermaid.dy = (rand() % MAX_MERMAID_DY) + 1;
	stMermaid.rx = NO_OLD_RADAR;
	stMermaid.fDraw = FALSE;
	stMermaid.sChangeDir = 1;  /* Causes direction to be set on move */
	stMermaid.sFrame = 0;
	stMermaid.sChangeFrame = 0;

	/* Set the time for appearance and indicate that not yet appeared */
	stMermaid.sAppearance = (rand() % MERMAID_APPEAR_RANGE)
																	+ MERMAID_APPEAR_MIN;
	stMermaid.fActive = FALSE;
}

/**************************************************************************
*
* FUNCTION NAME: InitBombers
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID InitBombers (SHORT iNumBombers)
{
	INT i;

	for (i = 0;  i < iNumBombers;  i++)
	{
		/* Set up the bomber variables */
		do
		{
		   /* Bomber is always in the top quarter of the screen */
			stBomber[i].x = rand() % PLANET_SIZE;
			stBomber[i].y = (rand() % (PLAY_HEIGHT / 4)) + 
									((PLAY_HEIGHT * 3) / 4) - stBitmap[BOMBERL].cy;
		} while (CheckProximity (stBomber[i].x, stBomber[i].y));
		stBomber[i].dx = (rand() & 1) ? BOMBER_DX : -BOMBER_DX;
		stBomber[i].sBitmap = (stBomber[i].dx > 0) ? BOMBERR : BOMBERL;
		stBomber[i].rx = NO_OLD_RADAR;
		stBomber[i].fDraw = FALSE;
		stBomber[i].fActive = TRUE;
		stBomber[i].sBombTimer = (rand() % MAX_BOMB_SPACING) + 
														WaveData[gsWave].usBombSpacing;
		stBomber[i].sHits = WaveData[gsWave].usBomberHits;

		/* Increment the number of bad guys */
      gusBadGuys++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: AddSwarmers
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID AddSwarmers (SHORT iNumSwarmers, SHORT x, SHORT y, SHORT dx)
{
	INT i;

	/* Look for a free space in the swarmer array and use this */
	for (i = 0;  iNumSwarmers && i < NUM_SWARMERS;  i++)
	{
		if (!stSwarmer[i].fActive)
		{
			/* Set up the swarmer variables */
			stSwarmer[i].x = x;
			stSwarmer[i].y = y;
			stSwarmer[i].dx = dx;
			stSwarmer[i].dy = (rand() % MAX_SWARMER_DY) + 1;
			if (rand() % 1)
			{
				stSwarmer[i].dy = -stSwarmer[i].dy;
			}
			stSwarmer[i].dy2 = (rand() & 1) ? 1 : -1;
			stSwarmer[i].rx = NO_OLD_RADAR;
			stSwarmer[i].fDraw = FALSE;
			stSwarmer[i].fActive = TRUE;

			/* Set a random delay before the swarmer moves */
			stSwarmer[i].sAccelChange = rand() % MAX_SWARMER_DELAY;

			/* Increment the number of bad guys */
      	gusBadGuys++;
			iNumSwarmers--;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: PlantBomb
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID PlantBomb (SHORT x, SHORT y)
{
	INT i;

	/* Look for a free space in the bomb array and use this */
	for (i = 0;  i < NUM_BOMBS;  i++)
	{
		if (!stBomb[i].fActive)
		{
			/* Set up the bomb variables */
			stBomb[i].x = x;
			stBomb[i].y = y;
			stBomb[i].sDuration = BOMB_DURATION;
			stBomb[i].fDraw = FALSE;
			stBomb[i].fActive = TRUE;
			break;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: FireSmart
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID FireSmart (VOID)
{
   INT i;

	/* Look thru the draw list to find anything to kill */
	for (i = 0;  i < usDrawItems;  i++)
	{
		if (!stDrawItem[i].fErase && stDrawItem[i].fDraw)
		{
			switch (stDrawItem[i].bitmap)
			{
				case LANDERL:
				case LANDERR:
				case LANDERC:
				case MUTANTL:
				case MUTANTR:
				case SWARMER:
				case SKIMMERL:
				case SKIMMERR:
				case JELLY1:
				case JELLY2:
				case JELLY3:
				case BOMBERL:
				case BOMBERR:
					HandleHit (i, FALSE);
					break;

				default:
					break;
			}
		}
	}

   MakeSound (SOUND_FIRE_SMART);

	/* Decrease the number of smarts left and redisplay the status panel */
	gsSmarts--;
	PAINT (DRAW_ICONS);
}

/**************************************************************************
*
* FUNCTION NAME: FireBullet
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID FireBullet (VOID)
{
	INT i, j, k, iOffset;
	SHORT sDy, sDx;
	SHORT sRand;

	/* If the timer has counted down, fire a bullet */
	if (gfBullets && gusBulletTimer == 0)
	{
		/* Look for a free space in the bullet array and use this */
		for (i = 0;  usDrawItems && stShip.fActive &&
						 gusNumBullets < NUM_BULLETS && i < NUM_BULLETS;  i++)
		{
			if (!stBullet[i].fActive)
			{
			   /* Start at a random point in the draw array */
				iOffset = rand() % usDrawItems;

				/* Find an alien in the draw list to fire the bullet */
				for (j = 0;  j < usDrawItems;  j++)
				{
		         k = (j + iOffset) % usDrawItems;
					if (!stDrawItem[k].fErase && stDrawItem[k].fDraw)
					{
				    	switch (stDrawItem[k].bitmap)
					 	{
						   /* If one of these, then fire the bullet at the ship */
					 	 	case LANDERL:
					 	 	case LANDERR:
					 	 	case LANDERC:
						 	case MUTANTL:
						 	case MUTANTR:
						 	case SWARMER:
						 	case RING:
								sRand = (SHORT) WaveData[gsWave].usBulletRand;

							   /* Set the start position */
								stBullet[i].x = stDrawItem[k].x + 
												    (stBitmap[stDrawItem[k].bitmap].cx/2);
								stBullet[i].y = stDrawItem[k].y + 
												    (stBitmap[stDrawItem[k].bitmap].cy/2);

								/* If this is too close to the ship - forget it */
								if (abs (stBullet[i].x - stShip.x) < 
														WaveData[gsWave].usBulletMinX)
								{
									continue;
								}

								/* 
								** Calculate the distance between ship and alien
								** ensuring no 'divide by zero' traps can occur
								*/
						      if ((sDx = stShip.x - stBullet[i].x + 
									  			(rand() % sRand) - (sRand / 2)) == 0)
								{
									sDx = 1;
								}
						      if ((sDy = stShip.y - stBullet[i].y +
									  			(rand() % sRand) - (sRand / 2)) == 0)
								{
									sDy = 1;
								}

								/* Base the speed upon the smallest of the above */
								if (abs(sDx) > abs(sDy))
								{
									stBullet[i].dx = (sDx > 0) ? BULLET_DINC 
																		: -BULLET_DINC;
								   stBullet[i].dy = (sDy * BULLET_DINC) / abs(sDx);
								}
								else
								{
									stBullet[i].dy = (sDy > 0) ? BULLET_DINC 
																		: -BULLET_DINC;
								   stBullet[i].dx = (sDx * BULLET_DINC) / abs(sDy);
								}

								/* Convert the bullet X position to world positions */
								stBullet[i].x = (stBullet[i].x + gsWorld) % PLANET_SIZE;

								/* Set the other variables */
								stBullet[i].fDraw = FALSE;
								stBullet[i].fActive = TRUE;
								gusBulletTimer = WaveData[gsWave].usBulletDelay;
								gusNumBullets++;
								return;

							default:
								;
						}
					}
				}
				break;
			}
		}
		/* 
		** If we could not find a suitable alien or all bullets were used, then
		** wait a while before trying again to reduce wasted time spent looking 
		** for a suitable alien when none is on screen
		*/
		gusBulletTimer = BULLET_CHECK;
	}
	else if (gfBullets)
	{
		/* Decrement the count */
		gusBulletTimer--;
	}
}
																							
/**************************************************************************
*
* FUNCTION NAME: SetLanderTarget
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     SHORT  i       Index into lander array
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID SetLanderTarget (SHORT i)
{
   INT j, k;
	INT iOffset;

	/* Select a human to attack and also set speed for pickup */
	for (j = 0;  j < NUM_HUMANS;  j++)
	{
		if (stHuman[j].fActive && !stHuman[j].fTargetted
									  &&  stHuman[j].eState == LANDED)
		{
			/* This human is not targetted for attack - he'll do! */
			if ((stLander[i].targetX = stHuman[j].x + 
						(stBitmap[HUMAN].cx / 2) - (stBitmap[LANDERL].cx / 2)) < 0)
			{
				stLander[i].targetX += PLANET_SIZE;
			}
			stLander[i].targetIdx = j;
			stHuman[j].fTargetted = TRUE;
			break;
		}
	}

	/*
	** If all humans were already targetted then find the first active
	** one starting from a random point in the human list
	*/
	if (j == NUM_HUMANS)
	{
		iOffset = rand() % NUM_HUMANS;
		for (j = 0;  j < NUM_HUMANS;  j++)
		{
		   k = (j + iOffset) % NUM_HUMANS;
			if (stHuman[k].fActive && stHuman[k].eState == LANDED)
			{
				/* This human is targetted already but he'll be shared */
				stLander[i].targetX = stHuman[k].x;
				stLander[i].targetIdx = k;
				break;
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: CheckProximity
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
BOOL CheckProximity (SHORT x, SHORT y)
{
	SHORT  sXLeft, sXRight;
	BOOL   fRet = FALSE;

	if ((y > stShip.y - PROXIMITY_LIMIT) && (y < stShip.y + PROXIMITY_LIMIT))
	{
      if ((sXLeft = stShip.x + gsWorld - PROXIMITY_LIMIT) < 0)
		{
			sXLeft += PLANET_SIZE;
		}
		else
		{
			sXLeft %= PLANET_SIZE;
		}

		sXRight = (sXLeft + 2 * PROXIMITY_LIMIT) % PLANET_SIZE;

		fRet = (sXRight > sXLeft) ? (x > sXLeft && x < sXRight)
										  : (x > sXLeft || x < sXRight);
	}

	return (fRet);
}

/*************************************************************************
*
*  FUNCTION NAME   : DefWindowProc
*
*  DESCRIPTION     : This window procedure handles the game window
*
*  INPUT PARAMETERS:
*           HWND   ==>  handle to the window receiving the message
*           ULONG  ==>  command message
*           MPARAM ==>  message specific data
*           MPARAM ==>  message specific data
*
*  RETURNS :
*           MRESULT ==> standard window proc return code
*
*  PSEUDOCODE:
*
**************************************************************************/
MRESULT EXPENTRY DefWindowProc (HWND   hwnd,
                                ULONG  msg,
                                MPARAM mp1,
                                MPARAM mp2)
{
	static RECTL       rclOrigView;         /* Initial viewport */
   static HDC         hdcPS;               /* Device context for PS */
   HPS                hpsPaint;
   RECTL     	       rcl;
	POINTL             ptl[4];
	SWP					 swp;
	static INT         iWidth, iHeight;
   SIZEL              sizl;
   LONG               i;
   LONG               lScale;
	LONG               lColor;
	USHORT			    usFlags;
	USHORT			    usScanCode;
	BOOL				    fKeyDown, fKeyHit;
	FONTMETRICS 		 fm;
   MCI_GENERIC_PARMS  mciGenericParms;
	CHAR  				 szText[50];
	SHORT 				 sXinc;


   switch (msg)
      {
      case WM_CREATE:
		   /* Create a micro PS and get the original viewport */
		   hdcPS = WinOpenWindowDC (hwnd);
			sizl.cx = SCREEN_WIDTH;
			sizl.cy = SCREEN_HEIGHT;
			hps = GpiCreatePS (hab, hdcPS, &sizl, 
										PU_ARBITRARY | GPIA_ASSOC | GPIT_MICRO);
			GpiQueryPageViewport (hps, &rclOrigView);

         /* Create memory contexts for bitmaps - they're loaded in WM_SIZE */
         for (i = 0;  i < NUM_BMPS;  i++)
         {
            hdc[i] = DevOpenDC (hab, OD_MEMORY, "*", 0L, 0L, 0L);
            sizl.cx = 0;
            sizl.cy = 0;
            hpsMemory[i] = GpiCreatePS (hab, hdc[i], &sizl,
                                        PU_PELS | GPIF_DEFAULT |
                                        GPIT_MICRO | GPIA_ASSOC);
         }

   		/* Query width and height of screen device */
   		iWidth = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
   		iHeight = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);

			/* Use the stored client window size if such a thing exists */
			if (glWindowX == -1)
			{
				/* Calculate the window size required */
				glWindowX = (iWidth * SCREEN_WIDTH) / VGA_WIDTH;

				/* Calculate a corresponding Y size */
				glWindowY = (SCREEN_HEIGHT * glWindowX) / SCREEN_WIDTH;
			}

			/* Get the frame handle */
			hwndDefFrame = WinQueryWindow (hwnd, QW_PARENT);

   		/* Get the frame boundary from our client window size */
   		rcl.yBottom = 0;
   		rcl.xLeft   = 0;
   		rcl.yTop    = glWindowY;
   		rcl.xRight  = glWindowX;
   		WinCalcFrameRect (hwndDefFrame, &rcl, FALSE);
			gsFrameWidth = (SHORT) (rcl.xRight - rcl.xLeft);
			gsFrameHeight = (SHORT) (rcl.yTop - rcl.yBottom);

			if (gsPosX == -1)
			{
				/* No previously defined position so just set size */
   			WinSetWindowPos (hwndDefFrame, HWND_TOP, 0, 0,
   		   	              gsFrameWidth,
   		      	           gsFrameHeight,
   		         	        SWP_SIZE);

			   /* Get the current position */
				WinQueryWindowRect (hwndDefFrame, &rcl);
				gsPosX = rcl.xLeft;
				gsPosY = rcl.yBottom;
			}
			else
			{
				/* Use stored position and set the size */
   			WinSetWindowPos (hwndDefFrame, HWND_TOP, gsPosX, gsPosY,
   		   	              gsFrameWidth,
   		      	           gsFrameHeight,
   		         	        SWP_SIZE | SWP_MOVE);
			}

			/* Where's the center of our window */
			gsCenterX = gsPosX + gsFrameWidth / 2;
			gsCenterY = gsPosY + gsFrameHeight / 2;

			/* Clear the window */
			WinQueryWindowRect (hwnd, &rcl);
         WinFillRect (hps, &rcl, CLR_BLUE);

			/* Find out the size of the font and store the parameters */
   		GpiQueryFontMetrics (hps, sizeof(fm), &fm);
   		gsCharcx = (SHORT) fm.lMaxCharInc + 2;
		   gsCharcx = (gsCharcx > BASE_CHARCX) ? gsCharcx : SCALEX(gsCharcx);
   		gsCharcy = (SHORT) (fm.lEmHeight * 5) / 4;
		   gsCharcy = (gsCharcy > BASE_CHARCY) ? gsCharcy : SCALEY(gsCharcy);

			/* Set the window title */
			WinSetWindowText (hwndDefFrame, "Aquanaut!");
 
			/* Set up text attributes for overpainting */
   		GpiSetBackColor (hps, CLR_BLUE);
   		GpiSetBackMix (hps, BM_OVERPAINT);

			break;

		case WM_MOVE:
         if (gsGameOn)
			{
				DosWaitEventSem (hPaintEventSem, SEM_INDEFINITE_WAIT);
			}

		   /* Save the new window position */
			WinQueryWindowPos (hwndDefFrame, &swp);
			gsPosX = swp.x;
			gsPosY = swp.y;
			gsCenterX = gsPosX + gsFrameWidth / 2;
			gsCenterY = gsPosY + gsFrameHeight / 2;
			break;

      case WM_SIZE:
         if (gsGameOn)
			{
				DosWaitEventSem (hPaintEventSem, SEM_INDEFINITE_WAIT);
			}

         /* Find the new client window size */
         glWindowX = (ULONG) SHORT1FROMMP (mp2);
         glWindowY = (ULONG) SHORT2FROMMP (mp2);

         /* Stretch the bitmaps to fit */
         for (i = 0;  i < NUM_BMPS;  i++)
         {
            LoadAndStretchBitmap (i);
         }

			/* Calculate the new frame size */
   		rcl.yBottom = 0;
   		rcl.xLeft   = 0;
   		rcl.yTop    = glWindowY;
   		rcl.xRight  = glWindowX;
   		WinCalcFrameRect (hwndDefFrame, &rcl, FALSE);
			gsFrameWidth = (SHORT) (rcl.xRight - rcl.xLeft);
			gsFrameHeight = (SHORT) (rcl.yTop - rcl.yBottom);
				
			lScale = min ((65536L * glWindowX) / rclOrigView.xRight,
							  (65536L * glWindowY) / rclOrigView.yTop);
			rcl.xLeft = 0;
			rcl.yBottom = 0;
			rcl.xRight = (rclOrigView.xRight * lScale) / 65536L;
			rcl.yTop = (rclOrigView.yTop * lScale) / 65536L;
			GpiSetPageViewport (hps, &rcl);

			/* Move the window if we're now offscreen */
			if (gsFrameWidth + gsPosX > iWidth)
			{
   			WinSetWindowPos (hwndDefFrame, HWND_TOP, 
									  iWidth - gsFrameWidth, gsPosY,
   		   	              0, 0, SWP_MOVE);
			}
			else if (gsFrameHeight + gsPosY > iHeight)
			{
   			WinSetWindowPos (hwndDefFrame, HWND_TOP, 
									  gsPosX, iHeight - gsFrameHeight,
   		   	              0, 0, SWP_MOVE);
			}

			gsCenterX = gsPosX + gsFrameWidth / 2;
			gsCenterY = gsPosY + gsFrameHeight / 2;

         /* Resize the character offsets */
   		GpiQueryFontMetrics (hps, sizeof(fm), &fm);
   		gsCharcx = (SHORT) fm.lMaxCharInc + 2;
		   gsCharcx = (gsCharcx > BASE_CHARCX) ? gsCharcx : SCALEX(gsCharcx);
   		gsCharcy = (SHORT) (fm.lEmHeight * 5) / 4;
		   gsCharcy = (gsCharcy > BASE_CHARCY) ? gsCharcy : SCALEY(gsCharcy);
         break;

      case WM_CLOSE:
         /* Close the audio device */
         if (gfSoundEnabled)
         {
            (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusWaveDeviceID,
                                                 MCI_CLOSE,
                                                 MCI_WAIT,
                                                 (ULONG) NULL,
                                                 (ULONG) NULL);
            (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusMidiDeviceID,
                                                 MCI_CLOSE,
                                                 MCI_WAIT,
                                                 (ULONG) NULL,
                                                 (ULONG) NULL);
         }
			gAction = EXIT;
			break;

		case WM_TIMER:
			/* Tell main thread to get on with its calculations. */
			DosPostEventSem (hEventSem);
		   break;

		case WM_COMMAND:
			/* Handle the command messages */
			HandleCommand (hwnd, msg, mp1, mp2);
			break;

      case WM_PAINT:
		   /* Suspend the main thread to stop painting activities */
			/* Compare against TRUE bacuase can have value WAVE_OVER */
         if (gsGameOn == TRUE)
			{
				DosWaitEventSem (hPaintEventSem, SEM_INDEFINITE_WAIT);
			}

         /* 
			** Get our PS  - we dont actually use this one but need to make the
			** call anyway
			*/
         hpsPaint = WinBeginPaint (hwnd, NULLHANDLE, &rcl);

			/* If we moved or sized the screen then repaint */
         if (gsGameOn)
         {
            RefreshScreen (hps);
         }
         else
         {
			   WinFillRect (hps, &rcl, CLR_BLUE);
         }

			WinEndPaint (hpsPaint);
			break;

		case WM_USER:
			switch ((SHORT) mp1)
			{
				case DRAW_LOGO:
					/* Draw the attract logo at the current place */
      			GpiBitBlt (hps, 
								  hpsMemory[LOGO], 
								  4L, 
								  ptlLogo, 
						  	  	  ROP_SRCCOPY,
						     	  BBO_AND);

					/* Draw all bubbles and erase blocks */
					for (i = 0;  i < usDrawItems;  i++)
					{
						if (stDrawItem[i].fDraw)
						{
   				   	ptl[0].x = SCALEX (stDrawItem[i].x);
   				   	ptl[0].y = SCALEY (stDrawItem[i].y);
   				   	ptl[1].x = ptl[0].x + stBitmap[stDrawItem[i].bitmap].sx;
   				   	ptl[1].y = ptl[0].y + stBitmap[stDrawItem[i].bitmap].sy;
   				   	ptl[2].x = 0;
   				   	ptl[2].y = 0;
   				   	GpiBitBlt (hps,
										  hpsMemory[stDrawItem[i].fErase ? 
   				                                   ERASE : stDrawItem[i].bitmap],
							           3L,
										  ptl,
										  ROP_SRCCOPY,
										  BBO_AND);
						}
					}

		 			PrintAt (hps, 0, SCREEN_HEIGHT/12, CLR_WHITE, 
										  "(C) Paul Stanley 1994", TRUE, FALSE);
					break;
			  
				case DRAW_GETNAME:
	   			PrintAt (hps, 0, (PLAY_HEIGHT/2) + gsCharcy * 2, CLR_GREEN, 
		            		"That score is one of the 10 highest scores!", 
								TRUE, FALSE);
					PrintAt (hps, 0, (PLAY_HEIGHT/2), CLR_CYAN, 
					 			"Enter your name and then press ENTER.", TRUE, FALSE);
					PrintAt (hps, 0, (PLAY_HEIGHT/2) - gsCharcy * 2,
													LASERCOLOR, gszName, TRUE, TRUE);
					break;
			   case DRAW_WAVEOVERNONELEFT:
					PrintAt (hps, 0, (SCREEN_HEIGHT / 2) + stBitmap[HUMAN].cy,
	         				gsPaintInfo2 ? CLR_BLUE : CLR_YELLOW, 
								"Wave complete.  All treasure has been captured", 
								TRUE, TRUE);
					break;

			   case DRAW_WAVEOVER:
					sXinc = stBitmap[HUMAN].cx * 5 / 4;
					PrintAt (hps, 0, (SCREEN_HEIGHT / 2) + stBitmap[HUMAN].cy,
	         				gsPaintInfo2 ? CLR_BLUE : CLR_YELLOW, 
								"Wave complete.  Treasure saved:", 
								TRUE, TRUE);

					/* Display a bitmap for each human so far displayed */
					for (i = 0;  i <= gsPaintInfo1;  i++)
					{
   					DrawBitmap (hps, 
										SCALEX (((SCREEN_WIDTH / 2) - (sXinc * 5) + 
																					(i * sXinc))), 
										SCALEY (((SCREEN_HEIGHT - stBitmap[HUMAN].cy) /
																								 2)), 
										gsPaintInfo2 ? ERASE : HUMAN, 
										FALSE);
						sprintf (szText, 
						         "Bonus points: %04d", SAVED_HUMAN_SCORE * (i + 1));
	   				PrintAt (hps, 0, 
									(SCREEN_HEIGHT / 2) - stBitmap[HUMAN].cy - gsCharcy, 
				   				gsPaintInfo2 ? CLR_BLUE : CLR_WHITE, 
									szText, TRUE, TRUE);
					}
					break;

				case DRAW_GAMEOVER:
	   			PrintAt (hps, 0, (PLAY_HEIGHT / 2) , 
								gsPaintInfo1 ? CLR_BLUE : LASERCOLOR, 
								GAME_OVER_STRING, TRUE, TRUE);
					break;

				case DRAW_PAUSED:
 					PrintAt (hps, 0, (PLAY_HEIGHT / 2), 
								gsPaintInfo1 ? CLR_BLUE : CLR_YELLOW, 
								PAUSED_STRING, TRUE, TRUE);
					break;

				case DRAW_HISCORES:
					PrintAt (hps, 
								0,	(SCREEN_HEIGHT / 2) + (5 * gsCharcy), CLR_WHITE,
		   		         "-= Aquanaut! Hall of Fame =-", TRUE, FALSE);
					for (i = 0;  i < NUM_HISCORES;  i++)
					{
					   lColor = LASERCOLOR;
		   			sprintf (szText, "%06d", 
												stHiscore[i].lScore, stHiscore[i].szName);
						PrintAt (hps, 0, 
						         (SCREEN_HEIGHT / 2) + 
												(NUM_HISCORES / 2 - 2 - i) * gsCharcy,
						         lColor, ". . . . . . . .", TRUE, FALSE);
						PrintAt (hps, (SCREEN_WIDTH / 2) - (5 * gsCharcx), 
						         (SCREEN_HEIGHT / 2) + 
												(NUM_HISCORES / 2 - 2 - i) * gsCharcy,
									lColor, szText, FALSE, FALSE);
						PrintAt (hps, (SCREEN_WIDTH / 2) + (2 * gsCharcx),
						         (SCREEN_HEIGHT / 2) + 
												(NUM_HISCORES / 2 - 2 - i) * gsCharcy,
									lColor, stHiscore[i].szName, FALSE, FALSE);
					}
					break;

				case DRAW_SCREEN:
					DrawScreen (hps);
					break;

				case REFRESH_SCREEN:
					RefreshScreen (hps);
					break;

				case DRAW_SCORE:
					DisplayNum (hps, glScore, gsScoreX, gsScoreY);
					break;
					
				case DRAW_ICONS:
					DrawLifeIcons (hps);
					DrawSmartIcons (hps);
					break;

				case CLEAN_SCREEN:
					WinQueryWindowRect (hwnd, &rcl);
					WinFillRect (hps, &rcl, CLR_BLUE);
					break;

				default:
				   break;
			}

			DosPostEventSem (hPaintEventSem);
         break;

		case WM_CHAR:
		   /* If we're in demo record mode we may be in the protected area */
			if (gfDemoKeyProtect)
			{
		   	return ((MRESULT)TRUE);
			}

		   /* What keypresses did we get */
			usFlags = SHORT1FROMMP (mp1);

			/* KeyDown is TRUE if the key is hit or held, KeyHit only when hit */
			fKeyDown = !(usFlags & KC_KEYUP);
			fKeyHit = fKeyDown && (!(usFlags & KC_PREVDOWN));

			/* Save the key info for the high score entry screen */
			gstKey.usChar = SHORT1FROMMP (mp2);
			gstKey.usVirtKey = SHORT2FROMMP (mp2);
			gstKey.fVirtual = (usFlags & KC_VIRTUALKEY);
			gstKey.fHit = fKeyHit;

			/* Look at the scancode */
			usScanCode = (USHORT) CHAR4FROMMP(mp1);
			if (usScanCode == gstKeyDefs.usUpKey)
			{
			   stShip.keyUp = fKeyDown;
			}
			else if (usScanCode == gstKeyDefs.usDownKey)
			{
			   stShip.keyDown = fKeyDown;
			}
			else if (usScanCode == gstKeyDefs.usTurnKey)
			{
            if (gstKeyDefs.fTurnThrust)
            {
			      stShip.keyTurn = fKeyHit;
            }
            else
            {
               /* The turn key means "face left and thrust" */
               stShip.keyThrust = fKeyDown;
               stShip.fFacingRight = FALSE;
            }
			}
			else if (usScanCode == gstKeyDefs.usThrustKey)
			{
            stShip.keyThrust = fKeyDown;
            if (!gstKeyDefs.fTurnThrust)
            {
               /* The turn key means "face right and thrust" */
               stShip.fFacingRight = TRUE;
            }
			}
			else if (usScanCode == gstKeyDefs.usFireKey)
			{
			   stShip.keyFire = fKeyHit;
			}
			else if (usScanCode == gstKeyDefs.usSmartKey)
			{
			   stShip.keySmart = fKeyHit;
			}
			else if (usScanCode == gstKeyDefs.usHyperKey)
			{
			   stShip.keyHyper = fKeyHit;
			}
		   return ((MRESULT)TRUE);

		case WM_BUTTON1DOWN:
			stShip.keyFire = TRUE;
			return ((MRESULT)TRUE);

		case WM_BUTTON2DOWN:
			stShip.keySmart = TRUE;
			return ((MRESULT)TRUE);

      case WM_DESTROY:
         for (i = 0;  i < NUM_BMPS;  i++)
         {
            GpiDestroyPS (hpsMemory[i]);
            DevCloseDC (hdc[i]);
            GpiDeleteBitmap (hbm[i]);
         }
			gAction = EXIT;
			WinReleasePS (hps);
         break;

      case HM_QUERY_KEYS_HELP:
		   /* Return the ID of the keys help panel */
         return ((MRESULT)PANEL_HELPKEYS);

      case MM_MCINOTIFY:
         if (SHORT2FROMMP (mp1) == PLAYLIST_ID)
         {
            /* This means sound has finished so set sound playing to FALSE */
            gsSoundPlaying = -1;
         }
         else if (SHORT1FROMMP (mp1) == MCI_NOTIFY_SUCCESSFUL)
         {
            /* The music has finished start it up again */
            gsMusicAction = START;
     	      DosPostEventSem (hSoundEventSem);
         }
         break;

      case MM_MCIPASSDEVICE:
		   /* Keep track of whether we have the device or not */
         gfPassed = !(SHORT1FROMMP(mp2) == MCI_GAINING_USE);
			break;

      case WM_MINMAXFRAME:
		   /* Pause the game if minimized */
		   if (((PSWP) mp1)->fl & SWP_MINIMIZE)
			{
				PauseGame ();
			}
			break;

		case WM_SINGLESELECT:
		   /* Give this window focus */
   		WinSetFocus (HWND_DESKTOP, hwndDefClient);
			return ((MRESULT) TRUE);

      case WM_ACTIVATE:
		   /* If the window is now active acquire the audio device */
         if ((BOOL) mp1)
         {
            if (gfPassed && gfSoundOn)
			   {
               mciGenericParms.hwndCallback = hwnd;
               (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusWaveDeviceID,
                                                    MCI_ACQUIREDEVICE,
                                                    (ULONG) MCI_NOTIFY,
                                                    (PVOID) &mciGenericParms,
                                                    (USHORT) 0);
			   }

            if (gfPassed && gfMusicEnabled)
				{
               (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusMidiDeviceID,
                                                    MCI_ACQUIREDEVICE,
                                                    (ULONG) MCI_NOTIFY,
                                                    (PVOID) &mciGenericParms,
                                                    (USHORT) 0);
            }

				/* Resume the game */
				ResumeGame ();
         }
         else
         {
            /* If the window is losing focus pause the game */
				PauseGame ();
         }
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));

      default:
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));
      }

   return ((MRESULT)0);
}

/*************************************************************************
*
*  FUNCTION NAME   : LogoWindowProc
*
*  DESCRIPTION     : This window procedure handles the logo bitmap on
*                    dialog boxes
*
*  INPUT PARAMETERS:
*           HWND   ==>  handle to the window receiving the message
*           ULONG  ==>  command message
*           MPARAM ==>  message specific data
*           MPARAM ==>  message specific data
*
*  RETURNS :
*           MRESULT ==> standard window proc return code
*
*  PSEUDOCODE:
*
**************************************************************************/
MRESULT EXPENTRY LogoWindowProc (HWND   hwnd,
                                 ULONG  msg,
                                 MPARAM mp1,
                                 MPARAM mp2)
{
   HPS                hps;
   RECTL     	       rcl;
	POINTL             ptl[4];

   switch (msg)
      {
      case WM_PAINT:
         /* Get our PS */
         hps = WinBeginPaint (hwnd, NULLHANDLE, &rcl);

			/* Query the extent of the logo window and blit into it */
			WinQueryWindowRect (hwnd, &rcl);
      	ptl[0].x = rcl.xLeft;
      	ptl[0].y = rcl.yBottom;
      	ptl[1].x = rcl.xRight;
      	ptl[1].y = rcl.yTop;
      	ptl[2].x = 0;
      	ptl[2].y = 0;
			ptl[3].x = stBitmap[LOGO].sx;
			ptl[3].y = stBitmap[LOGO].sy;
      	GpiBitBlt (hps, 
						  hpsMemory[LOGO], 
						  4L, 
						  ptl, 
				  	  	  ROP_SRCCOPY,
				     	  BBO_AND);

         WinEndPaint (hps);
         break;

      default:
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));
      }

   return ((MRESULT) 0);
}

/**************************************************************************
*
* FUNCTION NAME: GetMouseMoves
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
VOID GetMouseMoves (BOOL fClear)
{
	POINTL 		 ptl;
   SHORT        sMx, sMy;
	static SHORT sMoveX=0;
	static SHORT sMoveY=0;

	if (fClear)
	{
		/* Reset the movement totals */
		sMoveX = 0;
		sMoveY = 0;

		/* Keep the mouse pointer in the middle of our window */
		WinSetPointerPos (HWND_DESKTOP, (LONG) gsCenterX, (LONG) gsCenterY);
	}
	else
	{
		/* Get the new position */
		WinQueryPointerPos (HWND_DESKTOP, &ptl);
		
		/* Keep the mouse pointer in the middle of our window */
		WinSetPointerPos (HWND_DESKTOP, (LONG) gsCenterX, (LONG) gsCenterY);
		
		sMoveX = sMoveX + (gsCenterX - (SHORT) ptl.x);
		sMoveY = gsCenterY - (SHORT) ptl.y;
		
		/* Handle the up and down */
		stShip.keyDown = stShip.keyUp = FALSE;
   	if (sMoveY > 0)
   	{
  			stShip.keyDown = TRUE;
  		}
   	else if (sMoveY < 0)
   	{
  			stShip.keyUp = TRUE;
   	}
		
		/* Face the appropriate direction (< 1 so that it starts right facing) */
		stShip.fFacingRight = (sMoveX < 1);
		
		/* Thrust if mouse moved far enough off center */
		if (sMoveX > 300 || sMoveX < -300)
		{
   	   stShip.keyThrust = TRUE;
		}
		else
		{
			stShip.keyThrust = FALSE;
		}
	}
}		
		
/**************************************************************************
*
* FUNCTION NAME: LoadAndStretchBitmap
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
VOID LoadAndStretchBitmap (INT i)
{
	BITMAPINFOHEADER bmpData;

	/* Load the bitmap without stretching to get iys size */
   hbm[i] = GpiLoadBitmap (hpsMemory[i], NULLHANDLE,
										 stBitmap[i].bitmap, 0L, 0L);
	
	bmpData.cbFix = sizeof(bmpData);
	GpiQueryBitmapParameters (hbm[i], &bmpData);

	/* Calculate the new size and stretch accordingly */
	if (stBitmap[i].fScale)
	{
		if ((stBitmap[i].cx = (bmpData.cx * 5) / (11 - gsObjectSize)) < 1)
		{
			stBitmap[i].cx = 1;
		}

		if ((stBitmap[i].cy = (bmpData.cy * 5) / (11 - gsObjectSize)) < 1)
		{
			stBitmap[i].cy = 1;
		}
	}
	else
	{
		stBitmap[i].cx = bmpData.cx;
		stBitmap[i].cy = bmpData.cy;
	}

	stBitmap[i].sx = SCALEX (stBitmap[i].cx);
	stBitmap[i].sy = SCALEY (stBitmap[i].cy);

   /* Scale the bitmap offsets */
	stBitmap[i].sdx = SCALEX (stBitmap[i].dx);
	stBitmap[i].sdy = SCALEY (stBitmap[i].dy);

	/* Reload the bitmap, stretching it using the scale factor */
   hbm[i] = GpiLoadBitmap (hpsMemory[i], NULLHANDLE,
								   stBitmap[i].bitmap, 
								   stBitmap[i].sx,
								   stBitmap[i].sy);

	GpiSetBitmap (hpsMemory[i], hbm[i]);
}

/**************************************************************************
*
* FUNCTION NAME: RefreshScreen
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
VOID RefreshScreen (HPS hps)
{
   INT   i;
	RECTL rcl;

	/* 
	** Make sure nothing is drawn until a frame has elapsed - this is 
	** required since the WM_PAINT message could be generated at any time
	*/
	usDrawItems = 0;
	usMarkerItems = 0;
	usBoltItems = 0;

	/* Redraw the screen */
	WinQueryWindowRect (hwndDefClient, &rcl);
   WinFillRect (hps, &rcl, CLR_BLUE);
	DrawScreen (hps);
	DrawStatus (hps);
}

/**************************************************************************
*
* FUNCTION NAME: DrawScreen
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
VOID DrawScreen (HPS hps)
{
	INT 			  i;
   POINTL    	  ptl[3];
	MARKERBUNDLE  mbnd;
	LINEBUNDLE    lbnd;
	POINTL        point;

	/* Erase the old landscape */
	if (stLandDraw.lErasePts > 0)
	{
		lbnd.lColor = CLR_BLUE;
		lbnd.fxWidth = LINEWIDTH_THICK;
		GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH, 0L, &lbnd);
		GpiMove (hps, stLandDraw.pEraseData);
		GpiPolyLine (hps,
						 stLandDraw.lErasePts - 1,
						 &(stLandDraw.pEraseData[1]));
	}

	/* Draw the new landscape */
	if (stLandDraw.lDrawPts > 0)
	{
		lbnd.lColor = CLR_GREEN;
		lbnd.fxWidth = LINEWIDTH_THICK;
		GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH, 0L, &lbnd);
		GpiMove (hps, stLandDraw.pDrawData);
		GpiPolyLine (hps,
						 stLandDraw.lDrawPts - 1,
						 &(stLandDraw.pDrawData[1]));
	}

	/* Draw all markers in list (radar markers, stars and explosions */
	for (i = 0;  i < usMarkerItems;  i++)
	{
		if (stMarkerItem[i].fDraw)
		{
      	ptl[0].x = SCALEX (stMarkerItem[i].x);
      	ptl[0].y = SCALEY (stMarkerItem[i].y);
      	ptl[1].x = ptl[0].x + stBitmap[stMarkerItem[i].bitmap].sx;
      	ptl[1].y = ptl[0].y + stBitmap[stMarkerItem[i].bitmap].sy;

      	ptl[2].x = 0;
      	ptl[2].y = 0;
      	GpiBitBlt (hps,
						  hpsMemory[stMarkerItem[i].bitmap],
			           3L,
						  ptl,
						  ROP_SRCCOPY,
						  BBO_AND);
		}
	}

	/* Draw all items listed in the draw list */
	for (i = 0;  i < usDrawItems;  i++)
	{
		if (stDrawItem[i].fDraw)
		{
      	ptl[0].x = SCALEX (stDrawItem[i].x);
      	ptl[0].y = SCALEY (stDrawItem[i].y);
      	ptl[1].x = ptl[0].x + stBitmap[stDrawItem[i].bitmap].sx;
      	ptl[1].y = ptl[0].y + stBitmap[stDrawItem[i].bitmap].sy;
      	ptl[2].x = 0;
      	ptl[2].y = 0;
      	GpiBitBlt (hps,
						  hpsMemory[stDrawItem[i].fErase ? 
                                             ERASE : stDrawItem[i].bitmap],
			           3L,
						  ptl,
						  ROP_SRCCOPY,
						  BBO_AND);
		}
	}	

	/* Draw all the laser bolts in the list */
	for (i = 0;  i < usBoltItems;  i++)
	{
		lbnd.lColor = stBoltItem[i].lColor;
		GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd);
		GpiMove (hps, &(stBoltItem[i].point1));
		GpiLine (hps, &(stBoltItem[i].point2));
	}

   /* Redraw the screen markers in the radar box */
	lbnd.lColor = CLR_WHITE;
	GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd);
   for (i = 0;  i < 8;  i += 2)
   {
		GpiMove (hps, &(statusPts[SCREEN_MARKER_INDEX + i]));
		GpiLine (hps, &(statusPts[SCREEN_MARKER_INDEX + i + 1]));
   }

	/* Print any text which overlays the action - blacking it out if expired */
	for (i = 0;  i < NUM_TEXTS;  i++)
	{
		if (stText[i].fActive)
		{
		   if (--stText[i].sDuration == 0)
			{
				stText[i].fActive = FALSE;
			}

	      PrintAt (hps, stText[i].x, stText[i].y, 
			         stText[i].fActive ? stText[i].lColor : CLR_BLUE, 
						stText[i].szText, stText[i].fCenter, FALSE);
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: DrawStatus
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
VOID DrawStatus (HPS hps)
{
	INT 			  i;
	LINEBUNDLE    lbnd;
	SHORT         sNumX;
	POLYGON       poly;

	/* Draw a black box for the radar defines by the first 4 pts in array */
	poly.ulPoints = 3;
	poly.aPointl  = &statusPts[1];
	GpiSetColor (hps, CLR_BLACK);
	GpiMove (hps, &statusPts[0]);
	GpiPolygons (hps, 1L, &poly, 0L, 0L);
	lbnd.lColor = CLR_WHITE;
	GpiSetAttrs (hps, PRIM_LINE, LBB_COLOR, 0L, &lbnd);

	/* Draw the lines comprising the status frame */
	for (i = 0;  i < NUM_LINES;  i += 2)
	{
		/* Draw the line */
		GpiMove (hps, &statusPts[i]);
		GpiLine (hps, &statusPts[i+1]);
	}

	/* Calculate the position for the score */
	gsScoreX = (STATUS_WIDTH - 
				         (6 * stBitmap[DIGIT_ZERO].cx + 5 * DIGIT_GAP)) / 2 + 1;
   gsScoreY = SCREEN_HEIGHT - (STATUS_HEIGHT / 2) + 2;

	/* Draw the score and high score in the top left corner */
   DrawBitmap (hps, SCALEX (((STATUS_WIDTH - stBitmap[SCORE].cx) / 2)), 
					SCALEY ((SCREEN_HEIGHT - stBitmap[SCORE].cy - 4)), 
					SCORE, FALSE);
   DisplayNum (hps, glScore, 
					gsScoreX, 
					gsScoreY);
   DrawBitmap (hps, SCALEX (((STATUS_WIDTH - stBitmap[HISCORE].cx) / 2)), 
					SCALEY ((SCREEN_HEIGHT - (STATUS_HEIGHT / 2) - 4 - 
													       stBitmap[HISCORE].cy)), 
					HISCORE, FALSE);
   DisplayNum (hps, stHiscore[0].lScore, 
					gsScoreX, 
					SCREEN_HEIGHT - STATUS_HEIGHT + 2);

	/* Draw the life and smart icons */
	DrawLifeIcons (hps);
   DrawSmartIcons (hps);
}

/*************************************************************************
*
*  FUNCTION NAME   : DrawLifeIcons
*
*  DESCRIPTION     : 
*
*  INPUT PARAMETERS:
*
*  RETURNS :
*
*  PSEUDOCODE:
*
**************************************************************************/
VOID DrawLifeIcons (HPS hps)
{
   INT i;
	SHORT sYSpacing;
	SHORT sYPosition;
	SHORT sDrawNum;

	/* Calculate the spacing and position of first icon */
	sYSpacing = (STATUS_HEIGHT - 3*stBitmap[LIFE].cy)/4 + stBitmap[LIFE].cy;
	sYPosition = PLAY_HEIGHT + STATUS_HEIGHT - sYSpacing;
	sDrawNum = min (gsLives, 3);

	for (i = 0;  i < sDrawNum;  i++)
	{
   	DrawBitmap (hps,
						SCALEX((SCREEN_WIDTH - STATUS_WIDTH + stBitmap[LIFE].cx/2)),
 						SCALEY(sYPosition),
						LIFE, FALSE);
		sYPosition -= sYSpacing;
	}

	for (i = sDrawNum;  i < 3;  i++)
	{
   	DrawBitmap (hps,
						SCALEX((SCREEN_WIDTH - STATUS_WIDTH + stBitmap[LIFE].cx/2)),
 						SCALEY(sYPosition),
						LIFE, TRUE);
		sYPosition -= sYSpacing;
	}
}

/*************************************************************************
*
*  FUNCTION NAME   : DrawSmartIcons
*
*  DESCRIPTION     : 
*
*  INPUT PARAMETERS:
*
*  RETURNS :
*
*  PSEUDOCODE:
*
**************************************************************************/
VOID DrawSmartIcons (HPS hps)
{
   INT i;
	SHORT sYSpacing;
	SHORT sYPosition;
	SHORT sDrawNum;

	/* Calculate the spacing and position of first icon */
	sYSpacing = (STATUS_HEIGHT - 3*stBitmap[SMART].cy)/4 + stBitmap[SMART].cy;
	sYPosition = PLAY_HEIGHT + STATUS_HEIGHT - sYSpacing;
	sDrawNum = min (gsSmarts, 3);

	for (i = 0;  i < sDrawNum;  i++)
	{
   	DrawBitmap (hps, SCALEX((SCREEN_WIDTH - (stBitmap[SMART].cx * 3) / 2)),
 						SCALEY(sYPosition),
						SMART, FALSE);
		sYPosition -= sYSpacing;
	}

	for (i = sDrawNum;  i < 3;  i++)
	{
   	DrawBitmap (hps, SCALEX((SCREEN_WIDTH - (stBitmap[SMART].cx * 3) / 2)),
 						SCALEY(sYPosition),
						SMART, TRUE);
		sYPosition -= sYSpacing;
	}
}

/*************************************************************************
*
*  FUNCTION NAME   : IncScore
*
*  DESCRIPTION     : 
*
*  INPUT PARAMETERS:
*
*  RETURNS :
*
*  PSEUDOCODE:
*
**************************************************************************/
VOID IncScore (LONG lInc)
{
   LONG  lOldScore;

	lOldScore = glScore;
	if ((glScore += lInc) < 0)
	{
		glScore = 0;
	}
	PAINT (DRAW_SCORE);

	/* See if we've passed a 10000 point boundary for extra life and smart */
	if (lOldScore / BONUS_SCORE != glScore / BONUS_SCORE)
	{
      MakeSound (SOUND_BONUS_LIFE);
		gsSmarts++;
		gsLives++;
		PAINT (DRAW_ICONS);
	}
}

/*************************************************************************
*
*  FUNCTION NAME   : SizeWindowProc
*
*  DESCRIPTION     : This window procedure traps the WM_ADJUSTWINDOWPOS
*							message.
*
*  INPUT PARAMETERS:
*           HWND   ==>  handle to the window receiving the message
*           ULONG  ==>  command message
*           MPARAM ==>  message specific data
*           MPARAM ==>  message specific data
*
*  RETURNS :
*           MRESULT ==> standard window proc return code
*
*  PSEUDOCODE:
*
**************************************************************************/
MRESULT EXPENTRY SizeWindowProc (HWND   hwnd,
             						   ULONG  msg,
             						   MPARAM mp1,
             						   MPARAM mp2)
{
   PSWP       pswp;
   RECTL      rcl;
   ULONG      rc;
	SHORT      cx, cy;

   switch(msg)
   {
		/* Trap the sizing message to not allow the window to be sized */
      case WM_ADJUSTWINDOWPOS:
         pswp = (PSWP)mp1;
			if (pswp->fl & SWP_SIZE)
			{
   			/* Get the client window size from this frame size */
   			rcl.yBottom = 0;
   			rcl.xLeft   = 0;
   			rcl.yTop    = pswp->cy;
   			rcl.xRight  = pswp->cx;
   			WinCalcFrameRect (hwndDefFrame, &rcl, TRUE);
				cx = (SHORT) (rcl.xRight - rcl.xLeft);
				cy = (SHORT) (rcl.yTop - rcl.yBottom);

				/* Adjust size of least change to keep aspect ratio */
				if (abs (cx - (SHORT) glWindowX) > abs (cy - (SHORT) glWindowY))
				{
					/* Change in X is greater than in Y so use X size */
					cy = (cx * SCREEN_HEIGHT) / SCREEN_WIDTH;
				}
				else
				{
					/* Change in Y is greater than in X so use Y size */
					cx = (cy * SCREEN_WIDTH) / SCREEN_HEIGHT;
				}

   			/* Recalculate the frame size from this window size */
   			rcl.yBottom = 0;
   			rcl.xLeft   = 0;
   			rcl.yTop    = cy;
   			rcl.xRight  = cx;
   			WinCalcFrameRect (hwndDefFrame, &rcl, FALSE);
				pswp->cx = (SHORT) (rcl.xRight - rcl.xLeft);
				pswp->cy = (SHORT) (rcl.yTop - rcl.yBottom);
			}
			break;

   	default:
			break;
   }

   /* Return using the normal frame window procedure */
   return (*pfnOldFrameProc)(hwnd, msg, mp1, mp2);
}


/**************************************************************************
*
* FUNCTION NAME: CreateLandscape
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
SHORT CreateLandscape (VOID)
{
   SHORT x   = 0;
   SHORT y   = LOWEST_LAND;
   SHORT dy  = 1;
	SHORT dx;
   SHORT i   = HEADER_POINTS;	
   SHORT inc = 0;
	SHORT sStart;
	SHORT sPoints;
	SHORT sHighPoints = 0;
	BOOL  fLowerHill = FALSE;

   /* Loop randomly creating the hills */
   while (x < PLANET_SIZE)
   {
      /* make sure we haven't exceeded the array limit */
      if (i == MAX_VERTICES)
	   {
	  	   return (0);
	   }

      land[i].x = x;
      land[i].y = y;

      /* The hill can rise or fall between predefined limits */
      dx = (rand() % (MAX_UNDULATION - MIN_UNDULATION)) + MIN_UNDULATION;

      /* Ensure the hill does not go outside the bounds */
      if (dy == 1)
      {
         /*
			** The hill can rise between predefined limits - the max rise is
			** reduced to minimum if the hill is getting too large
			*/
         dx = (fLowerHill) ? MIN_UNDULATION :
			       (rand() % (MAX_UNDULATION - MIN_UNDULATION)) + MIN_UNDULATION;

         if (y + dx > HIGHEST_LAND)
      	{
   	      dx = HIGHEST_LAND - y;
   	   }
	   }
      else
      {
         /* The hill can fall between predefined limits */
         dx = (rand() % (MAX_UNDULATION - MIN_UNDULATION)) + MIN_UNDULATION;

         if (y - dx < LOWEST_LAND)
   	  	{
   	     	dx = y - LOWEST_LAND;

				/* Reset the lower flag since the hill has completely descended */
				fLowerHill = FALSE;
				sHighPoints = 0;
   	  	}
	   }

      /* increment the land positions */
      x += dx;
      y += (dx * dy);
      dy = -dy;
      i++;

		if (!fLowerHill && y >= HIGHEST_LAND/2)
		{
			if (++sHighPoints == LOWER_HILL_LIMIT)
			{
				fLowerHill = TRUE;
			}
		}
   }

   /*
   ** At this point the land is defined, except that the start and end
   ** points do not correspond.  This piece of code makes sure that
   ** the last land (x, y) connects correctly to land[HEADER_PTS] (x, y)
   */

   /* Set the index to the last low point - can tell this by sign of dy */
   i = (dy == -1) ? (i - 1) : (i - 2);

   /* Calculate the distance to the origin point */
   dx = PLANET_SIZE - land[i].x;
   dy = land[i].y - land[HEADER_POINTS].y;

   /*
   ** if (dx-dy)/2 is not positive and small enough then calculate the whole
   ** land again.  This is a bit of a cop out, but simplifies the logic.  This
   ** is a little dangerous - if you had really bad luck it could recurse
   ** indefinitely!!
   */
   if ((inc = (dx - dy)/2) < 0 || inc > MAX_UNDULATION)
   {
      return (CreateLandscape ());
   }

   /* Add the new point which will join up to land[HEADER_PTS] */
   land[i + 1].x = land[i].x + inc;
   land[i + 1].y = land[i].y + inc;
	sPoints = i + 1 - HEADER_POINTS;

	/*
	** Now fill in the header points.  These are replicated from the last pts
	** in the array.  They provide continuity when moving to x < 0.  It makes
	** drawing the landscape much simpler, since don't need to worry about
	** wrapping around.
	*/
	sStart = sPoints + HEADER_POINTS + 1;
	for (i = 0;  i < HEADER_POINTS;  i++)
	{
		land[i].x = land[sStart + i].x - PLANET_SIZE;
		land[i].y = land[sStart + i].y;
	}

	/*
	** Finally replicate enough of the land to extend beyond the end of the
	** land array (beyond sPoints) to provide continuity in that direction.
	*/
	for (i = 0; land[i+HEADER_POINTS].x <= (SCREEN_WIDTH+MAX_UNDULATION); i++)
	{
		land[sPoints+HEADER_POINTS+i+1].x = land[HEADER_POINTS+i].x + PLANET_SIZE;
		land[sPoints+HEADER_POINTS+i+1].y = land[HEADER_POINTS+i].y;
	}

	/*
	** Return the number of points in the array excluding the replicated
	** header and trailer points
	*/
   return (sPoints + 1);
}


/**************************************************************************
*
* FUNCTION NAME: CalculateAltitudes
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
VOID CalculateAltitudes (VOID)
{
	INT i;
	SHORT sLandIndex;
	SHORT sDy;

	/* Start from the 'first' point in the landscape array */
	height[0] = land[HEADER_POINTS].y;
	sDy = 1;

	/* The land index points to the next point */
	sLandIndex = HEADER_POINTS + 1;

	/* Look at the landscape array to calculate changes in altitude */
	for (i = 1;  i < PLANET_SIZE;  i++)
	{
		height[i] = height[i-1] + sDy;
		if (land[sLandIndex].x == i)
		{
			sLandIndex++;
			sDy = -sDy;
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: DisplayNum
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
VOID DisplayNum (HPS hps, LONG lNumber, SHORT x, SHORT y)
{
	CHAR   szNumber[7];		/* Number is 6 digits long */
	INT    i;
	POINTL ptl[3];
	LONG   bitmap;

	/* Start at the given coordinates */
	ptl[0].x = SCALEX (x);
	ptl[0].y = SCALEY (y);

	sprintf (szNumber, "%06d", lNumber);
	for (i = 0;  i < 6;  i++)
	{
		bitmap = DIGIT_ZERO + szNumber[i] - '0';
      ptl[1].x = ptl[0].x + stBitmap[bitmap].sx;
      ptl[1].y = ptl[0].y + stBitmap[bitmap].sy;
      ptl[2].x = 0;
      ptl[2].y = 0;
      GpiBitBlt (hps,
					  hpsMemory[bitmap],
		           3L,
					  ptl,
					  ROP_SRCCOPY,
					  BBO_AND);

		/* Increment the X coordinate for the next digit */
		ptl[0].x += stBitmap[bitmap].sx + SCALEX (DIGIT_GAP);
	}
}

/**************************************************************************
*
* FUNCTION NAME: ProcessEvent
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
VOID ProcessEvent	(VOID)
{
   INT i;

	if (++gulWaveTimer == stEvent[gusNextEvent].ulTime)
	{
		/* To prevent hangs in WM_PAINT */
		DosPostEventSem (hPaintEventSem);

		switch (stEvent[gusNextEvent].eEvent)
		{
			case LANDER_DROP:
				/* If the land is gone (no more humans) add mutants instead */
				if (stLandDraw.fActive)
				{
    				AddLanders (5);
				}
				else
				{
					for (i = 0;  i < 5;  i++)
					{
						AddMutant ((rand() % PLANET_SIZE), 
			                    (PLAY_HEIGHT - stBitmap[MUTANTL].cy),
									  TRUE);
					}
				}
				break;

			case WAVE_END:
				gsGameOn = WAVE_OVER;
				WaveOver ();
				gsGameOn = TRUE;
				return;

			case SKIMMER_DROP:
			   AddSkimmer ();
				break;

			default:
				break;
		}

		/* Increment the pointer to point at the next event */
		gusNextEvent++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: WaveOver
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
VOID WaveOver (VOID)
{
	INT   i;


	/* Display a bitmap for each human - sleeping in between */
	if (usNumHumans == 0)
	{
   	gsPaintInfo2 = FALSE;		/* This means not erase */
		gsPaintInfo1 = i;
		PAINT (DRAW_WAVEOVERNONELEFT);
	}
	else
	{
		for (i = 0;  i < usNumHumans && gAction != EXIT;  i++)
		{
	   	gsPaintInfo2 = FALSE;		/* This means not erase */
			gsPaintInfo1 = i;
			PAINT (DRAW_WAVEOVER);
			IncScore (SAVED_HUMAN_SCORE);
      	MakeSound (SOUND_AWARD);
			DosSleep (400);
		}
	}

	if (gAction != EXIT)
	{
		/* Wait a short while before removing what we've just drawn */
		DosSleep (4000);
   	gsPaintInfo2 = TRUE;				/* Erase */
		gsPaintInfo1 = i;
		if (usNumHumans > 0)
		{
			PAINT (DRAW_WAVEOVER);
		}
		else
		{
			PAINT (DRAW_WAVEOVERNONELEFT);
		}

		/* 
		** Increment the wave number but when setting up the wave ensure we don't 
		** overrun the wave array
		*/
		++gsWave;
		SetupWave (min ((NUM_WAVES - 1), gsWave));
	}
}


/**************************************************************************
*
* FUNCTION NAME: DrawBitmap
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
VOID DrawBitmap (HPS hps, SHORT x, SHORT y, LONG bitmap, BOOL fErase)
{
	POINTL ptl[3];

   ptl[0].x = x;
   ptl[0].y = y;
   ptl[1].x = ptl[0].x + stBitmap[bitmap].sx;
   ptl[1].y = ptl[0].y + stBitmap[bitmap].sy;
   ptl[2].x = 0;
   ptl[2].y = 0;
   GpiBitBlt (hps, 
				  hpsMemory[fErase ? ERASE : bitmap], 
				  3L, 
				  ptl, 
				  ROP_SRCCOPY,
				  BBO_AND);
}


/**************************************************************************
*
* FUNCTION NAME: PrintAt
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
VOID PrintAt (HPS hps, SHORT x, SHORT y, LONG lColor, 
										  PCHAR szText, BOOL fCenter, BOOL fErase)
{
	RECTL 		rcl;
	INT   		iLen;

	/* Calculate the length of the text string */
	iLen = strlen (szText);

	if (fCenter)
	{
		/* 
		** Make the bounding box the full width of the screen then query the
		** extent and use this to draw with.  The call to WinDrawText does not
		** draw anything but merely modifies the rcl points.
		*/
		rcl.xLeft = 0;
		rcl.yBottom = y;
		rcl.xRight = SCREEN_WIDTH;
		rcl.yTop = y + gsCharcy;
		WinDrawText (hps, iLen, szText, &rcl, 0, 0, DT_CENTER | DT_QUERYEXTENT);
		rcl.xRight++;
		rcl.yTop++;
	}
	else
	{
		rcl.xLeft = x;
		rcl.yBottom = y;
		rcl.xRight = x + iLen * gsCharcx;
		rcl.yTop = y + gsCharcy;
	}

	/* Draw the text in the specified color */
	WinDrawText (hps, iLen, szText, &rcl, lColor, CLR_BLUE, 
													 			 fErase ? DT_ERASERECT : 0);
}


/**************************************************************************
*
* FUNCTION NAME: ReadIniFile
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
VOID ReadIniFile (VOID)
{
	FILE *fp;
	INT i, j, c, iTemp;
	INT iVersion;

	if ((fp = fopen (szIniFile, "r")) != NULL)
	{
		/* Read the version number N.NN (there's no '.' in the string */
		fscanf (fp, "%d %d", &iTemp, &iVersion);
		iVersion += iTemp*100;

	   /* Read the key scan codes */
		fscanf (fp,
		        "%d %hu %hu %hu %hu %hu %hu %hu",
              &iTemp,
				  &gstKeyDefs.usUpKey,
				  &gstKeyDefs.usDownKey,
				  &gstKeyDefs.usTurnKey,
				  &gstKeyDefs.usThrustKey,
				  &gstKeyDefs.usFireKey,
				  &gstKeyDefs.usSmartKey,
				  &gstKeyDefs.usHyperKey);
      gstKeyDefs.fTurnThrust = (BOOL) iTemp;

		/* Read the hi score table */
		for (i = 0;  i < NUM_HISCORES;  i++)
		{
			fscanf (fp, "%ld", &stHiscore[i].lScore);

			/* Consume input until we hit the quote character */
			while ((c = fgetc (fp)) != (INT) '\"')
 				;

			/* Read in the name until we hit the closing quote */
			for (j = 0;  ;  j++)
			{
			   if ((c = fgetc (fp)) != (INT) '\"')
				{
					stHiscore[i].szName[j] = (CHAR) c;
				}
				else
				{
					stHiscore[i].szName[j] = '\0';
					break;
				}
			}

			fscanf (fp, "%d", &iTemp);
			stHiscore[i].fBullets = (BOOL) iTemp;

			fscanf (fp, "%hd %hd", &stHiscore[i].sShips, &stHiscore[i].sSmarts);

			fscanf (fp, "%hd %hd %hd", &stHiscore[i].sWave, 
			             &stHiscore[i].sAliens, &stHiscore[i].sHitRate);

			fscanf (fp, "%s", stHiscore[i].szDate);
		}

		/* Read the registered flag */
		fscanf (fp, "%d", &iTemp);
		gfRegistered = (BOOL) iTemp;

		/* Read the frame rate and the object size */
		fscanf (fp, "%d %hd", &gFramesPerSec, &gsObjectSize);

		/* Read the bullets flag */
		fscanf (fp, "%d", &iTemp);
		gfBullets = (BOOL) iTemp;

		/* Read the starting number of ships and smarts */
		fscanf (fp, "%hd %hd", &gsStartShips, &gsStartSmarts);

		/* Read the sound state */
		fscanf (fp, "%d", &iTemp);
		gfSoundOn = (BOOL) iTemp;

		/* Read the music state */
		fscanf (fp, "%d", &iTemp);
		gfMusicEnabled = (BOOL) iTemp;

		/* Read the mouse state */
		fscanf (fp, "%d", &iTemp); 
		gfMouseEnabled = (BOOL) iTemp;

		/* Read the bubbles state */
		fscanf (fp, "%d", &iTemp); 
		gfBubbles = (BOOL) iTemp;

		/* Read the scanner state */
		fscanf (fp, "%hd", &stScanner.sMode);

		/* Read the window position and size */
		fscanf (fp, "%hd %hd %ld %ld", &gsPosX, &gsPosY, &glWindowX, &glWindowY);

		/*
		** Now read stuff that was added after the original version
		*/
		if (iVersion >= 101)
		{
			/* If v1.1 the registered name was added */

			/* Consume input until we hit the quote character */
			while ((c = fgetc (fp)) != (INT) '\"')
 				;

			/* Read in the name until we hit the closing quote */
			for (j = 0;  ;  j++)
			{
			   if ((c = fgetc (fp)) != (INT) '\"')
				{
					gszRegName[j] = (CHAR) c;
				}
				else
				{
					gszRegName[j] = '\0';
					break;
				}
			}
		}
		else
		{
			strcpy (gszRegName, "Anonymous");
		}
	}

	fclose (fp);
}

/**************************************************************************
*
* FUNCTION NAME: WriteIniFile
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
VOID WriteIniFile (VOID)
{
	FILE *fp;
	INT i;

	/* A bit mean here!  Only save the settings if the game is registered */
	if ((fp = fopen (szIniFile, "w")) != NULL)
	{
	   /* Write the version number */
		fprintf (fp, VERSION_STRING);

	   /* Write the key scan codes */
		fprintf (fp, 
		         "%d %hu %hu %hu %hu %hu %hu %hu\n",
               (INT) gstKeyDefs.fTurnThrust,
				   gstKeyDefs.usUpKey,
				   gstKeyDefs.usDownKey,
				   gstKeyDefs.usTurnKey,
				   gstKeyDefs.usThrustKey,
				   gstKeyDefs.usFireKey,
				   gstKeyDefs.usSmartKey,
				   gstKeyDefs.usHyperKey);
	
		/* Write the hi score table */
		for (i = 0;  i < NUM_HISCORES;  i++)
		{
			fprintf (fp, "%ld \"%s\" ", stHiscore[i].lScore, stHiscore[i].szName);
	
			fprintf (fp, "%d ", (INT) stHiscore[i].fBullets);
	
			fprintf (fp, "%hd %hd ", stHiscore[i].sShips, stHiscore[i].sSmarts);
	
			fprintf (fp, "%hd %hd %hd ", stHiscore[i].sWave, 
			             stHiscore[i].sAliens, stHiscore[i].sHitRate);
	
			fprintf (fp, "%s\n", stHiscore[i].szDate);
		}
	
		/* Write the registered flag, frame rate and object size */
		fprintf (fp, "%d %d %hd ", (INT) gfRegistered, 
													gFramesPerSec, gsObjectSize);
	
		/* Write the bullets flag */
		fprintf (fp, "%d ", (INT) gfBullets);
	
		/* Write the starting number of ships and smarts */
		fprintf (fp, "%hd %hd ", gsStartShips, gsStartSmarts);
	
		/* Write the sound states, bubbles state and the scanner mode */
		fprintf (fp, "%d %d %d %d %hd ", (INT) gfSoundOn, 
											      (INT) gfMusicEnabled, 
													(INT) gfMouseEnabled,
													(INT) gfBubbles,
													stScanner.sMode);

		/* Write the window position and size */
		fprintf (fp, "%hd %hd %ld %ld", gsPosX, gsPosY, glWindowX, glWindowY);

		/* Write the registered name */
		fprintf (fp, "\n\"%s\"", gszRegName);
	}
	
	fclose (fp);
}		


/**************************************************************************
*
* FUNCTION NAME: AttractMode
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
VOID AttractMode (VOID)
{
	INT    i, j;
	CHAR   szString[50];
	SHORT  dy, dx;
	LONG   lColor;
	RECTL  rcl;

	/* Set up the bubbles for the display */
	gsWorldShift = 0;
	InitStars (TRUE);

	while (gAction != START && gAction != EXIT)
	{
   	/* Clean up the screen */
		PAINT (CLEAN_SCREEN);

      /* Play the title music */
      MakeSound (SOUND_TITLE);
      DosSleep (100);

		/* Zoom the logo into the frame */
		for (j = 1; 
		     gAction != START && gAction != EXIT && j < SCREEN_WIDTH;
			  j += ((j * 5) / SCREEN_WIDTH) + 1)
		{
			/* Move the bubbles */
	   	usDrawItems = 0;
			MoveStars (TRUE, FALSE);

			if (j < SCREEN_WIDTH / 2)
			{
				i = j;
			}

			/* Set up the POINTL array */
      	ptlLogo[0].x = SCALEX(((SCREEN_WIDTH / 2) - (i / 2)));
      	ptlLogo[0].y = SCALEY((((SCREEN_HEIGHT * 3) / 5) - 
										(i * SCREEN_HEIGHT) / (2 * SCREEN_WIDTH)));
      	ptlLogo[1].x = ptlLogo[0].x + SCALEX(i);
      	ptlLogo[1].y = ptlLogo[0].y + SCALEY((((i * SCREEN_HEIGHT) / 
																				SCREEN_WIDTH)));
      	ptlLogo[2].x = 0;
      	ptlLogo[2].y = 0;
			ptlLogo[3].x = stBitmap[LOGO].sx;
			ptlLogo[3].y = stBitmap[LOGO].sy;

      	/* Tell the PM thread that we're drawing the logo */
      	PAINT (DRAW_LOGO);
      }

   	/* Clean up the screen */
		PAINT (CLEAN_SCREEN);

		/* Display the high score table for 5 seconds */
		for (i = 0;  gAction != START && gAction != EXIT && i < 50;  i++)
		{
			PAINT (DRAW_HISCORES);
			DosSleep (100);
		}

		/* Play the game again if demo mode is set to play */
		if (eDemoMode == DEMO_PLAY && gAction != EXIT)
		{
			gAction = START;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: CheckScore
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
VOID CheckScore (VOID)
{
   INT i, j;
	INT iIndex;
	BOOL fEntered = FALSE;
	time_t lTime;
	struct tm *tm;

   /* Print the game over banner for 3 seconds */
   MakeSound (SOUND_GAMEOVER);
	for (i = 0;  i < 30 && gAction != EXIT;  i++)
	{
		gsPaintInfo1 = FALSE;
		PAINT (DRAW_GAMEOVER);
	   DosSleep (100);
	}

	/* Erase the banner */
	gsPaintInfo1 = FALSE;
	PAINT (DRAW_GAMEOVER);

	/* Compare the score to see if this goes into the high score table */
	for (i = 0;  eDemoMode == DEMO_NONE && i < NUM_HISCORES;  i++)
	{
	   if (glScore > stHiscore[i].lScore)
		{
		   break;
		}
	}

	/* Get the user's input if this is a high score */
	if (eDemoMode == DEMO_NONE && i < NUM_HISCORES)
	{
		PAINT (DRAW_GETNAME);

		/* Initialize the hiscore name to underscores with spaces at each end */
		for (j = 0;  j < HISCORE_NAME_LEN + 4;  j++)
		{
			gszName[j] = ' ';
		}
		for (j = 2;  j < HISCORE_NAME_LEN + 2;  j++)
		{
			gszName[j] = '_';
		}
		gszName[HISCORE_NAME_LEN+4] = '\0';
		iIndex = 2;

		/* Initialize the key hit flag */
		gstKey.fHit = FALSE;

		while (gAction != EXIT && iIndex < (HISCORE_NAME_LEN + 2) && !fEntered)
		{
		   /* Display the current string in a random colour */
			PAINT (DRAW_GETNAME);

		   /* See if any keys were pressed */
			if (gstKey.fHit)
			{
				if (gstKey.fVirtual && gstKey.usVirtKey == VK_BACKSPACE)
				{
					/* Delete the last character */
					if (iIndex > 2)
					{
						gszName[--iIndex] = '_';
					}
				}
				else if (gstKey.fVirtual && gstKey.usVirtKey == VK_NEWLINE)
				{
					fEntered = TRUE;
				}
				else if (gstKey.fVirtual && gstKey.usVirtKey == VK_SPACE)
				{
					gszName[iIndex++] = ' ';
				}
				else if (!gstKey.fVirtual)
				{
					gszName[iIndex++] = gstKey.usChar;
				}
				gstKey.fHit = FALSE;
			}
			else
			{
				PAINT (DRAW_GETNAME);
				DosSleep (50);
			}
		}

		if (gAction != EXIT)
		{
			/* Move the scores down to make room for this one */
			for (j = NUM_HISCORES-1;  j > i;  j--)
			{
				stHiscore[j] = stHiscore[j-1];
			}

			/* 
			** Remove leading spaces and replace the '_' characters in the 
			** name just entered
			*/
			for (j = 0;  j < HISCORE_NAME_LEN;  j++)
			{
			   gszName[j] = gszName[j+2];
				if (gszName[j] == '_')
				{
					gszName[j] = '\0';
					break;
				}
			}
			gszName[HISCORE_NAME_LEN] = '\0';

			/* Store the new score */
			stHiscore[i].lScore = glScore;
			strcpy (stHiscore[i].szName, gszName);
			stHiscore[i].fBullets = gfBullets;
			stHiscore[i].sShips = gsStartShips;
			stHiscore[i].sSmarts = gsStartSmarts;
			stHiscore[i].sWave = gsWave;
			stHiscore[i].sAliens = gsAliens;

			/* Avoid that divide by zero problem */
			if (gsShots > 0)
			{
				stHiscore[i].sHitRate = (gsAliens * 100) / gsShots;
			}
			else
			{
				stHiscore[i].sHitRate = 0;
			}

			/* Get the current time */
   		time ((time_t *) &lTime);
			tm = localtime (&lTime);
			sprintf (stHiscore[i].szDate,
			         "%02d/%0d/%02d", 
						tm->tm_mon+1, tm->tm_mday, tm->tm_year);
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: PauseGame
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
VOID PauseGame (VOID)
{
	if (gsGameOn)
	{
 		/* Set the window title */
 		WinSetWindowText (hwndDefFrame, "Aquanaut! - PAUSED");
 
   	/* Check the menu item */
   	WinCheckMenuItem (hwndMenu, MENU_PAUSE_ID, TRUE);
		
		gAction = PAUSED;
	}
}

/**************************************************************************
*
* FUNCTION NAME: ResumeGame
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
VOID ResumeGame (VOID)
{
   INT i;

	if (gsGameOn)
	{
		/* Set the window title */
		WinSetWindowText (hwndDefFrame, "Aquanaut!");
 
   	/* Uncheck the menu item */
   	WinCheckMenuItem (hwndMenu, MENU_PAUSE_ID, FALSE);

		gAction = NONE;
	}
}

/**************************************************************************
*
* FUNCTION NAME: EnableMouse
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
VOID EnableMouse (BOOL fEnable)
{
   static BOOL fVisible = TRUE;		/* Track current pointer state */

	if (fEnable && gfMouseEnabled)
	{
	   if (gsGameOn && fVisible)
		{
			/* Hide the pointer */
			WinShowPointer (HWND_DESKTOP, FALSE);
			fVisible = FALSE;
		}

		/* Get the current mouse position to be centered in our window */
		WinSetPointerPos (HWND_DESKTOP, (LONG) gsCenterX, (LONG) gsCenterY);
	}
	else if (!fEnable)
	{
	   if (!fVisible)
		{
			/* Show the pointer */
			WinShowPointer (HWND_DESKTOP, TRUE);
			fVisible = TRUE;
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: UseDemoKeys
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
VOID UseDemoKeys (CHAR buffer[], PULONG pulFrame)
{
	CHAR val;

	/* Get the current value and increment the pointer */
	val = buffer[*pulFrame];
	(*pulFrame)++;

	stShip.keyUp = val & DEMO_UP;
	stShip.keyDown = val & DEMO_DOWN;
   stShip.keyThrust = val & DEMO_THRUST;
	stShip.keyFire = val & DEMO_FIRE;
	stShip.keySmart = val & DEMO_SMART;
	stShip.keyHyper = val & DEMO_HYPER;
   stShip.fFacingRight = val & DEMO_FACING_RIGHT;
}

/**************************************************************************
*
* FUNCTION NAME: UseSeed
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
VOID UseSeed (CHAR buffer[], PULONG pulFrame)
{
   CHAR cSeed;

	/* Get the current value and increment the pointer */
	cSeed = buffer[*pulFrame];
	(*pulFrame)++;

	if (!(cSeed & DEMO_SRAND_VAL))
	{
		DosBeep (100,100);
		printf ("Data error at index %d!\n", *pulFrame - 1);
	}

	/* Use the stored seed value */
	srand ((LONG) cSeed);
}

/**************************************************************************
*
* FUNCTION NAME: SaveDemoKeys
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
VOID SaveDemoKeys (CHAR buffer[], PULONG pulFrame)
{
	CHAR val = 0;

	if (stShip.keyUp) 
		val |= DEMO_UP;
	if (stShip.keyDown)
		val |= DEMO_DOWN;
   if (stShip.keyThrust)
		val |= DEMO_THRUST;
	if (stShip.keyFire)
		val |= DEMO_FIRE;
	if (stShip.keySmart)
		val |= DEMO_SMART;
	if (stShip.keyHyper)
		val |= DEMO_HYPER;
   if (stShip.fFacingRight)
		val |= DEMO_FACING_RIGHT;

	/* Save the value in the buffer */
	if (*pulFrame < MAX_DEMO_FRAMES)
	{
		buffer[*pulFrame] = val;
		(*pulFrame)++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: SaveSeed
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
VOID SaveSeed (CHAR buffer[], PULONG pulFrame, LONG seed)
{
   CHAR cSeed;

	/* Save the value in the buffer */
	if (*pulFrame < MAX_DEMO_FRAMES)
	{
		cSeed = (((CHAR) seed) & 0x7F) | DEMO_SRAND_VAL;
		srand ((LONG) cSeed);
		acDemo[*pulFrame] = cSeed;
 		(*pulFrame)++;
	}
}

/**************************************************************************
*
* FUNCTION NAME: StoreDemoData
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
VOID StoreDemoData (PCHAR pc, ULONG ulSize)
{
	FILE *fp;

	fp = fopen (DEMO_FILE, "wb");
	fwrite (pc, ulSize, 1, fp);
	fclose (fp);
}

/**************************************************************************
*
* FUNCTION NAME: LoadDemoData
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
BOOL LoadDemoData (PCHAR pc, ULONG ulSize)
{
	FILE *fp;
	BOOL  fFound = FALSE;

	if ((fp = fopen (DEMO_FILE, "rb")) != NULL)
	{
		fread (pc, ulSize, 1, fp);
		fclose (fp);
		fFound = TRUE;
	}

	return (fFound);
}

