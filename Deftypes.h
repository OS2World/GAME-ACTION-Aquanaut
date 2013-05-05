/*********************
  Typedefs
*********************/

typedef struct
{
   ULONG  bitmap; 	 /* Bitmap ID */
	BOOL   fScale;     /* TRUE if the bitmap size is scalable */
	USHORT cx, cy;     /* Bitmap original dimensions */
	USHORT sx, sy;     /* Bitmap scaled dimensions */
   USHORT dx, dy;     /* Bitmap offsets (used for animation) */
   USHORT sdx, sdy;   /* Scaled bitmap offsets (used for animation) */
} BITMAPDATA;

typedef struct
{
	SHORT bitmap;
	SHORT sDrawIdx;  	 /* index into data array of original object */
	SHORT sRadarIdx;   /* index into array of radar markers */
	SHORT x, y;
	BOOL  fErase;		 /* TRUE if erasing an old bitmap */
	BOOL  fLaserCheck; /* TRUE if requires check on laser hit */
	BOOL  fShipCheck;  /* TRUE if requires check on ship hit */
	BOOL  fDraw;		 /* TRUE unless object gets hit by laser etc */
} DRAWDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  fx, fy;
	SHORT  dx;
	SHORT  sFlame;
	SHORT  sMomentum;
	SHORT  sCarrying;
	SHORT  sRestartCnt;
	SHORT  sInvincible;
	BOOL   fActive;
	BOOL   fDrawFlame;
	BOOL	 fFacingRight;
	BOOL   fMoved;
   BOOL   keyUp, keyDown, keyTurn, keyThrust, keyFire, keyHyper, keySmart;
} SHIPDATA;

typedef enum
{
	LANDING, LANDED, GRABBED, CAPTURED, CARRIED, MUTATE, APPEARING
} STATE;

typedef enum
{
	DRAW_UNDEFINED, DRAW_LOGO, DRAW_SCREEN, DRAW_HISCORES,
	REFRESH_SCREEN, DRAW_SCORE, DRAW_ICONS, CLEAN_SCREEN,
	DRAW_WAVEOVER, DRAW_WAVEOVERNONELEFT, DRAW_GAMEOVER, 
	DRAW_GETNAME, DRAW_PAUSED
} DRAWSTATE;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx;
	SHORT  targetX;		/* X coord of targetted human */
	SHORT  targetIdx;		/* Index into human array of targetted human */
	SHORT  sAppearCnt;
	SHORT  sBitmap;
	STATE  eState;
	BOOL   fDraw;
	BOOL   fActive;
} LANDERDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  sChangeDir;
	SHORT  sAppearCnt;
	STATE  eState;
	BOOL   fDraw;
	BOOL   fActive;
} MUTANTDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  sChangeDir;
	SHORT  sFrame;
	SHORT  sChangeFrame;
	BOOL   fDraw;
	BOOL   fActive;
} PODDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  screeny;
	SHORT  bitmap;
	SHORT  dx, dy;
	SHORT  dxMaster;
   USHORT usChangeDir;
   USHORT usDirIdx;
	BOOL   fDraw;
	BOOL   fActive;
   BOOL   fShot;
} RINGDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  sChangeDir;
	SHORT  sFrame;
	SHORT  sChangeFrame;
	SHORT  sAppearance;
	BOOL   fDraw;
	BOOL   fActive;
} MERMAIDDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  dy2;
	SHORT  sAccelChange;
	BOOL   fDraw;
	BOOL   fActive;
} SWARMERDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  sBombTimer;
	SHORT  sBitmap;
	SHORT  sHits;
	BOOL   fDraw;
	BOOL   fActive;
} BOMBERDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  screenx;
	SHORT  dx, dy;
	SHORT  dx2;
	SHORT  sHoverTimer;
	SHORT  sThrustTimer;
	SHORT  sHits;
	BOOL   fDraw;
	BOOL   fActive;
} SKIMMERDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  x1;				/* End point of the laser bolt */
	SHORT  length;
	SHORT  dx;
	BOOL	 fActive;
} LASERDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  screenx;
	SHORT  sDuration;
	BOOL	 fActive;
	BOOL   fDraw;
} BOMBDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  screenx;
   SHORT  dx, dy;
	BOOL	 fActive;
	BOOL   fDraw;
} BULLETDATA;

