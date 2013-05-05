/**********************
  Function prototypes
**********************/

MRESULT EXPENTRY DefWindowProc (HWND   hwnd,
                                ULONG  msg,
                                MPARAM mp1,
                                MPARAM mp2);
MRESULT EXPENTRY LogoWindowProc (HWND   hwnd,
                                 ULONG  msg,
                                 MPARAM mp1,
                                 MPARAM mp2);
MRESULT EXPENTRY SizeWindowProc (HWND hwnd,
             						   ULONG  msg,
             						   MPARAM mp1,
             						   MPARAM mp2);
MRESULT EXPENTRY ProcessUnregisteredDialog (HWND hwnd, 
                                            ULONG msg,
                                            MPARAM mp1, 
                                            MPARAM mp2);
VOID RefreshScreen (HPS hps);
VOID DrawScreen (HPS hps);
VOID DrawStatus (HPS hps);
VOID DrawLifeIcons (HPS hps);
VOID DrawSmartIcons (HPS hps);
VOID ScreenThread (VOID);
VOID LoadAndStretchBitmap (INT i);
VOID Initialize (VOID);
VOID ProcessEvent	(VOID);
VOID SetupWave (USHORT usWave);
VOID RestartAfterShipHit (VOID);
VOID InitHumans (BOOL fGameStart);
VOID InitShip (VOID);
VOID InitStars (BOOL fTitle);
VOID InitScanner (VOID);
VOID AddLanders (SHORT sNum);
VOID AddMutants (SHORT x, SHORT y, BOOL fNew);
VOID AddSkimmer (VOID);
VOID InitPods (SHORT iNumPods);
VOID InitRings (SHORT iNumRings);
VOID InitMermaid (VOID);
VOID InitBombers (SHORT iNumBombers);
VOID AddSwarmers (SHORT sNum, SHORT x, SHORT y, SHORT dy);
VOID PlantBombs (SHORT x, SHORT y);
VOID FireBullet (VOID);
VOID FireSmart (VOID);
VOID SetLanderTarget (SHORT i);
BOOL CheckProximity (SHORT x, SHORT y);
BOOL CreateExplosion (SHORT x, SHORT y, LONG lColor);
VOID MoveExplosions (VOID);
VOID MoveShip (VOID);
VOID MoveLanders (VOID);
VOID MoveMutants (VOID);
VOID MovePods (VOID);
VOID MoveRings (VOID);
VOID MoveMermaid (VOID);
VOID MoveBombers (VOID);
VOID MoveBombs (VOID);
VOID MoveBullets (VOID);
VOID MoveSwarmers (VOID);
VOID MoveSkimmers (VOID);
VOID MoveStars (BOOL fTitle, BOOL fNoPrevDraw);
VOID MoveHumans (VOID);
VOID MoveCarriedHuman (SHORT i);
VOID MoveLasers (VOID);
VOID MutateAll (BOOL fFlash);
VOID HandleHit (SHORT sObjIndex, BOOL fLaserHit);
VOID IncScore (LONG lInc);
VOID MoveLand (BOOL fForceDraw);
VOID DrawBitmap (HPS hps, SHORT x, SHORT y, LONG bitmap, BOOL fClear);
SHORT CreateLandscape (VOID);
VOID CalculateAltitudes (VOID);
VOID DisplayNum (HPS hps, LONG lNumber, SHORT x, SHORT y);
VOID PrintAt (HPS hps, SHORT x, SHORT y, LONG lColor, PCHAR szText, 
															BOOL fCenter, BOOL fErase);
VOID WaveOver (VOID);
VOID ReadIniFile (VOID);
VOID WriteIniFile (VOID);
MRESULT EXPENTRY HandleCommand (HWND hwnd, 
										  ULONG msg, 
										  MPARAM mp1, 
										  MPARAM mp2);
VOID AttractMode (VOID);
VOID CheckScore (VOID);
VOID InitHelp (VOID);
VOID HandleSounds (PVOID dummy);
VOID MakeSound (USHORT usSound);
BOOL EnableSound (VOID);
VOID PauseGame (VOID);
VOID ResumeGame (VOID);
VOID EnableMouse (BOOL fEnable);
VOID SaveDemoKeys (CHAR buffer[], PULONG pulFrame);
VOID SaveSeed (CHAR buffer[], PULONG pulFrame, LONG seed);
VOID UseDemoKeys (CHAR buffer[], PULONG pulFrame);
VOID UseSeed (CHAR buffer[], PULONG pulFrame);
VOID StoreDemoData (PCHAR pc, ULONG ulSize);
BOOL LoadDemoData (PCHAR pc, ULONG ulSize);
