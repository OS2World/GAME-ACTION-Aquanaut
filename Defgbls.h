/*********************
  Global variables
*********************/

extern CHAR  gszPath[];
extern INT   gFramesPerSec;
extern ULONG ulDefFlags;
extern BITMAPDATA	stBitmap[];
extern LONG lLaserColor[];
extern POINTL statusPts[];
extern SHIPDATA stShip;
extern LANDERDATA stLander[];
extern MUTANTDATA stMutant[];
extern HUMANDATA stHuman[];
extern PODDATA stPod[];
extern RINGDATA stRing[];
extern MERMAIDDATA stMermaid;
extern SWARMERDATA stSwarmer[];
extern BOMBERDATA stBomber[];
extern SKIMMERDATA stSkimmer[];
extern BOMBDATA stBomb[];
extern BULLETDATA stBullet[];

extern STARDATA stStar[];
extern XPLODDATA stExplosion[];
extern LASERDATA stLaser[];
extern SCANNERDATA stScanner;
 
extern LANDDATA     land[];
extern LANDDRAWDATA stLandDraw;
extern SHORT        height[];

extern DRAWDATA 	stDrawItem[];
extern USHORT   	usDrawItems;

extern MARKERDATA  stMarkerItem[];
extern USHORT      usMarkerItems;
extern TEXTDATA    stText[];

extern BOLTDATA    stBoltItem[];
extern USHORT      usBoltItems;

extern USHORT      usNumHumans;
extern USHORT      gusNumBullets;
extern USHORT      gusBulletTimer;
extern USHORT      gusLaserColor;
extern BOOL        gfBullets;

extern PFNWP   pfnOldFrameProc;
extern HAB     hab;            
extern HPS     hps;
extern HWND    hwndDefFrame;
extern HWND    hwndDefClient;
extern HPS     hpsMemory[];
extern HDC     hdc[];
extern HBITMAP hbm[];
extern HEV     hEventSem;
extern HEV     hSoundEventSem;
extern HEV     hPaintEventSem;
extern ACTION  gAction;
extern DRAWSTATE gsDrawState;
extern BOOL      gfCleanScreen;
extern BOOL    gfRegistered;
extern BOOL    gsGameOn;
extern BOOL    gfSoundEnabled;
extern BOOL    gfSoundOn;
extern SHORT   gsSoundPlaying;
extern SHORT   gsFrameWidth;
extern SHORT   gsFrameHeight;
extern SHORT   gsMarker;
extern USHORT  gusSoundIdx;
extern LONG    glWindowX;
extern LONG    glWindowY;
extern SHORT   gsPosX;
extern SHORT   gsPosY;
extern BOOL    gfShowFrame;
extern SHORT   gsObjectSize;
extern USHORT  gusWaveDeviceID;
extern USHORT  gusMidiDeviceID;
extern BOOL	   gfPassed;
extern SHORT   gsCenterX, gsCenterY;
extern BOOL    gfMouseEnabled;
extern BOOL    gfMusicEnabled;
extern ACTION  gsMusicAction;
extern BOOL    gfBubbles;

extern HWND hwndTitlebar;
extern HWND hwndSysmenu;
extern HWND hwndMinmax;
extern HWND hwndMenu;

extern SHORT  gsWorld;
extern SHORT  gsWorldShift;
extern SHORT  gsRadar;
extern SHORT  gsScoreX;
extern SHORT  gsScoreY;

extern USHORT gusLandPoints; 

extern SHORT  gsLandIndex;
extern SHORT  gsCharcx;
extern SHORT  gsCharcy;

extern LONG   glScore;
extern SHORT  gsLives;
extern SHORT  gsSmarts;
extern SHORT  gsWave;
extern SHORT  gsAliens;
extern SHORT  gsShots;
extern SHORT  gsStartShips;
extern SHORT  gsStartSmarts;
extern ULONG  gulCount;

extern USHORT   gusBadGuys;
extern WAVEDATA WaveData[];
extern EVENT    stEvent[];
extern ULONG    gulWaveTimer;
extern USHORT   gusNextEvent;

extern SHORT    gsPaintInfo1, gsPaintInfo2;
extern CHAR     gszName[];
extern CHAR     gszRegName[];

extern KEYDEFS  gstKeyDefs;
extern DEMOMODE eDemoMode;
extern BOOL     gfDemoLoaded;
extern BOOL	    gfDemoKeyProtect;
extern KEYCODES gstKey;
extern HISCORES stHiscore[];
extern MCIPROC  stMMPMFn[];
extern RINGMOVE astRingMove[];