typedef struct
{
   SHORT  x, y;
	SHORT  rx, ry;
	SHORT  dy, dy2;
	SHORT  screenx;
	STATE  eState;
	BOOL   fTargetted;
	BOOL   fDraw;
	BOOL   fActive;
} HUMANDATA;

typedef struct
{
	SHORT  x, y;
	SHORT  dx, dy;
	SHORT  changeDx;
} STARDATA;

typedef struct
{
	SHORT  x[NUM_DEBRIS], y[NUM_DEBRIS];
	SHORT  dx[NUM_DEBRIS], dy[NUM_DEBRIS];
	LONG   lColor;
	SHORT  sLife;
	BOOL   fActive;
} XPLODDATA;

typedef struct
{
	SHORT  x, y;
	LONG   bitmap;
	BOOL   fDraw;
} MARKERDATA;

typedef struct
{
   CHAR   szText[20];
	SHORT  x, y;
	SHORT  sDuration;
	LONG   lColor;
	BOOL   fActive;
	BOOL   fCenter;
} TEXTDATA;

typedef struct
{
	SHORT  rx, ry1, ry2;
	SHORT  sMode;
} SCANNERDATA;

typedef struct
{
	POINTL point1;
	POINTL point2;
	LONG   lColor;
} BOLTDATA;

typedef struct
{
   SHORT x, y;
} LANDDATA;

typedef struct
{
	POINTL  array1[50];
	POINTL  array2[50];
	LONG    lErasePts;
	LONG    lDrawPts;
	PPOINTL pEraseData;
	PPOINTL pDrawData;
   BOOL    fActive;
} LANDDRAWDATA;

typedef struct
{
	enum {LANDER_DROP, 
			SKIMMER_DROP, 
			WAVE_END}     eEvent;
	ULONG               ulTime;
} EVENT;

typedef enum
{
   NONE, START, EXIT, STOP, PAUSED
} ACTION;

typedef struct
{
	USHORT usNumPods;					/* Number of jellyfish */
	USHORT usNumBombers;				/* Number of U-Boats */
	USHORT usBulletDelay;			/* Delay between bullets */
	USHORT usBulletRand;				/* Bullet accuracy factor */
	USHORT usBulletMinX;          /* Min distance before will fire */
	USHORT usMutantDx;				/* Mutant DX speed */
	USHORT usMutantDy;				/* Mutant DY speed */
	USHORT usBomberHits;				/* Number of hits required to kill U-Boat */
	USHORT usBombSpacing;			/* Spacing between U-Boat depth charges */
	USHORT usNumRings;      		/* Number of rings */
	EVENT  stEvent[NUM_EVENTS];
} WAVEDATA;

typedef struct
{
   BOOL   fTurnThrust;
	USHORT usUpKey;
	USHORT usDownKey;
	USHORT usTurnKey;
	USHORT usThrustKey;
	USHORT usFireKey;
	USHORT usSmartKey;
	USHORT usHyperKey;
} KEYDEFS;

typedef struct
{
	USHORT usBitmap;
	USHORT dx, dy;
} ANIMFRAME;

typedef struct
{
	USHORT usChar;
	USHORT usVirtKey;
	BOOL   fVirtual;
	BOOL   fHit;
} KEYCODES;

typedef struct
{
	LONG  lScore;
	CHAR  szName[HISCORE_NAME_LEN+1];
	BOOL  fBullets;
	SHORT sShips;
	SHORT sSmarts;
	SHORT sWave;
	SHORT sHitRate;
	SHORT sAliens;
	CHAR  szDate[10];
} HISCORES;

typedef struct
{
	PSZ	 szName;
	USHORT usDll;
	PFN 	 pFn;
} MCIPROC;

typedef struct
{
   SHORT  dx;
   SHORT  dy;
   USHORT count;
   USHORT bitmapL;
   USHORT bitmapR;
} RINGMOVE;

typedef enum
{
	DEMO_NONE, DEMO_PLAY, DEMO_RECORD
} DEMOMODE;
