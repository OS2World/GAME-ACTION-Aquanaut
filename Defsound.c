/***************************************************************************
*																									*
*  FILE  : DEFSOUND.C   																	*
*																									*
*	DATE  : August 94																			*
*																									*
*	NAME  : Paul Stanley																		*
*																									*
*	This file contains the sound routines routines for the def game using   *
*  MMPM/2 mci calls.                                                       *
*																									*
***************************************************************************/

#define INCL_DOSMODULEMGR
#include "def.h"
#include "defid.h"
#include "deftypes.h"
#include "defgbls.h"

/*
** TYPES
*/
typedef struct
{
   ULONG ulCmd;
   ULONG ulOp1;
   ULONG ulOp2;
   ULONG ulOp3;
} PLAYLIST;

typedef struct
{
   PULONG pulData;
   ULONG  ulSize;
   USHORT usPriority;
   PCHAR  szFilename;
} SOUNDDATA;

/* 
** Global variables
*/
SOUNDDATA stWaveData[] =
{
    {NULL, 0, 1, "BONUS.WAV"},    // bonus life
    {NULL, 0, 4, "TREASSAV.WAV"}, // treasure saved
    {NULL, 0, 5, "TREASPIC.WAV"}, // treasure picked up
    {NULL, 0, 3, "SMART.WAV"},    // smart fired
    {NULL, 0, 3, "APPEAR.WAV"},   // lander formation
    {NULL, 0, 7, "LASER.WAV"},    // laser sound
    {NULL, 0, 2, "BOOM.WAV"},     // ship exploding
    {NULL, 0, 2, "HYPER.WAV"},    // go into hyperspace
    {NULL, 0, 2, "WAVEOVER.WAV"}, // when the last one is hit
    {NULL, 0, 3, "JELLY.WAV"},    // jellyfish is hit
    {NULL, 0, 4, "TREASHIT.WAV"}, // treasure is hit
    {NULL, 0, 4, "SONAR.WAV"},    // sonar 'ping'
    {NULL, 0, 2, "AQUANAUT.WAV"}, // title music
    {NULL, 0, 2, "AWARD.WAV"},    // noise as treasure is shown
    {NULL, 0, 2, "THANKYOU.WAV"}, // mermaid rescued
    {NULL, 0, 2, "AAARGH.WAV"},   // mermaid killed
    {NULL, 0, 1, "PEEKABOO.WAV"}, // mermaid appears
    {NULL, 0, 1, "GAMEOVER.WAV"}, // "Game over" voice
    {NULL, 0, 2, "RASPBERY.WAV"}, // Rubber ring raspberry noise
    {NULL, 0, 2, "GETRING.WAV"}   // Rubber ring picked up by ship
};

#define NUM_WAV_FILES (sizeof(stWaveData)/sizeof(SOUNDDATA))

PLAYLIST stPlaylist[] =
{
    DATA_OPERATION,    0, 0, 0,   // Line 0: File to play
    EXIT_OPERATION,    0, 0, 0    // Line 1: Exit
};

/* Function prototypes */
VOID SoundError (PSZ pszError);

