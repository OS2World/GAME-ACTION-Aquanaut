#define INCL_32
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_ERRORS
#define INCL_WIN
#define INCL_PM
#define INCL_DOS
#define INCL_MMIO
#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINMENUS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#define _stdcall _cdecl
#include <string.h>
#include <time.h>
#include <ctype.h>

/* MMPM/2 Headers */
#define INCL_OS2MM
#include <os2me.h>
#include <sw.h>

/*********************
  Definitions
*********************/

/* General definitions */
#define STACK_SIZE        65536

/* Value does not matter since we have only one timer */
#define TIMER_ID			  1

/* If don't specify otherwise - play at "40 frames per second" */
#define DEFAULT_GAME_RATE 40

/* Width of a VGA screen */
#define VGA_WIDTH         640

/* The name of the INI file */
#define INI_FILE          "AQUANAUT.INI"
#define MIDI_FILE         "AQUANAUT.MID"
#define DEMO_FILE         "AQUANAUT.DEM"

/* Strings */
#define PAUSED_STRING     "== P A U S E D =="
#define GAME_OVER_STRING  "G A M E   O V E R"
#define VERSION_STRING    "1 01 "

/* Dimensions in pixels of the screen */
#define SCREEN_HEIGHT     300
#define SCREEN_WIDTH      400
#define STATUS_WIDTH			50
#define STATUS_HEIGHT      48
#define RADAR_WIDTH			(SCREEN_WIDTH - (2 * STATUS_WIDTH))
#define RADAR_SCREEN_WIDTH ((SCREEN_WIDTH * RADAR_WIDTH) / PLANET_SIZE)
#define SCREEN_MARKER      5
#define PLAY_HEIGHT       	(SCREEN_HEIGHT - STATUS_HEIGHT - 2)
#define SCREEN_MARKER_INDEX 8   /* Points into defgbls array */

/* Font size bases for appropriate scaling */ 
#define BASE_CHARCY       12
#define BASE_CHARCX        5

/* Defines the offset from gusWorld to the left edge of the radar */
#define RADAR_START			((PLANET_SIZE/2)-(SCREEN_WIDTH/2))

/* Size in world x coords of the planet */
#define PLANET_SIZE       3500

/* Define the limits of where the ship can move to */
#define SHIP_BORDER       60
#define SHIP_MIN_X        SHIP_BORDER
#define SHIP_MAX_X        (SCREEN_WIDTH - SHIP_BORDER)
#define SHIP_MIN_Y        0

/* Lander parameters */
#define ASCEND_SPEED       1    /* Also used by humans when grabbed */
#define DESCEND_SPEED      2
#define MAX_LANDER_DX      4    /* Max lander speed in x direction */

/* definitions for the landscape - xxxx should depend on SCREEN_HEIGHT etc */
#define MAX_UNDULATION    50 	  /* Max size of hill slope */
#define MIN_UNDULATION    10	  /* Min size of hill slope */								
#define LOWEST_LAND       10	  /* Lowest land point */
#define HIGHEST_LAND      150   /* Highest land point */
#define LOWER_HILL_LIMIT	5	  /* Num of high points before lowering starts */
										  /* Max number of land vertices */
#define MAX_VERTICES    (PLANET_SIZE / MIN_UNDULATION) 

#define HEADER_POINTS	3		  /* Number of vertices replicated at start of
											  land array */
#define MIN_SEPARATION  20      /* Min distance humans can be to each other */

/* Definitions for the numerical display */
#define DIGIT_GAP 3			/* The pixel gap between successive digits */

/* Maximum number of items that can exst at one time */
#define NUM_HUMANS		10
#define NUM_LANDERS     20
#define NUM_MUTANTS     20
#define NUM_LASERS 		 4
#define NUM_PODS         4
#define NUM_SWARMERS     (NUM_PODS * 4) 
#define NUM_RINGS        3
#define NUM_SKIMMERS     3
#define NUM_BOMBERS      3
#define NUM_BOMBS        8
#define NUM_BULLETS      4
#define NUM_TEXTS        5

