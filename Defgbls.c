/***************************************************************************
*																									*
*  FILE  : DEFGBLS.C																  			*
*																									*
*	DATE  : March 94																			*
*																									*
*	NAME  : Paul Stanley																		*
*																									*
*	This file contains all global data structures used by the program.      *
*  (There are far too many of these!)          									   *
*																									*
***************************************************************************/

/*********************
  Global variables
*********************/

#include "def.h"
#include "defid.h"
#include "deftypes.h"

/* NUM_BMPS must equal number of entries in this array */
BITMAPDATA	stBitmap[] = { 
									{LANDERL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
							   	{MUTANTL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {SHIPL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {SHIPR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N0_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N1_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N2_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N3_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N4_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N5_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N6_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N7_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N8_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {N9_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
								   {HUMAN_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SCORE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 
									{FLAMER0_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMER1_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMER2_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMER3_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMEL0_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMEL1_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMEL2_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{FLAMEL3_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{JELLY1_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SWARMER_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SKIMMERL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{BOMBERL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{BULLET_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{BOMB_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SMART_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{HISCORE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{LIFE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{CLRWHITE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 	
									{CLRBLACK_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 	
									{CLRGREEN_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 	
									{CLRPINK_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 		
									{CLRYELLOW_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 	
									{CLRPALEGRAY_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{CLRRED_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0}, 		
									{CLRCYAN_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{CLRBLUE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{LOGO_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{ERASE_BMP, FALSE, 0, 0, 0, 0, 0, 0, 0, 0},
									{LANDERR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{LANDERC_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{MUTANTR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{BOMBERR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{JELLY2_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{JELLY3_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SKIMMERR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{BUBBLE_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{MERMAID1_BMP, TRUE, 0, 0, 0, 0, 9, 10, 0, 0},
									{MERMAID2_BMP, TRUE, 0, 0, 0, 0, 9, 1, 0, 0},
									{MERMAID3_BMP, TRUE, 0, 0, 0, 0, 9, 0, 0, 0},
									{MERMAID4_BMP, TRUE, 0, 0, 0, 0, 3, 2, 0, 0},
									{MERMAID5_BMP, TRUE, 0, 0, 0, 0, 0, 9, 0, 0},
									{RING_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{RINGN_BMP, TRUE, 0, 0, 0, 0, 16, 0, 0, 0},
									{RINGS_BMP, TRUE, 0, 0, 0, 0, 16, 0, 0, 0},
									{RINGE_BMP, TRUE, 0, 0, 0, 0, 0, 15, 0, 0},
									{RINGW_BMP, TRUE, 0, 0, 0, 0, 0, 15, 0, 0},
									{RINGNW_BMP, TRUE, 0, 0, 0, 0, 6, 6, 0, 0},
									{RINGNE_BMP, TRUE, 0, 0, 0, 0, 6, 6, 0, 0},
									{RINGSW_BMP, TRUE, 0, 0, 0, 0, 6, 6, 0, 0},
									{RINGSE_BMP, TRUE, 0, 0, 0, 0, 6, 6, 0, 0},
									{SHIPINVL_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0},
									{SHIPINVR_BMP, TRUE, 0, 0, 0, 0, 0, 0, 0, 0}
								 };

/* NUM_LASERCOLORS must equal number of entries in this array */
LONG lLaserColor[] = { CLR_WHITE,
						     CLR_PINK,     
						     CLR_GREEN,    
						     CLR_CYAN,    
						     CLR_YELLOW,   
						     CLR_PALEGRAY };

/* 
** The following array defines the lines comprising the status panel 
** NUM_LINES must equal number of entries in this array.  The radar 
** vertices must be the first 4 entries in the list, and the screen
** markers starting in position defined by SCREEN_MARKER_INDEX
*/
POINTL statusPts[] = {
							 {STATUS_WIDTH, PLAY_HEIGHT},
							 {STATUS_WIDTH, PLAY_HEIGHT+STATUS_HEIGHT},
							 {STATUS_WIDTH+RADAR_WIDTH, PLAY_HEIGHT+STATUS_HEIGHT},
							 {STATUS_WIDTH+RADAR_WIDTH, PLAY_HEIGHT},
							 {0, PLAY_HEIGHT}, 
							 {SCREEN_WIDTH, PLAY_HEIGHT},
							 {0, PLAY_HEIGHT+STATUS_HEIGHT},
							 {SCREEN_WIDTH, PLAY_HEIGHT+STATUS_HEIGHT},
							 {(SCREEN_WIDTH+RADAR_SCREEN_WIDTH)/2, PLAY_HEIGHT},
							 {(SCREEN_WIDTH+RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+SCREEN_MARKER},
							 {(SCREEN_WIDTH+RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+STATUS_HEIGHT},
							 {(SCREEN_WIDTH+RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+STATUS_HEIGHT-SCREEN_MARKER},
							 {(SCREEN_WIDTH-RADAR_SCREEN_WIDTH)/2, PLAY_HEIGHT},
							 {(SCREEN_WIDTH-RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+SCREEN_MARKER},
							 {(SCREEN_WIDTH-RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+STATUS_HEIGHT},
							 {(SCREEN_WIDTH-RADAR_SCREEN_WIDTH)/2, 
							 					 PLAY_HEIGHT+STATUS_HEIGHT-SCREEN_MARKER}
							};

SHIPDATA stShip;

LANDERDATA stLander[NUM_LANDERS];

MUTANTDATA stMutant[NUM_MUTANTS];

HUMANDATA stHuman[NUM_HUMANS];

PODDATA stPod[NUM_PODS];

RINGDATA stRing[NUM_RINGS];

MERMAIDDATA stMermaid;

SWARMERDATA stSwarmer[NUM_SWARMERS];

BOMBERDATA stBomber[NUM_BOMBERS];

SKIMMERDATA stSkimmer[NUM_SKIMMERS];

BOMBDATA stBomb[NUM_BOMBS];

BULLETDATA stBullet[NUM_BULLETS];

STARDATA stStar[NUM_STARS];

XPLODDATA stExplosion[NUM_EXPLOSIONS];
LASERDATA stLaser[NUM_LASERS];
SCANNERDATA stScanner = {0, 0, 0, SCANNER_SWEEP};
 
LANDDATA     land[MAX_VERTICES];
LANDDRAWDATA stLandDraw;
SHORT        height[PLANET_SIZE];

/* List of bitmaps to be drawn including erase blocks */
DRAWDATA 	stDrawItem[100];
USHORT   	usDrawItems;

/* List of radar and explosion markers to be drawn including erase markers */
MARKERDATA  stMarkerItem[300];
USHORT      usMarkerItems;

TEXTDATA    stText[NUM_TEXTS];

/* List of laser bolts to be drawn including erase lines */
BOLTDATA    stBoltItem[NUM_LASERS * 2 + 2];  /* +2 for the scanner lines */
USHORT      usBoltItems;

/* Number of items on screen */
USHORT      usNumHumans  = 0;
USHORT      gusNumBullets = 0;
USHORT      gusBulletTimer = 0;
USHORT      gusLaserColor = 0;

/* Are enemy bullets required or not */
BOOL        gfBullets = TRUE;

PFNWP   pfnOldFrameProc;	  /* Pointer to the old wind proc for the frame */
HAB     hab;                 /* Anchor block handle for window thread */
HPS     hps;
HWND    hwndDefFrame;
HWND    hwndDefClient;
HPS     hpsMemory[NUM_BMPS]; /* PS handle to device memory */
HDC     hdc[NUM_BMPS];
HBITMAP hbm[NUM_BMPS];
HEV     hEventSem;
HEV     hSoundEventSem;
HEV     hPaintEventSem;
ACTION  gAction = NONE;
BOOL    gfRegistered = FALSE;
BOOL    gsGameOn;
BOOL    gfSoundEnabled;
BOOL    gfSoundOn = TRUE;
USHORT  gusSoundIdx;
SHORT   gsSoundPlaying;
SHORT   gsFrameWidth;
SHORT   gsFrameHeight;
SHORT   gsMarker;      		  /* Marker index assigned in DrawMarker macro */
LONG    glWindowX = -1;
LONG    glWindowY = -1;
SHORT   gsPosX = -1;
SHORT   gsPosY = -1;
BOOL    gfShowFrame = TRUE;
DRAWSTATE gsDrawState = CLEAN_SCREEN;
BOOL      gfFromPaint = FALSE;
SHORT   gsObjectSize = 6;
USHORT  gusWaveDeviceID;
USHORT  gusMidiDeviceID;
BOOL	  gfPassed = FALSE;
SHORT   gsCenterX, gsCenterY;
BOOL    gfMouseEnabled = FALSE;
BOOL    gfMusicEnabled = TRUE;
ACTION  gsMusicAction = NONE;
BOOL    gfBubbles = TRUE;

/* Handles to frame controls */
HWND hwndTitlebar;
HWND hwndSysmenu;
HWND hwndMinmax;
HWND hwndMenu;

/* 
** World position of the left edge of the screen window - the planet extends
** from 0 to PLANET_SIZE.
*/
SHORT  gsWorld = 0;
SHORT  gsWorldShift = 0;
SHORT  gsRadar = 0;

SHORT  gsScoreX;
SHORT  gsScoreY;

USHORT gusLandPoints;   	 /* The number of vertices in the land array */

SHORT  gsLandIndex = HEADER_POINTS; 	/* Current position in land array */

LONG   glScore;
SHORT  gsLives;
SHORT  gsSmarts;
SHORT  gsWave;
SHORT  gsAliens;
SHORT  gsShots;

SHORT  gsStartShips = 3;
SHORT  gsStartSmarts = 3;
ULONG  gulCount;
USHORT gusBadGuys;
SHORT  gsCharcx;
SHORT  gsCharcy;

SHORT  gsPaintInfo1, gsPaintInfo2;
CHAR   gszName[HISCORE_NAME_LEN+5];
CHAR   gszRegName[REGISTER_NAME_LEN];

/*
** The events that define each wave.  These get progressively tougher!
** Note that the define NUM_WAVES must be set to correspond to the number
** of entries here
*/
WAVEDATA WaveData[] = {
                       /* Wave 1 */
                       {1,                  /* Number of pods */
							   1,                  /* Number of bombers */
								80,					  /* Bullet delay */
								400,					  /* Bullet randomness */
								250,					  /* Bullet closeness */
								4, 3,					  /* Mutant speed */
								5, 40,				  /* U-Boat hits and bomb spacing */
                        0,                  /* Number of rings */
							   {{LANDER_DROP, 10},
							    {LANDER_DROP, 500}, 
			  				    {LANDER_DROP, 1000}, 
				 			    {LANDER_DROP, 1500},
							    {SKIMMER_DROP, 2000},
							    {SKIMMER_DROP, 2500},
							    {WAVE_END, MAX_TIME}}},

							  /* Wave 2 */
                       {2,
							   2,
								40,
								200,
								175,
								4, 3,
								7, 33,
                        1,                  /* Number of rings */
							   {{LANDER_DROP, 10},
							    {LANDER_DROP, 400}, 
			  				    {LANDER_DROP, 800}, 
				 			    {LANDER_DROP, 1200},
							    {SKIMMER_DROP, 1600},
							    {SKIMMER_DROP, 2000},
							    {WAVE_END, MAX_TIME}}},

							  /* Wave 3 */
                       {3,
							   3,
								20,
								100,
								125,
								5, 4,
								9, 25,
                        1,
							   {{LANDER_DROP, 10},
                         {LANDER_DROP, 11},
							    {LANDER_DROP, 400}, 
			  				    {LANDER_DROP, 800}, 
				 			    {LANDER_DROP, 1200},
							    {SKIMMER_DROP, 1600},
							    {SKIMMER_DROP, 1800},
							    {WAVE_END, MAX_TIME}}},

							  /* Wave 4 */
                       {3,
							   3,
								15,
								50,
								75,
								6, 4,
								10, 20,
                        2,
								{{LANDER_DROP, 10},
                         {LANDER_DROP, 11},
							    {LANDER_DROP, 400}, 
							    {LANDER_DROP, 401}, 
			  				    {LANDER_DROP, 800}, 
			  				    {LANDER_DROP, 801}, 
							    {SKIMMER_DROP, 1000},
							    {SKIMMER_DROP, 1500},
							    {WAVE_END, MAX_TIME}}},

							  /* Wave 5 */
                       {3,
							   3,
								10,
								25,
								25,
								6, 4,
								10, 20,
                        2,
								{{LANDER_DROP, 10},
                         {LANDER_DROP, 11},
							    {LANDER_DROP, 300}, 
							    {LANDER_DROP, 301}, 
			  				    {LANDER_DROP, 600}, 
			  				    {LANDER_DROP, 601}, 
							    {SKIMMER_DROP, 900},
							    {SKIMMER_DROP, 901},
							    {WAVE_END, MAX_TIME}}}
						  };

EVENT  stEvent[NUM_EVENTS];
ULONG	 gulWaveTimer;
USHORT gusNextEvent;

CHAR  gszPath[80];       /* Path of executable used for ini file */
INT   gFramesPerSec = DEFAULT_GAME_RATE;

/* Default scancodes - see scancode.h to work out what keys they are */
KEYDEFS gstKeyDefs = {TRUE, 30, 44, 57, 51, 52, 53, 42};
KEYCODES gstKey;

DEMOMODE eDemoMode;
BOOL     gfDemoLoaded = FALSE;
BOOL		gfDemoKeyProtect = FALSE;

/* Hi score table */
HISCORES stHiscore[] = 
						{
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"},
                     {0, "---", 0, 0, 0, 0, 0, 0, "--/--/--"}
					   };

/* This array holds the addresses of the specified procedures */
MCIPROC stMMPMFn[] = 
{
   {"mmioOpen",       MMIO_DLL, NULL},
   {"mmioClose",      MMIO_DLL, NULL},
   {"mmioRead",       MMIO_DLL, NULL},
   {"mmioGetHeader",  MMIO_DLL, NULL},
   {"mciSendCommand", MDM_DLL,  NULL},
   {"mciSendString",  MDM_DLL,  NULL},
   {"mciGetDeviceID", MDM_DLL,  NULL}
};

/* 
** Array of direction changes for the ring once it has been burst.
** These moves trace out a figure eight which heads back towards the
** ship that killed the ring.
*/
RINGMOVE astRingMove[] = {{RING_SPEED, 0, RING_CHANGE*3, RINGW, RINGE},
                          {RING_SPEED, -RING_SPEED, RING_CHANGE, RINGSW, RINGSE},
                          {RING_SPEED, 0, RING_CHANGE, RINGW, RINGE},
                          {RING_SPEED, RING_SPEED, RING_CHANGE, RINGNW, RINGNE},
                          {0, RING_SPEED, RING_CHANGE, RINGN, RINGN},
                          {-RING_SPEED, RING_SPEED, RING_CHANGE, RINGNE, RINGNW},
                          {-RING_SPEED, 0, RING_CHANGE, RINGE, RINGW},
                          {-RING_SPEED, -RING_SPEED, RING_CHANGE, RINGSE, RINGSW},
                          {-RING_SPEED, 0, RING_CHANGE, RINGE, RINGW},
                          {-RING_SPEED, -RING_SPEED, RING_CHANGE, RINGSE, RINGSW},
                          {-RING_SPEED, 0, RING_CHANGE, RINGE, RINGW},
                          {-RING_SPEED, RING_SPEED, RING_CHANGE, RINGNE, RINGNW},
                          {0, RING_SPEED, RING_CHANGE, RINGN, RINGN},
                          {-RING_SPEED, RING_SPEED, RING_CHANGE, RINGNE, RINGNW},
                          {-RING_SPEED, 0, RING_CHANGE, RINGE, RINGW},
                          {-RING_SPEED, -RING_SPEED, RING_CHANGE, RINGSE, RINGSW},
                          {0, -RING_SPEED, 0xFFFF, RINGS, RINGS}};