/**************************************************************************
*
* FUNCTION NAME: HandleSounds
*
* DESCRIPTION:
* A separate thread of execution which:
* + Uses MMIO to open the necessary waveaudio files, allocate memory
*   for each file, read the waveaudio data into memory, and build
*   a global array of wave data.
* + Opens the default waveaudio device, using MCI
* + Sets up the waveaudio device instance to handle the specific
*   settings of the wave files being used.
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID HandleSounds (PVOID dummy)
{
    HAB            hab;                  /* Create an anchor block for */
    HMQ            hmq;
    HMMIO          hmmio;                /* MMIO handle */
    MMIOINFO       mmioInfo;             /* For mmioOpen() */
    MMAUDIOHEADER  mmAudioHeader;        /* Filled in by mmioGetHead() */
    ULONG          ulBytesRead;
    BOOL           fContinue = TRUE;     /* Keeps track of any failed APIs */
    USHORT         i;
    ULONG          ulSize, *pulData;
    ULONG          ulCount;
    ULONG          rc;
    ULONG          ulOpenWaveFlags = MCI_WAIT | MCI_OPEN_ALIAS | 
                                     MCI_OPEN_SHAREABLE | MCI_OPEN_PLAYLIST;
    ULONG          ulOpenMidiFlags = MCI_WAIT | MCI_OPEN_ALIAS | 
                                     MCI_OPEN_SHAREABLE | MCI_OPEN_ELEMENT;
    CHAR           szString[256];
    MCI_OPEN_PARMS mciOpenParms;
    MCI_PLAY_PARMS mciPlayParms;
    MCI_GENERIC_PARMS mciGenericParms;


    /* There are no sounds playing as yet */
    gsSoundPlaying = -1;
    gsMusicAction = NONE;

    hmq = WinCreateMsgQueue (hab, 1000);

    /* Set the pointer to an hourglass */
    WinSetPointer (HWND_DESKTOP,
                   WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));        /* Return the system pointer's handle.       */

    /* 
    ** Open each wave file via MMIO, allocate enough memory for the wave,
    ** read the data into the buffer, and close the file (using MMIO)
    */
    for (i = 0;  i < NUM_WAV_FILES && fContinue; i++)
    {
        memset ((PVOID) &mmioInfo, 0, sizeof(mmioInfo));

        /* Open the file for reading */
		  sprintf (szString, "%s%s", gszPath, stWaveData[i].szFilename);
		  hmmio = (*stMMPMFn[MMIO_OPEN_FN].pFn) (szString, NULL, MMIO_READ);

        if (hmmio == (HMMIO) NULL)
        {
		  		sprintf (szString, "Sound file %s not found",
									   stWaveData[i].szFilename);
		      SoundError (szString);
            fContinue = FALSE;
        }
        else
        {
            /* Get the wavefile's header */
            rc = (*stMMPMFn[MMIO_GETHEADER_FN].pFn) (hmmio,
                                				    		 (PVOID) &mmAudioHeader,
                                					   	 sizeof (MMAUDIOHEADER),
                                						    (PLONG) &ulBytesRead,
                                						    (ULONG) NULL,
                                				   		 (ULONG) NULL);

            if (rc == MMIO_ERROR)
            {
		  			sprintf (szString, "Sound file %s has a bad header",
									      stWaveData[i].szFilename);
		      	SoundError (szString);
               fContinue = FALSE;
            }

            /* Save the size of this file's wave audio data */
            ulSize =
               mmAudioHeader.mmXWAVHeader.XWAVHeaderInfo.ulAudioLengthInBytes;

            if ((pulData = (PULONG) malloc (ulSize)) == (PULONG) NULL)
            {
                fContinue = FALSE;
            }
            else
            {
                /* Read the entire wavefile into pulData */
                rc = (*stMMPMFn[MMIO_READ_FN].pFn) (hmmio,
                               						    (PSZ) pulData,
                               						    ulSize);

                if (rc == MMIO_ERROR)
                {
		  				  sprintf (szString, "Sound file %s could not be read",
						 			   		  stWaveData[i].szFilename);
		      		  SoundError (szString);
                    fContinue = FALSE;
                }
            }

            /* Close file */
            rc = (*stMMPMFn[MMIO_CLOSE_FN].pFn) (hmmio, 0);

            stWaveData[i].pulData = pulData;
            stWaveData[i].ulSize  = ulSize;
        }
    }

    /* Open the waveaudio device */
    if (fContinue)
    {
        memset ((PVOID) &mciOpenParms, 0, sizeof(mciOpenParms));

        mciOpenParms.pszDeviceType  = "WAVEAUDIO";
        mciOpenParms.pszElementName = (PSZ) &stPlaylist;
        mciOpenParms.pszAlias       = "PLAYLIST";
        mciOpenParms.hwndCallback   = hwndDefClient;

        rc = (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (0,
                                   	      	     MCI_OPEN,
                                    	     	     ulOpenWaveFlags,
                                       	        (PVOID) &mciOpenParms,
                                          	     0);

        if (!rc)
        {
            /* Set up the sampling rate and bits */
            rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) 
										 ("SET PLAYLIST CHANNELS 1 WAIT",
                                szString,
                                sizeof (szString),
                                (ULONG) NULL,
                                0);
		  }

		  if (!rc)
		  {
            rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) 
										 ("SET PLAYLIST SAMPLESPERSEC 11025 WAIT",
                                szString,
                                sizeof (szString),
                                (ULONG) NULL,
                                0);
		  }

		  if (!rc)
		  {
            rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) 
										 ("SET PLAYLIST BITSPERSAMPLE 8 WAIT",
                                szString,
                                sizeof (szString),
                                (ULONG) NULL,
                                0);
	     }
   }

	if (rc)
	{
		/* If there's a problem with sound dont bother trying to open MIDI */
		gfMusicEnabled = FALSE;
		fContinue = FALSE;
	}

   if (fContinue)
   {
      /* Open the midi device */
      memset ((PVOID) &mciOpenParms, 0, sizeof(mciOpenParms));
      mciOpenParms.pszDeviceType  = "MIDI";
		sprintf (szString, "%s%s", gszPath, MIDI_FILE);
      mciOpenParms.pszElementName = (PSZ) szString;
      mciOpenParms.pszAlias       = "MUSIC";
      mciOpenParms.hwndCallback   = hwndDefClient;

      if ((*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (0,
                                 	      	   MCI_OPEN,
                                  	     	      ulOpenMidiFlags,
                                     	         (PVOID) &mciOpenParms,
                                        	      0))
		{
			gfMusicEnabled = FALSE;
		}
   }

	/* 
	** Acquire the device appropriately at this point.  We must do this
	**	since there may be only one device physically supported (eg ACPA cannot
	** support both sound and MIDI simultaneously
	*/
   if (gfMusicEnabled)
	{
      (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusMidiDeviceID,
                                           MCI_ACQUIREDEVICE,
                                           (ULONG) MCI_NOTIFY,
                                           (PVOID) &mciGenericParms,
                                           (USHORT) 0);
	}

	if (gfSoundOn)
	{
      mciGenericParms.hwndCallback = hwndDefClient;
      (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusWaveDeviceID,
                                           MCI_ACQUIREDEVICE,
                                           (ULONG) MCI_NOTIFY,
                                           (PVOID) &mciGenericParms,
                                           (USHORT) 0);
	}

   /* Set the pointer back to an arrow */
   WinSetPointer (HWND_DESKTOP,
                   WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));        /* Return the system pointer's handle.       */

	/* Post the event semaphore to tell the main thread to get going */
	DosPostEventSem (hSoundEventSem);
   DosResetEventSem (hSoundEventSem, &ulCount);

	/* Loop around playing sounds until the game is over */
   while (gAction != EXIT && fContinue)
   {
      /* Wait for a sound to be signalled */
	   DosWaitEventSem (hSoundEventSem, SEM_INDEFINITE_WAIT);
	   DosResetEventSem (hSoundEventSem, &ulCount);

      if (gsMusicAction == START)
      {
         gsMusicAction = NONE;
         gusMidiDeviceID = (USHORT) (*stMMPMFn[MCI_GETDEVICEID_FN].pFn) ("MUSIC");

         /* Send the callback to the main PM thread */
         mciPlayParms.hwndCallback = hwndDefClient;

         /* Rewind the music file */
         rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) ("SEEK MUSIC TO START WAIT",
                                              	  szString,
                                              	  sizeof (szString),
                                              	  (ULONG) NULL,
                                              	  0);
         /* Play the file */
         rc = (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusMidiDeviceID,
                                                   MCI_PLAY,
                                                   MCI_NOTIFY,
                                                   &mciPlayParms,
                                                   MIDI_ID);
      }
      else if (gsMusicAction == STOP)
      {
         /* Stop the music */
         gsMusicAction = NONE;
         rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) ("STOP MUSIC WAIT",
                                              	  szString,
                                              	  sizeof (szString),
                                              	  (ULONG) NULL,
                                              	  0);
      }
      else
      {      
         /* REWIND the playlist before play */
         rc = (*stMMPMFn[MCI_SENDSTRING_FN].pFn) ("SEEK PLAYLIST TO START",
                                              	  szString,
                                              	  sizeof (szString),
                                              	  (ULONG) NULL,
                                              	  0);

         stPlaylist[0].ulOp1 = (ULONG) stWaveData[gusSoundIdx].pulData;
         stPlaylist[0].ulOp2 = stWaveData[gusSoundIdx].ulSize;

         gsSoundPlaying = (SHORT) gusSoundIdx;

         gusWaveDeviceID = (USHORT) 
                           (*stMMPMFn[MCI_GETDEVICEID_FN].pFn) ("PLAYLIST");

         /* Send the callback to the main PM thread */
         mciPlayParms.hwndCallback = hwndDefClient;

         rc = (*stMMPMFn[MCI_SENDCOMMAND_FN].pFn) (gusWaveDeviceID,
                                                   MCI_PLAY,
                                                   MCI_NOTIFY,
                                                   &mciPlayParms,
                                                   PLAYLIST_ID);
      }
   }

   for (i = 0;  i < NUM_WAV_FILES;  i++)
   {
      if (stWaveData[i].pulData != NULL)
      {
         free ((PVOID) stWaveData[i].pulData);
      }
   }

   if (!fContinue)
   {
		gfSoundEnabled = FALSE;
		gfSoundOn = FALSE;
   }
}