/* Explosion parameters */
#define NUM_EXPLOSIONS   5
#define NUM_DEBRIS      10
#define EXPLOSION_LIFE  25
#define MAX_DISPERSION  15

/* Ring parameters */
#define RING_SPEED   12
#define RING_CHANGE   4    /* Number of frames before direction changes */

/* Laser size parameters */
#define LASER_MIN    20    /* Min length of bolt */
#define LASER_MAX    100   /* Max length of bolt */
#define LASER_INC		20    /* Increment in size */
#define LASER_SPEED  20    /* Travel speed */

/* Human parameters */
#define HUMAN_TERMVEL   15   /* Terminal velocity when falling to planet */
#define HUMAN_DEATHVEL 	 5   /* How fast human has to hit planet to die */
#define HUMAN_DY2       10   /* Determines rate of acceleration to ground */
#define HUMAN_YOFFSET   LOWEST_LAND
									  /* How far above planet surface human sits */

/* Ship parameters */
#define SHIP_DY          5   /* Rate of ascent and descent of ship */
#define SHIP_MAX_DX     15   /* Max rate of ship thrust */
#define SHIP_MOMENTUM    3   /* This factor provides momentum */
#define NO_CARRY        -1   /* Index value if nothing being carried */
#define HYPER_CHANCE     3   /* 1:3 chance of blowing up on reentry */
#define PROXIMITY_LIMIT  60  /* No alien should be started any closer */
#define SHIP_INVINCIBLE 600  /* Ship invincible for this period */
#define SHIP_WARNING     60  /* Flash the rubber for this period */

/* Mutant parameters */
#define MUTANT_CHANGE    6   /* Max num of frames before change of direc */

/* Pod parameters */
#define MAX_POD_DY        1
#define MAX_POD_DX        1
#define POD_CHANGE        20 /* max number of moves before direction change */
#define POD_FRAME_RATE    3  /* Frame animation rate */

/* Mermaid parameters */
#define MAX_MERMAID_DY       1
#define MAX_MERMAID_DX       1
#define MERMAID_CHANGE       20  /* max number of moves before direc change */
#define MERMAID_FRAME_RATE   3   /* Frame animation rate */
#define MERMAID_APPEAR_MIN   700
#define MERMAID_APPEAR_RANGE 500
#define MERMAID_PERSISTENCE  150	/* Time mermaid stays around */

/* Swarmer parameters */
#define SLOW_AT_BOTTOM	  (PLAY_HEIGHT/4)
#define SLOW_AT_TOP       (PLAY_HEIGHT - SLOW_AT_BOTTOM)
#define SWARMER_ACCEL     2
#define SWARMER_DX        4
#define MAX_SWARMER_DY    4
#define MAX_SWARMER_DELAY 30   /* Delay before swarmer accelerates */

/* Bomber parameters */
#define BOMBER_DX         3
#define MAX_BOMB_SPACING  20

/* Skimmer parameters */
#define SKIMMER_DX          20
#define SKIMMER_HOVER_TIME  30
#define SKIMMER_THRUST_TIME (PLANET_SIZE / (3 * SKIMMER_DX))
#define SKIMMER_HITS			 4

/* Bomb parameters */
#define BOMB_DURATION     100
#define BOMB_DY             2

/* Lander/mutant appearance parameters */
#define APPEAR_SPEED       4
#define APPEAR_CYCLES     20

/* Bullet parameters */
#define BULLET_DINC        8
#define BULLET_DELAY      10
#define BULLET_CHECK      20

/* Star parameters */
#define MAX_STAR_DY        2
#define MAX_STAR_DX        2
#define STAR_DX_CHANGE     3	  /* Change dx every 3 frames */