/**************************************************************************
*
* FUNCTION NAME: MakeSound
*
* DESCRIPTION:
*
* INPUT PARAMETERS:
*     USHORT usSound - index into WAV array
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID MakeSound (USHORT usSound)
{
	INT rc;

   if (gfSoundEnabled && gfSoundOn)
	{
   	/* If there is some sound playing then we must have as good priority */
   	if (gsSoundPlaying == -1 || stWaveData[usSound].usPriority <= 
													stWaveData[gsSoundPlaying].usPriority)
   	{
      	/* Set the global sound variable and post the sound thread to play */
      	gusSoundIdx = usSound;
      	rc = DosPostEventSem (hSoundEventSem);
		}
   }
}


/**************************************************************************
*
* FUNCTION NAME: EnableSound
*
* DESCRIPTION:
*   Loads the MDM and MMIO DLLs (if present) and fetches the procedure 
*   addresses for all the mci and mmio functions called.
*
* INPUT PARAMETERS:
*     None.
*
* OUTPUT PARAMETERS:
*     TRUE if sound is enabled
*
**************************************************************************/
BOOL EnableSound (VOID)
{
   CHAR    szObjName[100];
	HMODULE hmodMDM;			/* Handle to MDM.DLL  */
	HMODULE hmodMMIO;			/* Handle to MMIO.DLL */
	INT     i;
	ULONG   rc;

   /* Sound is enabled unless MMPM/2 is not present or there's a problem */
   gfSoundEnabled = TRUE;

	/* Look for the existence of MDM.DLL - cant enable sound without it */
	if (rc = DosLoadModule (szObjName, sizeof(szObjName), "MDM", &hmodMDM))
	{
		gfSoundEnabled = FALSE;
		gfMusicEnabled = FALSE;
		gfSoundOn = FALSE;
	}
	else if (rc = DosLoadModule (szObjName, sizeof(szObjName), 
																		"MMIO", &hmodMMIO))
	{
		gfSoundEnabled = FALSE;
		gfMusicEnabled = FALSE;
		gfSoundOn = FALSE;
	}
	else
	{
		for (i = 0;  i < NUM_MMPM_FUNCTIONS;  i++)
		{
		  	if (rc = DosQueryProcAddr ((stMMPMFn[i].usDll == MDM_DLL) ? 
																  		     hmodMDM : hmodMMIO, 
												0, 
												stMMPMFn[i].szName, 
												&(stMMPMFn[i].pFn)))
			{
				SoundError ("Failed to access MMPM/2 function from DLL");
				gfSoundEnabled = FALSE;
      		gfMusicEnabled = FALSE;
				gfSoundOn = FALSE;
				break;				
			}
		}
	}

   /* Enable or disable the sound menu item */
   WinEnableMenuItem (hwndMenu, MENU_SOUND_ID, gfSoundEnabled);
   WinEnableMenuItem (hwndMenu, MENU_MUSIC_ID, gfMusicEnabled);

	return (gfSoundEnabled);
}

/**************************************************************************
*
* FUNCTION NAME: SoundError
*
* DESCRIPTION:
*   Displays an error message and disables sound.
*
* INPUT PARAMETERS:
*     PSZ pszError - error message string
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID SoundError (PSZ pszError)
{
	WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, pszError, "Sound Error", 0,
						MB_OK | MB_WARNING | MB_MOVEABLE);
	gfSoundEnabled = FALSE;
}