/* Scanner parameters */
#define SCANNER_DX         4
#define SCANNER_PERSIST		120  /* The number of radar pixels that persist */
#define SCANNER_OFF        0    /* The 3 scanner modes... */
#define SCANNER_ON         1
#define SCANNER_SWEEP      2

/* Game timing parameters */
#define SHIP_DEAD_DELAY   50   /* Time before new ship appears after dying */
#define END_OF_WAVE_DELAY 50   /* Time after last alien killed before wave
																	  end processing occurs */ 

/* 
** These numbers must correspond to the number of entries in the defgbls.c
** arrays which correspond to these.
*/
#define NUM_BMPS 			68
#define NUM_LASERCOLORS  6
#define NUM_LINES	 		16
#define NUM_STARS	 		12
#define NUM_WAVES        5
#define NUM_HISCORES    10

/* Miscellaneous */
#define NO_OLD_RADAR    -1   

/* Event definitions */
#define NUM_EVENTS      20
#define MAX_TIME        0xFFFFFFFF   

/* Time that text survives on screen */
#define TEXT_PERSISTENCE      75

/* Max length of name in hi-score entry */
#define HISCORE_NAME_LEN      12

/* Max length of name in register dialog */
#define REGISTER_NAME_LEN      30

/* Scores */
#define MUTANT_SCORE 			150
#define LANDER_SCORE 			100
#define POD_SCORE    			200
#define SWARMER_SCORE			150
#define SKIMMER_SCORE   		300
#define BOMBER_SCORE    		100
#define DROPPED_HUMAN_SCORE   250
#define PICKED_HUMAN_SCORE    250
#define SAVED_HUMAN_SCORE     100
#define RING_SCORE            200
#define MERMAID_SCORE			1000   /* + for rescue, - for kill */

#define BONUS_SCORE         10000    /* Bonus life and smart every... */

/* Sound indexes into SOUNDDATA array (defsound.c) */
#define SOUND_BONUS_LIFE      0
#define SOUND_SAVE_HUMAN      1
#define SOUND_HUMAN_PICKUP    2
#define SOUND_FIRE_SMART      3
#define SOUND_FORMATION       4
#define SOUND_FIRE_LASER      5
#define SOUND_EXPLOSION       6
#define SOUND_HYPERSPACE      7
#define SOUND_LASTONE_HIT     8
#define SOUND_POD_HIT         9
#define SOUND_HUMAN_HIT       10
#define SOUND_SONAR           11
#define SOUND_TITLE           12
#define SOUND_AWARD           13
#define SOUND_MERMAID_RESCUED 14
#define SOUND_MERMAID_KILLED  15
#define SOUND_MERMAID_APPEARS 16
#define SOUND_GAMEOVER        17
#define SOUND_RING_BURST      18
#define SOUND_GOT_RING        19

/* These indices point into the stMMPMFn array */
#define MMIO_OPEN_FN     	0	
#define MMIO_CLOSE_FN     	1
#define MMIO_READ_FN     	2
#define MMIO_GETHEADER_FN  3
#define MCI_SENDCOMMAND_FN 4
#define MCI_SENDSTRING_FN  5
#define MCI_GETDEVICEID_FN 6

#define NUM_MMPM_FUNCTIONS 7	/* The number of entries in the array */

/* Defines for the 2 MMPM DLLs */
#define MDM_DLL  0
#define MMIO_DLL 1

/* Message IDs received in MM_MCINOTIFY message */
#define PLAYLIST_ID 1
#define MIDI_ID     2

/* Bit values to be stored for the demo */
#define DEMO_UP			 		0x01
#define DEMO_DOWN					0x02
#define DEMO_THRUST				0x04
#define DEMO_FIRE					0x08
#define DEMO_SMART				0x10
#define DEMO_HYPER				0x20
#define DEMO_FACING_RIGHT		0x40
#define DEMO_SRAND_VAL			0x80
