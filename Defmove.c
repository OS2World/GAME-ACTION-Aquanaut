/***************************************************************************
*																									*
*  FILE  : DEFMOVE.C																			*
*																									*
*	DATE  : March 94																			*
*																									*
*	NAME  : Paul Stanley																		*
*																									*
*	This file contains the routines which move objects (aliens, ship etc)   *
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

/**************************************************************************
*
* FUNCTION NAME: MoveShip
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
VOID MoveShip (VOID)
{
	INT   i, j;
	SHORT sOldRadar;
	SHORT oldX, oldY;
	LONG  bitmap;
	CHAR  szString[5];

	oldX = stShip.x;
	oldY = stShip.y;
	stShip.fMoved = FALSE;

	/* Process the up and down key presses */
	if (stShip.keyUp && stShip.y < (PLAY_HEIGHT - SHIP_DY - stBitmap[SHIPL].cy))
	{
	   stShip.y += SHIP_DY;
		stShip.fMoved = TRUE;
	}
	else if (stShip.keyDown && stShip.y > SHIP_MIN_Y)
	{
	   stShip.y -= SHIP_DY;
		stShip.fMoved = TRUE;
	}

	/* Change direction if the turn key is pressed */
	if (stShip.keyTurn)
	{
		stShip.keyTurn = FALSE;
		stShip.fFacingRight = !stShip.fFacingRight;
	}

	/*
	** Only look at the thrust key if the ship is active.  If it's just
	** recently died we want the land to keep on scrolling, so use the
	** normal deceleration code while the explosion occurs
	*/
	if (stShip.fActive && stShip.keyThrust)
	{
	   if (stShip.fFacingRight)
		{
		   if (stShip.dx < SHIP_MAX_DX)
			   stShip.dx++;
		}
		else
		{
		   if (stShip.dx > -SHIP_MAX_DX)
			   stShip.dx--;
		}

		/* Note that this assumes that there are 4 frames of animation */
		stShip.sFlame = (++stShip.sFlame) % 4;
	}
	else
	{
		/* Decelerate slowly to add momentum effect */
		if ((++stShip.sMomentum % SHIP_MOMENTUM) == 0)
		{
	   	if (stShip.dx > 0)
		   	stShip.dx--;
			else if (stShip.dx < 0)
		   	stShip.dx++;
		}
	}

	gsWorldShift = stShip.dx;

	/* Only look at the rest of the code if the ship is active */
	if (stShip.fActive)
	{
      /* If the ship is invincible decrement the count */
      if (stShip.sInvincible)
      {
         stShip.sInvincible--;
      }

		/* Move the ship if it is not at the frame extremities */
		if (stShip.fFacingRight && stShip.x > SHIP_MIN_X)
		{
			stShip.x -= 10;
			gsWorldShift += 10;
			stShip.fMoved = TRUE;
		}
		else if (!stShip.fFacingRight && stShip.x <
										             (SHIP_MAX_X - stBitmap[SHIPR].cx))
		{
			stShip.x += 10;
			gsWorldShift -= 10;
			stShip.fMoved = TRUE;
		}
		
		/* Erase the old marker */
		DrawMarker (stShip.rx, stShip.ry, CLRBLACK, TRUE);
		sOldRadar = gsMarker;
		
		/* Erase the old ship bitmap */
		if (stShip.fMoved)
		{
		   Draw (oldX, oldY, TRUE, SHIPR, FALSE, FALSE, 0, sOldRadar);
		}
		
		/* Erase the old thrust flame bitmap */
		if (stShip.fDrawFlame)
		{
		   Draw (stShip.fx, stShip.fy, TRUE, FLAMER, FALSE, FALSE, 0, 0);
		}

		/*
		** Check for hits with objects by looking for any correspondence in the
		** draw list - only look at items which are flagged to be checked
		*/
		for (i = 0;  i < usDrawItems;  i++)
		{
		   if (stDrawItem[i].fShipCheck && stDrawItem[i].fDraw)
		   {
		  	   /* First check the X correspondence */
		  	   if (stShip.x < 
							   stDrawItem[i].x + stBitmap[stDrawItem[i].bitmap].cx &&
				    stShip.x + stBitmap[SHIPL].cx > stDrawItem[i].x)
		  	   {
		  		   /* Now check the Y values in the same way */
		  	   	if (stShip.y < 
								stDrawItem[i].y + stBitmap[stDrawItem[i].bitmap].cy &&
				   	 stShip.y + stBitmap[SHIPL].cy > stDrawItem[i].y)
		  			{
						/* If we've hit a human, then it's falling so pick it up */
						switch (stDrawItem[i].bitmap)
						{
						case HUMAN:
							/* We can only carry one human: any more continue to fall */
							if (stShip.sCarrying == NO_CARRY)
							{
								j = stDrawItem[i].sDrawIdx;
								stShip.sCarrying = j;
								stHuman[j].eState = CARRIED;
								IncScore (PICKED_HUMAN_SCORE);
	                     sprintf (szString, "%d", PICKED_HUMAN_SCORE);
	                     DisplayText (szString, 
								             (SCREENX(stHuman[j].x) - 2 * gsCharcx), 
		                               (stHuman[j].y + stBitmap[HUMAN].cy), 
												 CLR_GREEN, FALSE, FALSE);
                        MakeSound (SOUND_SAVE_HUMAN);
							}
						   break;

						case MERMAID1:
						case MERMAID2:
						case MERMAID3:
						case MERMAID4:
						case MERMAID5:
						   /* Hooray!  Picked up a mermaid */
      					MakeSound (SOUND_MERMAID_RESCUED);
							gsSmarts++;
							gsLives++;
							IncScore (MERMAID_SCORE);
	                  sprintf (szString, "%d", MERMAID_SCORE);
	                  DisplayText (szString, 
								          (SCREENX(stMermaid.x) - 2 * gsCharcx), 
		                            stMermaid.y,
											 CLR_WHITE, FALSE, FALSE);
							PAINT (DRAW_ICONS);

                     /* Make her vanish next call to MoveMermaid */
                     stMermaid.sAppearance = 1;
                     break;

                  case RINGN:
                  case RINGS:
                  case RINGE:
                  case RINGW:
                  case RINGSW:
                  case RINGSE:
                  case RINGNW:
                  case RINGNE:
                     /* 
                     ** Picked up ring - go invincible for a while and make
                     ** the ring disappear
                     */
                     stShip.sInvincible = SHIP_INVINCIBLE;
                     MakeSound (SOUND_GOT_RING);
			            stRing[stDrawItem[i].sDrawIdx].fActive = FALSE;
			            stDrawItem[i].fDraw = FALSE;
			            stMarkerItem[stDrawItem[i].sRadarIdx].fDraw = FALSE;

							/* See if there are any baddies left */
   						if (--gusBadGuys == 0)
							{
								/* Change the next event to be end of wave */
								stEvent[gusNextEvent].eEvent = WAVE_END;
								stEvent[gusNextEvent].ulTime = gulWaveTimer + 
																				END_OF_WAVE_DELAY;
   						   MakeSound (SOUND_LASTONE_HIT);
							}
                     break;

						default:
		  					/* Blow up the object that we collided with */
		  					HandleHit (i, FALSE);

							/* If we were carrying then release the human */
							if (stShip.sCarrying != NO_CARRY)
							{
								stHuman[stShip.sCarrying].eState = LANDING;
							}

							/* May not have erased the old bitmap so make sure */
		   				Draw (oldX, oldY, TRUE, SHIPR, FALSE, FALSE, 0, 0);

                     if (stShip.sInvincible == 0)
                     {
							   /*
							   ** Remove the ship and create an impressive explosion by
							   ** using up all explosions that are in the array
							   */
							   stShip.fActive = FALSE;
							   while (CreateExplosion (
                                    stShip.x + (stBitmap[SHIPR].cx / 2),
								    			stShip.y + (stBitmap[SHIPR].cy / 2),
								            CLRWHITE))
								   ;

							   /* Set the delay until the game restarts with new ship */
							   stShip.sRestartCnt = SHIP_DEAD_DELAY;
                        MakeSound (SOUND_EXPLOSION);
                     }
						}
		  				break;
		  			}
		  		}
			}
		}
	
		/* 
		** Reposition the carried human if there is one - note this is called
		** before redrawing the ship to prevent the erasing of the human
		** overwriting the ship's bitmap (which would occur if ship moves down)
		*/
		if (stShip.sCarrying != NO_CARRY)
		{
			MoveCarriedHuman (stShip.sCarrying);
		}

		/* See if the hyperspace key was pressed */
		if (stShip.keyHyper)
		{
			stShip.keyHyper = FALSE;
			gsWorld = rand() % PLANET_SIZE;

			/* This is a bit of a fiddle to ensure the land gets redrawn */
			if (gsWorldShift == 0)
			{
			   gsWorldShift = stShip.fFacingRight ? 1 : -1;
			}

			/* May possibly blow up on reentry */
			if (!(rand() % HYPER_CHANCE))
			{
				/* May not have erased the old bitmap so make sure */
		   	Draw (oldX, oldY, TRUE, SHIPR, FALSE, FALSE, 0, 0);

				stShip.fActive = FALSE;
				while (CreateExplosion(stShip.x + (stBitmap[SHIPR].cx/2),
					 						  stShip.y + (stBitmap[SHIPR].cy/2),
				 	                    CLRWHITE))
				 	;

				/* Set the delay until the game restarts with new ship */
				stShip.sRestartCnt = SHIP_DEAD_DELAY;
            MakeSound (SOUND_EXPLOSION);
			}
         else
         {
            MakeSound (SOUND_HYPERSPACE);
         }
		}

		/* This check required because may just have been killed */
		if (stShip.fActive)
		{
   		/* Draw the new marker */
			stShip.rx = RADARX ((stShip.x + gsWorld) % PLANET_SIZE);
			stShip.ry = RADARY (stShip.y);
			DrawMarker (stShip.rx, stShip.ry, CLRYELLOW, FALSE);
			
			/* Draw the new ship bitmap */
         if ((stShip.sInvincible > SHIP_WARNING) || (stShip.sInvincible & 1))
         {
            Draw (stShip.x, stShip.y, FALSE,
	   							  stShip.fFacingRight ? SHIPINVR : SHIPINVL,
			  						  FALSE, FALSE, 0, gsMarker);
         }
         else
         {
			   Draw (stShip.x, stShip.y, FALSE,
				   							  stShip.fFacingRight ? SHIPR : SHIPL,
					   						  FALSE, FALSE, 0, gsMarker);
         }
			
   		/* Draw the new thrust flame bitmap */
			if (stShip.keyThrust)
			{
			   if (stShip.fFacingRight)
				{
			      bitmap = FLAMER + stShip.sFlame;
					stShip.fx = stShip.x - stBitmap[FLAMER].cx;
				}
				else
				{
			      bitmap = FLAMEL + stShip.sFlame;
					stShip.fx = stShip.x + stBitmap[SHIPL].cx;
				}
			   stShip.fy = stShip.y + (stBitmap[SHIPL].cy * 3 / 8);
				stShip.fDrawFlame = TRUE;
			   Draw (stShip.fx, stShip.fy, FALSE, bitmap, FALSE, FALSE, 0, 0);
			}
			else
			{
				stShip.fDrawFlame = FALSE;
			}
		
			/* See if the smart bomb key was pressed */
			if (stShip.keySmart)
			{
				stShip.keySmart = FALSE;
				if (gsSmarts > 0)
				{
					FireSmart ();
				}
			}

			/* Finally see if the 'fire' key was pressed and start a laser if so */
			if (stShip.keyFire)
			{
				stShip.keyFire = FALSE;
			
				/* Try and find an inactive laser entry */
				for (i = 0;  i < NUM_LASERS;  i++)
				{
					if (!stLaser[i].fActive)
						break;
				}
			
				if (i < NUM_LASERS)
				{
					stLaser[i].fActive = TRUE;
					stLaser[i].length  = 0;
					stLaser[i].y = stShip.y + (stBitmap[SHIPR].cy * 3 / 8);
					if (stShip.fFacingRight)
					{
						stLaser[i].x = stShip.x + stBitmap[SHIPR].cx + 1;
						stLaser[i].dx = 1;
					}
					else
					{
						stLaser[i].x = stShip.x - 1;
						stLaser[i].dx = -1;
					}

					/* Increment the shot tally */
					gsShots++; 
               MakeSound (SOUND_FIRE_LASER);
				}
			}
		}
	}
	else
	{
		if (--stShip.sRestartCnt == 0)
		{
			RestartAfterShipHit ();
		}
	}

	gfDemoKeyProtect = FALSE;
}


/**************************************************************************
*
* FUNCTION NAME: MoveStars
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
VOID MoveStars (BOOL fTitle, BOOL fNoPrevDraw)
{
   INT         i;
	static BOOL fPrevDraw = FALSE;  /* TRUE if we drew bubbles last time */
	BOOL        fThisDraw;          /* TRUE if we drew this time */

	/* If we;re calling this just to reset the prevdraw flag... */
	if (fNoPrevDraw)
	{
	   fPrevDraw = FALSE;
		return;
	}

	for (i = 0;  i < NUM_STARS;  i++)
	{
	   if (fPrevDraw)
		{
	   	/* Erase the old star position */
			Draw (stStar[i].x, stStar[i].y, TRUE, BUBBLE, FALSE, FALSE, 0, 0);
		}

		if (gfBubbles || fTitle)
		{
			/* Adjust the star's x position */
			if ((stStar[i].x = (stStar[i].x + stStar[i].dx - gsWorldShift / 2) 
                                                         % SCREEN_WIDTH) < 0)
			{
				stStar[i].x += SCREEN_WIDTH;
			}
			
      	/* Change the sway of the bubble */
			if (--stStar[i].changeDx == 0)
			{
      		stStar[i].dx = -stStar[i].dx;
				stStar[i].changeDx = STAR_DX_CHANGE;
			}
			
			/* Adjust the star's y position */
			stStar[i].y += stStar[i].dy;
			if ((fTitle && stStar[i].y >= SCREEN_HEIGHT - stBitmap[BUBBLE].cy) ||
			    (!fTitle && stStar[i].y >= PLAY_HEIGHT - stBitmap[BUBBLE].cy))
			{
			   /* Start again at the bottom of the screen and new X posn */
				stStar[i].y = LOWEST_LAND;
      	   stStar[i].dy = (rand() % MAX_STAR_DY) + 1;
				if (fTitle)
				{
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
			}
			
			/* Draw the star in the new position */
			Draw (stStar[i].x, stStar[i].y, FALSE, BUBBLE, FALSE, FALSE, 0, 0);
			fThisDraw = TRUE;
		}
	}		

	fPrevDraw = fThisDraw;
}

/**************************************************************************
*
* FUNCTION NAME: CreateExplosion
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
BOOL CreateExplosion (SHORT x, SHORT y, LONG lColor)
{
   INT i, j;

	/* Find an unused explosion */
	for (i = 0;  i < NUM_EXPLOSIONS;  i++)
	{
	   if (!stExplosion[i].fActive)
		{
	      stExplosion[i].fActive = TRUE;
	      stExplosion[i].sLife = EXPLOSION_LIFE;
			stExplosion[i].lColor = lColor;

	   	for (j = 0;  j < NUM_DEBRIS;  j++)
			{
			   /* Set up the initial position */	
				stExplosion[i].x[j] = x;
				stExplosion[i].y[j] = y;

				/* Draw the initial mark */
				DrawMarker (stExplosion[i].x[j],
								stExplosion[i].y[j],
								stExplosion[i].lColor, FALSE);

			   /*
				** Generate random directions for the debris parts - note that
				**	the dy value is positive meaning that all debris will fly up
				** initially, but gravity will bring it down!
				*/
				stExplosion[i].dx[j] = (rand() % MAX_DISPERSION) -
																			(MAX_DISPERSION/2);
				stExplosion[i].dy[j] = rand() % MAX_DISPERSION;
			}

			break;
		}
	}

	/* Return TRUE if an explosion was set up, otherwise FALSE */
	return (i < NUM_EXPLOSIONS);
}

/**************************************************************************
*
* FUNCTION NAME: MoveExplosions
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
VOID MoveExplosions (VOID)
{
   INT i, j;

	for (i = 0;  i < NUM_EXPLOSIONS;  i++)
	{
	   if (stExplosion[i].fActive)
		{
			/*
			**If the life has expired then make it inactive but still need to
			** erase the old debris so continue into 'j' loop
			*/
			if (--stExplosion[i].sLife == 0)
			{
				stExplosion[i].fActive = FALSE;
			}

	   	for (j = 0;  j < NUM_DEBRIS;  j++)
			{
				/* Erase the old position */
				if (stExplosion[i].y[j] < (PLAY_HEIGHT - stBitmap[CLRBLUE].cy))
				{
					DrawMarker (stExplosion[i].x[j],
									stExplosion[i].y[j],
									CLRBLUE, TRUE);
				}

				if (stExplosion[i].fActive)
				{
					/* Move the debris - note the --dy for gravity effect */
					stExplosion[i].x[j] += stExplosion[i].dx[j];
					stExplosion[i].y[j] += --stExplosion[i].dy[j];

					/* Draw the new position */
					if (stExplosion[i].y[j] < (PLAY_HEIGHT - stBitmap[CLRBLUE].cy))
					{
						DrawMarker (stExplosion[i].x[j],
										stExplosion[i].y[j],
										stExplosion[i].lColor, TRUE);
					}
				}
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveLanders
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
VOID MoveLanders (VOID)
{
   INT i, j;

	for (i = 0;  i < NUM_LANDERS;  i++)
	{
		if (stLander[i].fActive)
		{
		   /* Handle the situation where the lander is just appearing */
		   if (stLander[i].eState == APPEARING)
			{
			   /* If this lander is on screen, then handle its appearance */
				if (stLander[i].fDraw)
				{
               if (gsSoundPlaying != SOUND_FORMATION)
               {
                  MakeSound (SOUND_FORMATION);
               }

					/* Erase the old appearance marker blobs */
				   DrawAppearance (stLander[i].screenx, 
										 stLander[i].y,
										 stLander[i].sAppearCnt, 
										 CLRBLUE);

					if (!(stLander[i].sAppearCnt -= APPEAR_SPEED)) 
					{
						/* The lander has appeared! */
						stLander[i].eState = LANDING;
						stLander[i].fDraw = FALSE;

						/*
						** Need to reposition the lander, since its original X 
						** value has not been updated and the land may have moved
						*/
						stLander[i].x = 
										(gsWorld + stLander[i].screenx) % PLANET_SIZE;
					}
					else
					{
					   /* Draw the new appearance marker blobs */
				      DrawAppearance (stLander[i].screenx, 
						   				 stLander[i].y,
							   			 stLander[i].sAppearCnt, 
								   		 CLRGREEN);
					}
				}
				else if (!(stLander[i].sAppearCnt -= APPEAR_SPEED)) 
				{
					/* The lander has appeared! */
					stLander[i].eState = LANDING;
				}
			}
			else
			{
				/* Erase the old radar position */
				DrawMarker (stLander[i].rx, stLander[i].ry, CLRBLACK, TRUE);

	         /* Erase the old bitmap if it was on screen */
				if (stLander[i].fDraw)
				{
				   Draw (stLander[i].screenx, stLander[i].y, TRUE, LANDERL,
											  		   FALSE, FALSE, 0, gsMarker);
				}

				/* If all the landers mutated because all the humans died... */
				if (stLander[i].eState == MUTATE)
				{
					stLander[i].fActive = FALSE;
   				if (--gusBadGuys == 0)
					{
						/* Change the next event to be end of wave */
						stEvent[gusNextEvent].eEvent = WAVE_END;
						stEvent[gusNextEvent].ulTime = gulWaveTimer + 
						                                          END_OF_WAVE_DELAY;
					}
					continue;
				}

				/* Move the X position of the lander unless we are ascending */
				if (stLander[i].eState != GRABBED)
				{
					if ((stLander[i].x = 
								(stLander[i].x + stLander[i].dx) % PLANET_SIZE) < 0)
					{
						stLander[i].x += PLANET_SIZE;
					}
				}

				/* Descend if the lander has not yet landed */
				if (stLander[i].eState == LANDING)
				{
					if ((stLander[i].y -= DESCEND_SPEED) <=
  		    		  (height[stLander[i].x] + stBitmap[HUMAN].cy + HUMAN_YOFFSET))
					{
						stLander[i].y = height[stLander[i].x] + 
													stBitmap[HUMAN].cy + HUMAN_YOFFSET;
						stLander[i].eState = LANDED;
					}
				}
				else if (stLander[i].eState == LANDED)
				{
					stLander[i].y = height[stLander[i].x] + 
													stBitmap[HUMAN].cy + HUMAN_YOFFSET;

				   /* Check whether we've reached the target human yet */
					if (abs (stLander[i].x - stLander[i].targetX) < MAX_LANDER_DX)
					{
                  MakeSound (SOUND_HUMAN_PICKUP);
						stLander[i].eState = GRABBED;
						stLander[i].sBitmap = LANDERC;
						stHuman[stLander[i].targetIdx].eState = GRABBED;
                  stLander[i].y = stHuman[stLander[i].targetIdx].y +
													stBitmap[HUMAN].cy + 2;
						/*
						** Look for any landers which also had this human as a
						** target and assign a new human to these landers
						*/
						for (j = 0;  j < NUM_LANDERS;  j++)
						{
							if (stLander[j].fActive && i != j &&
								 stLander[j].targetIdx == stLander[i].targetIdx)
							{
            				SetLanderTarget (j);
							}
						}
					}
				}
				else
				{
					if ((stLander[i].y += ASCEND_SPEED) >=
									 			   (PLAY_HEIGHT - stBitmap[LANDERL].cy))
					{
						/* Captured the human!! */
						stHuman[stLander[i].targetIdx].eState = CAPTURED;
						stLander[i].fActive = FALSE;
   					--gusBadGuys;

						/* At this point create a mutant to replace the lander */
						AddMutant (stLander[i].x, stLander[i].y, FALSE);
						continue;
					}
				}

				/* Calculate the new radar position and draw marker in green */
				stLander[i].rx = RADARX(stLander[i].x);
				stLander[i].ry = RADARY(stLander[i].y);
				DrawMarker (stLander[i].rx, stLander[i].ry, CLRGREEN, FALSE);

				/* Draw the lander if it is still on screen */
				if (stLander[i].fDraw = ON_SCREEN(stLander[i].x))
				{
				   stLander[i].screenx = SCREENX(stLander[i].x);
				   Draw (stLander[i].screenx, stLander[i].y, FALSE, 
										 stLander[i].sBitmap, TRUE, TRUE, i, gsMarker);
				}
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveMutants
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
VOID MoveMutants (VOID)
{
   INT   i, iRand;
	SHORT sXLeft, sXRight;

	for (i = 0;  i < NUM_MUTANTS;  i++)
	{
		if (stMutant[i].fActive)
		{
		   /* Handle the situation where the mutant is just appearing */
		   if (stMutant[i].eState == APPEARING)
			{
			   /* If this mutant is on screen, then handle its appearance */
				if (stMutant[i].fDraw)
				{
               if (gsSoundPlaying != SOUND_FORMATION)
               {
                  MakeSound (SOUND_FORMATION);
               }

					/* Erase the old appearance marker blobs */
				   DrawAppearance (stMutant[i].screenx, 
										 stMutant[i].y,
										 stMutant[i].sAppearCnt, 
										 CLRBLUE);

					if (!(stMutant[i].sAppearCnt -= APPEAR_SPEED)) 
					{
						/* The mutant has appeared! */
						stMutant[i].eState = MUTATE;
						stMutant[i].fDraw = FALSE;

						/*
						** Need to reposition the mutant, since its original X 
						** value has not been updated and the land may have moved
						*/
						stMutant[i].x = 
										(gsWorld + stMutant[i].screenx) % PLANET_SIZE;
					}
					else
					{
					   /* Draw the new appearance marker blobs */
				      DrawAppearance (stMutant[i].screenx, 
						   				 stMutant[i].y,
							   			 stMutant[i].sAppearCnt, 
								   		 CLRPINK);
					}
				}
				else if (!(stMutant[i].sAppearCnt -= APPEAR_SPEED)) 
				{
					/* The mutant has appeared! */
					stMutant[i].eState = MUTATE;
				}
			}
			else
			{
				/* Erase the old radar position */
				DrawMarker(stMutant[i].rx, stMutant[i].ry, CLRBLACK, TRUE);

	         /* Erase the old bitmap if it was on screen */
				if (stMutant[i].fDraw)
				{
				   Draw (stMutant[i].screenx, stMutant[i].y, TRUE, MUTANTL,
											  							FALSE, FALSE, 0, gsMarker);
				}

				/* If it's time, calculate a new direction for the mutant */
				if ((--stMutant[i].sChangeDir) == 0)
				{
					/* Save on rand calls be reusing the same value */
					iRand = rand();

					/* Pick a random duration before change direction */
					stMutant[i].sChangeDir = (iRand % MUTANT_CHANGE) + 1;

					/* 50% of time pick random direction - else move towards ship */
					if (iRand & 1)
					{
						stMutant[i].dy = (iRand & 2) ? 
					    WaveData[gsWave].usMutantDy : -WaveData[gsWave].usMutantDy;
					}
					else
					{
						stMutant[i].dy = (stMutant[i].y < stShip.y) ?
				       WaveData[gsWave].usMutantDy : -WaveData[gsWave].usMutantDy;
					}

					/*
					** These boundaries form a box half the planet width to the
					** right of the ship.  We then see if the mutant is in this box
					*/
					sXLeft = ((gsWorld + stShip.x) % PLANET_SIZE);
					sXRight = (sXLeft + (PLANET_SIZE / 2)) % PLANET_SIZE;

					/* This code determines whether mutant is to R or L of ship */
					stMutant[i].dx = ((sXLeft < sXRight) ?
										((stMutant[i].x >= sXLeft) &&
												(stMutant[i].x <= sXRight)) :
										((stMutant[i].x >= sXLeft) ||
												(stMutant[i].x <= sXRight))) ? 
												-WaveData[gsWave].usMutantDx :
																WaveData[gsWave].usMutantDx;
				}

				/* Move the X position of the mutant */
				if ((stMutant[i].x =
									  (stMutant[i].x + stMutant[i].dx) % PLANET_SIZE) < 0)
				{
					stMutant[i].x += PLANET_SIZE;
				}

				/* Move the Y position of the mutant - keeping it in bounds */
				if ((stMutant[i].y += stMutant[i].dy) < 0)
				{
					stMutant[i].y = 0;
					stMutant[i].dy = WaveData[gsWave].usMutantDy;
				}
				else if (stMutant[i].y + stBitmap[MUTANTL].cy >= PLAY_HEIGHT)
				{
					stMutant[i].y = PLAY_HEIGHT - stBitmap[MUTANTL].cy;
					stMutant[i].dy = -WaveData[gsWave].usMutantDy;
				}

				/* Calculate the new radar position and draw marker in pink */
				stMutant[i].rx = RADARX(stMutant[i].x);
				stMutant[i].ry = RADARY(stMutant[i].y);
				DrawMarker (stMutant[i].rx, stMutant[i].ry, CLRRED, FALSE);

				/* Draw the mutant if it is still on screen */
				if (stMutant[i].fDraw = ON_SCREEN(stMutant[i].x))
				{
				   stMutant[i].screenx = SCREENX(stMutant[i].x);
				   Draw (stMutant[i].screenx, stMutant[i].y, FALSE, 
					        			(stMutant[i].dx > 0) ? MUTANTR : MUTANTL,
										TRUE, TRUE, i, gsMarker);
				}
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MovePods
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
VOID MovePods (VOID)
{
   INT   i, iRand;

	/* Array of bitmaps for the animation of the jellyfish pod */
	static SHORT asFrame[] = {JELLY1, JELLY2, JELLY3, JELLY2};

   #define NUM_POD_FRAMES (sizeof(asFrame) / sizeof(SHORT))

	for (i = 0;  i < NUM_PODS;  i++)
	{
		if (stPod[i].fActive)
		{
			/* Erase the old radar position */
			DrawMarker(stPod[i].rx, stPod[i].ry, CLRBLACK, TRUE);

	      /* Erase the old bitmap if it was on screen */
			if (stPod[i].fDraw)
			{
			   Draw (stPod[i].screenx, stPod[i].y, TRUE, JELLY1,
										  							FALSE, FALSE, 0, gsMarker);
			}

			/* Move the X position of the pod */
			if ((stPod[i].x =                                       
							  (stPod[i].x + stPod[i].dx) % PLANET_SIZE) < 0)
			{
				stPod[i].x += PLANET_SIZE;
			}

			/* Move the Y position of the pod - keeping it in bounds */
			if ((stPod[i].y += stPod[i].dy) < 0)
			{
				stPod[i].y = 0;
				stPod[i].dy = -stPod[i].dy;
			}
			else if (stPod[i].y + stBitmap[JELLY1].cy >= PLAY_HEIGHT)
			{
				stPod[i].y = PLAY_HEIGHT - stBitmap[JELLY1].cy;
				stPod[i].dy = -stPod[i].dy;
			}

			/* See if it's time to change direction */
			if (--stPod[i].sChangeDir == 0)
			{
				/* Pick a random duration before change direction */
				stPod[i].sChangeDir = (rand() % POD_CHANGE) + 1;

				stPod[i].dy = (rand() % MAX_POD_DY) + 1;
				if (rand() & 1)
				{
					stPod[i].dy = -stPod[i].dy;
				}
			}

			/* Calculate the new radar position and draw marker in yellow */
			stPod[i].rx = RADARX(stPod[i].x);
			stPod[i].ry = RADARY(stPod[i].y);
			DrawMarker (stPod[i].rx, stPod[i].ry, CLRPINK, FALSE);

			/* Draw the pod if it is still on screen */
			if (stPod[i].fDraw = ON_SCREEN(stPod[i].x))
			{
			   /* Change animation frame */
				if (++stPod[i].sChangeFrame == POD_FRAME_RATE)
				{
					stPod[i].sFrame = ((++stPod[i].sFrame) % NUM_POD_FRAMES);
					stPod[i].sChangeFrame = 0;
				}
			   stPod[i].screenx = SCREENX(stPod[i].x);
			   Draw (stPod[i].screenx, stPod[i].y, FALSE, 
								asFrame[stPod[i].sFrame], TRUE, TRUE, i, gsMarker);
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveRings
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
VOID MoveRings (VOID)
{
   INT   i;

	for (i = 0;  i < NUM_RINGS;  i++)
	{
		if (stRing[i].fActive)
		{
			/* Erase the old radar position */
			DrawMarker(stRing[i].rx, stRing[i].ry, CLRBLACK, TRUE);

	      /* Erase the old bitmap if it was on screen */
			if (stRing[i].fDraw)
			{
			   Draw (stRing[i].screenx, stRing[i].screeny,
                  TRUE, stRing[i].bitmap, FALSE, FALSE, 0, gsMarker);
			}

         /* If the ring has been shot move it! */
         if (stRing[i].fShot)
         {
            /* Move the X position on the ring */
   			if ((stRing[i].x =
							  (stRing[i].x + stRing[i].dx) % PLANET_SIZE) < 0)
   			{
	   			stRing[i].x += PLANET_SIZE;
		   	}
            
			   /* Move the Y position of the ring checking for end of ring */
			   if ((stRing[i].y += stRing[i].dy) < 0)
			   {
               /* This ring is done! */
				   stRing[i].fActive = FALSE;

					/* Check whether this is the last baddy */
   				if (--gusBadGuys == 0)
					{
						/* Change the next event to be end of wave */
						stEvent[gusNextEvent].eEvent = WAVE_END;
						stEvent[gusNextEvent].ulTime = gulWaveTimer + 
																		END_OF_WAVE_DELAY;
   				   MakeSound (SOUND_LASTONE_HIT);
					}
               continue;
			   }

			   /* See if it's time to change direction */
			   if (--stRing[i].usChangeDir == 0)
			   {
               stRing[i].usDirIdx++;
               stRing[i].dx = astRingMove[stRing[i].usDirIdx].dx *
                                                         stRing[i].dxMaster;
               stRing[i].dy = astRingMove[stRing[i].usDirIdx].dy;
               stRing[i].usChangeDir = astRingMove[stRing[i].usDirIdx].count;
				}
			}

			/* Calculate the new radar position and draw marker in red */
			stRing[i].rx = RADARX(stRing[i].x);
			stRing[i].ry = RADARY(stRing[i].y);
			DrawMarker (stRing[i].rx, stRing[i].ry, CLRRED, FALSE);

			/* Draw the ring if it is still on screen */
			if (stRing[i].fDraw = ON_SCREEN(stRing[i].x))
			{
            if (stRing[i].fShot)
            {
               stRing[i].bitmap = (stRing[i].dxMaster > 0) ? 
                           astRingMove[stRing[i].usDirIdx].bitmapR :
                                 astRingMove[stRing[i].usDirIdx].bitmapL;
			      stRing[i].screenx = SCREENX(stRing[i].x) +
                                           stBitmap[stRing[i].bitmap].dx;
               stRing[i].screeny = stRing[i].y + 
                                           stBitmap[stRing[i].bitmap].dy;
            }
            else
            {
			      stRing[i].screenx = SCREENX(stRing[i].x);
               stRing[i].screeny = stRing[i].y; 
			      stRing[i].bitmap = RING;
            }
		      Draw (stRing[i].screenx, stRing[i].screeny, FALSE,
                               stRing[i].bitmap, TRUE, TRUE, i, gsMarker);
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveMermaid
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
VOID MoveMermaid (VOID)
{
   INT  iRand;

	/* Array of bitmaps for the animation of the jellyfish pod */
	static USHORT ausFrame[] = {MERMAID1,
										 MERMAID2,
										 MERMAID3,
										 MERMAID4,
										 MERMAID5,
										 MERMAID4,
										 MERMAID3,
										 MERMAID2};

   #define NUM_MERMAID_FRAMES (sizeof(ausFrame) / sizeof(USHORT))

	/* Decrement the appearance count  - should she appear now? */
	if (--stMermaid.sAppearance == 0 && !stMermaid.fActive)
	{
		stMermaid.fActive = TRUE;
		stMermaid.sAppearance = MERMAID_PERSISTENCE;
      MakeSound (SOUND_MERMAID_APPEARS);
	}

	if (stMermaid.fActive)
	{
		/* Erase the old radar position */
		DrawMarker(stMermaid.rx, stMermaid.ry, CLRBLACK, TRUE);

	   /* Erase the old bitmap if it was on screen */
		if (stMermaid.fDraw)
		{
		   Draw (stMermaid.screenx,
					stMermaid.y + stBitmap[ausFrame[stMermaid.sFrame]].dy,
					TRUE, ausFrame[stMermaid.sFrame], 
					FALSE, FALSE, 0, gsMarker);
		}

		if (stMermaid.sAppearance == 0)
		{
			/* If her time is up, then reinitialize her */
			InitMermaid ();
			return;
		}

		/* Move the X position of the mermaid */
		if ((stMermaid.x =
						  (stMermaid.x + stMermaid.dx) % PLANET_SIZE) < 0)
		{
			stMermaid.x += PLANET_SIZE;
		}

		/* 
		** Move the Y position of the pod - keeping it in bounds -
		** MERMAID3 has the largest Y size 
		*/
		if ((stMermaid.y += stMermaid.dy) < 0)
		{
			stMermaid.y = 0;
			stMermaid.dy = -stMermaid.dy;
		}
		else if (stMermaid.y + stBitmap[MERMAID3].cy >= PLAY_HEIGHT)
		{
			stMermaid.y = PLAY_HEIGHT - stBitmap[MERMAID3].cy;
			stMermaid.dy = -stMermaid.dy;
		}

		/* See if it's time to change direction */
		if (--stMermaid.sChangeDir == 0)
		{
			/* Pick a random duration before change direction */
			stMermaid.sChangeDir = (rand() % MERMAID_CHANGE) + 1;

			stMermaid.dy = (rand() % MAX_MERMAID_DY) + 1;
			if (rand() & 1)
			{
				stMermaid.dy = -stMermaid.dy;
			}
		}

		/* Calculate the new radar position and draw marker in yellow */
		stMermaid.rx = RADARX(stMermaid.x);
		stMermaid.ry = RADARY(stMermaid.y);
		DrawMarker (stMermaid.rx, stMermaid.ry, CLRWHITE, FALSE);

		/* Draw the pod if it is still on screen */
		if (stMermaid.fDraw = ON_SCREEN(stMermaid.x))
		{
		   /* Change animation frame */
			if (++stMermaid.sChangeFrame == POD_FRAME_RATE)
			{
				stMermaid.sFrame = ((++stMermaid.sFrame) % 
																NUM_MERMAID_FRAMES);
				stMermaid.sChangeFrame = 0;
			}
		   stMermaid.screenx = SCREENX(stMermaid.x) + 
					                  stBitmap[ausFrame[stMermaid.sFrame]].dx;
		   Draw (stMermaid.screenx, 
					stMermaid.y + stBitmap[ausFrame[stMermaid.sFrame]].dy,
					FALSE, ausFrame[stMermaid.sFrame], 
					TRUE, TRUE, 0, gsMarker);
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: MoveBombers
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
VOID MoveBombers (VOID)
{
   INT   i;

	for (i = 0;  i < NUM_BOMBERS;  i++)
	{
		if (stBomber[i].fActive)
		{
			/* Erase the old radar position */
			DrawMarker(stBomber[i].rx, stBomber[i].ry, CLRBLACK, TRUE);

	      /* Erase the old bitmap if it was on screen */
			if (stBomber[i].fDraw)
			{
			   Draw (stBomber[i].screenx, stBomber[i].y, TRUE, BOMBERL,
										  							FALSE, FALSE, 0, gsMarker);
			}

			/* Move the X position of the pod */
			if ((stBomber[i].x =
							  (stBomber[i].x + stBomber[i].dx) % PLANET_SIZE) < 0)
			{
				stBomber[i].x += PLANET_SIZE;
			}

			/* Calculate the new radar position and draw marker in gray */
			stBomber[i].rx = RADARX(stBomber[i].x);
			stBomber[i].ry = RADARY(stBomber[i].y);
			DrawMarker (stBomber[i].rx, stBomber[i].ry, CLRPALEGRAY, FALSE);

			/* Draw the bomber if it is still on screen */
			if (stBomber[i].fDraw = ON_SCREEN(stBomber[i].x))
			{
			   stBomber[i].screenx = SCREENX(stBomber[i].x);
			   Draw (stBomber[i].screenx, stBomber[i].y, FALSE, 
										stBomber[i].sBitmap,	TRUE, TRUE, i, gsMarker);

			   /* Decrement the bomb timer to plant bombs */
				if (--stBomber[i].sBombTimer == 0)
				{
					stBomber[i].sBombTimer = (rand() % MAX_BOMB_SPACING) + 
															  WaveData[gsWave].usBombSpacing;
					PlantBomb (stBomber[i].x + stBitmap[BOMBERL].cx / 2,
					           stBomber[i].y);
				}
			}
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: MoveBombs
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
VOID MoveBombs (VOID)
{
   INT   i;

	for (i = 0;  i < NUM_BOMBS;  i++)
	{
		if (stBomb[i].fActive)
		{
	      /* Erase the old bitmap if it was on screen */
			if (stBomb[i].fDraw)
			{
			   Draw (stBomb[i].screenx, stBomb[i].y, TRUE, BOMB,
										  							FALSE, FALSE, 0, 0);
			}

			/* Make the bomb fall */
			stBomb[i].y -= BOMB_DY;

			/* Draw the bomb if it is still on screen */
			if (--stBomb[i].sDuration > 0 &&
							(stBomb[i].fDraw = ON_SCREEN(stBomb[i].x)) &&
							stBomb[i].y > 0)
			{
			   stBomb[i].screenx = SCREENX(stBomb[i].x);
			   Draw (stBomb[i].screenx, stBomb[i].y, FALSE, BOMB,
																	FALSE, TRUE, i, 0);
			}
			else
			{
			   /* The bomb is off the screen or has expired */
				stBomb[i].fActive = FALSE;
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveBullets
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
VOID MoveBullets (VOID)
{
   INT   i;

	for (i = 0;  i < NUM_BULLETS;  i++)
	{
		if (stBullet[i].fActive)
		{
	      /* Erase the old bitmap if it was on screen */
			if (stBullet[i].fDraw)
			{
			   Draw (stBullet[i].screenx, stBullet[i].y, TRUE, BULLET, FALSE, 
																					FALSE, 0, 0);
		   }

			/* Move the bullet */
			stBullet[i].x += stBullet[i].dx;
			stBullet[i].y += stBullet[i].dy;

			/* 
			** Check that the bullet has not gone off top or bottom of screen,
			** or the option to have alien bullets has not changed to off
			*/
			if (stBullet[i].y < 0 ||
			    (stBullet[i].y + stBitmap[BULLET].cy) > PLAY_HEIGHT ||
				 !gfBullets)
			{
				stBullet[i].fActive = FALSE;
				gusNumBullets--;
			}

			/* Draw the bullet in the new position */
			else if ((stBullet[i].fDraw = ON_SCREEN(stBullet[i].x)))
			{
			   stBullet[i].screenx = SCREENX(stBullet[i].x);
			   Draw (stBullet[i].screenx, stBullet[i].y, FALSE, BULLET,
																	FALSE, TRUE, i, 0);
			}
			else
			{
				stBullet[i].fActive = FALSE;
				gusNumBullets--;
			}
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: MoveSwarmers
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
VOID MoveSwarmers (VOID)
{
   INT   i;

	for (i = 0;  i < NUM_SWARMERS;  i++)
	{
		if (stSwarmer[i].fActive)
		{
			/* Erase the old radar position */
			DrawMarker(stSwarmer[i].rx, stSwarmer[i].ry, CLRBLACK, TRUE);

	      /* Erase the old bitmap if it was on screen */
			if (stSwarmer[i].fDraw)
			{
			   Draw (stSwarmer[i].screenx, stSwarmer[i].y, TRUE, SWARMER,
										  							FALSE, FALSE, 0, gsMarker);
			}

			/* Move the X position of the swarmer unless just created */
			if (stSwarmer[i].sAccelChange <= SWARMER_ACCEL)
			{
			   if ((stSwarmer[i].x =
							  (stSwarmer[i].x + stSwarmer[i].dx) % PLANET_SIZE) < 0)
				{
					stSwarmer[i].x += PLANET_SIZE;
				}
			}

			/* Move the Y position of the swarmer - keeping it in bounds */
			if ((stSwarmer[i].y += stSwarmer[i].dy) < 0)
			{
				stSwarmer[i].y = 0;
				stSwarmer[i].dy = -stSwarmer[i].dy;
			}
			else if (stSwarmer[i].y + stBitmap[SWARMER].cy >= PLAY_HEIGHT)
			{
				stSwarmer[i].y = PLAY_HEIGHT - stBitmap[SWARMER].cy;
				stSwarmer[i].dy = -stSwarmer[i].dy;
			}

			/* Change the acceleration of the swarmer */
			if (--stSwarmer[i].sAccelChange == 0)
			{
				stSwarmer[i].sAccelChange = SWARMER_ACCEL;

			   /* If near apex, then change the direction */
				if (stSwarmer[i].dy2 == 1)
				{
				   if (stSwarmer[i].y > SLOW_AT_TOP)
					{
					   stSwarmer[i].dy2 = -1;
					}
					else if (stSwarmer[i].dy < MAX_SWARMER_DY)
					{
					   stSwarmer[i].dy += stSwarmer[i].dy2;
					}
		   	}
			   else
			   {
				   if (stSwarmer[i].y < SLOW_AT_BOTTOM)
					{
					   stSwarmer[i].dy2 = 1;
					}
					else if (stSwarmer[i].dy > -MAX_SWARMER_DY)
					{
					   stSwarmer[i].dy += stSwarmer[i].dy2;
					}
				}
			}

			/* Calculate the new radar position and draw marker in red */
			stSwarmer[i].rx = RADARX(stSwarmer[i].x);
			stSwarmer[i].ry = RADARY(stSwarmer[i].y);
			DrawMarker (stSwarmer[i].rx, stSwarmer[i].ry, CLRWHITE, FALSE);

			/* Draw the swarmer if it is still on screen */
			if (stSwarmer[i].fDraw = ON_SCREEN(stSwarmer[i].x))
			{
			   stSwarmer[i].screenx = SCREENX(stSwarmer[i].x);
			   Draw (stSwarmer[i].screenx, stSwarmer[i].y, FALSE, SWARMER,
																	TRUE, TRUE, i, gsMarker);
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveSkimmers
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
VOID MoveSkimmers (VOID)
{
   INT   i;
	SHORT sXLeft, sXRight;

	for (i = 0;  i < NUM_SKIMMERS;  i++)
	{
		if (stSkimmer[i].fActive)
		{
			/* Erase the old radar position */
			DrawMarker(stSkimmer[i].rx, stSkimmer[i].ry, CLRBLACK, TRUE);

	      /* Erase the old bitmap if it was on screen */
			if (stSkimmer[i].fDraw)
			{
			   Draw (stSkimmer[i].screenx, stSkimmer[i].y, TRUE, SKIMMERL,
										  							FALSE, FALSE, 0, gsMarker);
			}

			/* If the skimmer is hovering then do not move */
			if (!stSkimmer[i].sHoverTimer)
			{
				/* Move the X position of the skimmer unless just created */
			   if ((stSkimmer[i].x =
							  (stSkimmer[i].x + stSkimmer[i].dx) % PLANET_SIZE) < 0)
				{
					stSkimmer[i].x += PLANET_SIZE;
				}

				/* Move the Y position of the skimmer - keeping it in bounds */
				if ((stSkimmer[i].y += stSkimmer[i].dy) < 0)
				{
					stSkimmer[i].y = 0;
					stSkimmer[i].dy = -stSkimmer[i].dy;
				}
				else if (stSkimmer[i].y + stBitmap[SKIMMERL].cy >= PLAY_HEIGHT)
				{
					stSkimmer[i].y = PLAY_HEIGHT - stBitmap[SKIMMERL].cy;
					stSkimmer[i].dy = -stSkimmer[i].dy;
				}

				/* Move until the thrust timer has expired */
				if (--stSkimmer[i].sThrustTimer == 0)
				{
					stSkimmer[i].sHoverTimer = SKIMMER_HOVER_TIME;
				}
			}
			else
			{
				if (--stSkimmer[i].sHoverTimer == 0)
				{
					/* 
					** The hover time has elapsed - set up the acceleration...
					** These boundaries form a box half the planet width to the
					** right of the ship.  We then see if the mutant is in this box
					*/
					sXLeft = ((gsWorld + stShip.x) % PLANET_SIZE);
					sXRight = (sXLeft + (PLANET_SIZE / 2)) % PLANET_SIZE;
					
					/* This code determines whether skimmer is to R or L of ship */
					stSkimmer[i].dx = ((sXLeft < sXRight) ?
										((stSkimmer[i].x >= sXLeft) &&
											(stSkimmer[i].x <= sXRight)) :
										((stSkimmer[i].x >= sXLeft) ||
											(stSkimmer[i].x <= sXRight))) ? -SKIMMER_DX
																				  : SKIMMER_DX;

					stSkimmer[i].dy =(stShip.y-stSkimmer[i].y)/SKIMMER_THRUST_TIME;
					stSkimmer[i].sThrustTimer = SKIMMER_THRUST_TIME;
				}
			}

			/* Calculate the new radar position and draw marker in green */
			stSkimmer[i].rx = RADARX(stSkimmer[i].x);
			stSkimmer[i].ry = RADARY(stSkimmer[i].y);
			DrawMarker (stSkimmer[i].rx, stSkimmer[i].ry, CLRPALEGRAY, FALSE);

			/* Draw the skimmer if it is still on screen */
			if (stSkimmer[i].fDraw = ON_SCREEN(stSkimmer[i].x))
			{
			   stSkimmer[i].screenx = SCREENX(stSkimmer[i].x);
			   Draw (stSkimmer[i].screenx, stSkimmer[i].y, FALSE, 
									(stSkimmer[i].dx > 0) ? SKIMMERR : SKIMMERL,
									TRUE, TRUE, i, gsMarker);
			}
		}
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveLasers
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
VOID MoveLasers (VOID)
{
	INT i, j;
	SHORT xMin, xMax;

	for (i = 0;  i < NUM_LASERS;  i++)
	{
		if (stLaser[i].fActive)
		{
			/* Add a line to the laser draw array to erase the old bolt */
			if (stLaser[i].length > 0)
			{
				DrawLaser (stLaser[i].x, stLaser[i].y, 
                                     stLaser[i].x1, stLaser[i].y, CLR_BLUE);
			}

			/* Expand out the laser if it is not max length */
			if (stLaser[i].length < LASER_MAX)
			{
				stLaser[i].length += LASER_INC;
			}

			/* Handle the laser moving to the right */
			if (stLaser[i].dx == 1)
			{
				/* See if we're now off the screen */
				if ((stLaser[i].x += LASER_SPEED) > SCREEN_WIDTH)
				{
					stLaser[i].fActive = FALSE;
					continue;
				}

				stLaser[i].x1 = stLaser[i].x + stLaser[i].length;
			}

			/* Handle the laser moving to the left */
			else
			{
				/* See if we're now off the screen */
				if ((stLaser[i].x -= LASER_SPEED) < 0)
				{
					stLaser[i].fActive = FALSE;
					continue;
				}

				stLaser[i].x1 = stLaser[i].x - stLaser[i].length;
			}

			/*
			** Check for laser hits by looking for any correspondence in the
			** draw list - only look at items which are flagged to be checked
			*/
			for (j = 0;  j < usDrawItems;  j++)
			{
				if (stDrawItem[j].fLaserCheck && stDrawItem[j].fDraw)
				{
					/* Eliminate anything not at the right Y position */
					if (stLaser[i].y >= stDrawItem[j].y &&
					    stLaser[i].y < (stDrawItem[j].y +
						 							stBitmap[stDrawItem[j].bitmap].cy))
					{
						if (stLaser[i].dx == 1)
						{
							xMin = stLaser[i].x;
							xMax = stLaser[i].x1;
						}
						else
						{
							xMin = stLaser[i].x1;
							xMax = stLaser[i].x;
						}

						/* Now check the X values - think about this! */
						if (xMax >= stDrawItem[j].x &&
					    		xMin < (stDrawItem[j].x +
						 							stBitmap[stDrawItem[j].bitmap].cx))
						{
							/* We have hit something!! Process the hit */
							HandleHit (j, TRUE);
							stLaser[i].fActive = FALSE;
							break;
						}
					}
				}
			}

			/* Draw the new laser position unless we hit something */
			if (stLaser[i].fActive)
			{
				DrawLaser (stLaser[i].x, stLaser[i].y, 
                                     stLaser[i].x1, stLaser[i].y, LASERCOLOR);
			}
		}
	}
}


/**************************************************************************
*
* FUNCTION NAME: MoveScanner
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
VOID MoveScanner (VOID)
{
   static BOOL fDrawn = FALSE;

	
	/* Erase the old line if scanner is on if it was drawwn previously */
	if (fDrawn)
	{
		/* Add a line to the laser draw array to erase the old line */
		DrawLaser (stScanner.rx, stScanner.ry1, 
                                  stScanner.rx, stScanner.ry2, CLR_BLACK);
	}

	if (stScanner.sMode != SCANNER_OFF)
	{
      /* Move the radar line */
		if ((stScanner.rx += SCANNER_DX) >= (RADAR_WIDTH + STATUS_WIDTH))
      {
         stScanner.rx -= RADAR_WIDTH;
         MakeSound (SOUND_SONAR);
      }

		DrawLaser (stScanner.rx, stScanner.ry1, 
                                  stScanner.rx, stScanner.ry2, CLR_GREEN);
		fDrawn = TRUE;
	}
}

/**************************************************************************
*
* FUNCTION NAME: HandleHit
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
VOID HandleHit (SHORT sDrawIndex, BOOL fLaserHit)
{
   INT i, j;

	i = stDrawItem[sDrawIndex].sDrawIdx;

	/* Processing depends on what object was hit */
	switch (stDrawItem[sDrawIndex].bitmap)
	{
		case LANDERL:
		case LANDERR:
		case LANDERC:
         gusBadGuys--;
			stLander[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[LANDERL].cx / 2),
								 stDrawItem[sDrawIndex].y + (stBitmap[LANDERL].cy / 2),
								 CLRGREEN);

			/* If the lander was carrying a human, then the human now falls */
			if (stLander[i].eState == GRABBED)
			{
				stHuman[stLander[i].targetIdx].eState = LANDING;
				stHuman[stLander[i].targetIdx].dy = 0;
				stHuman[stLander[i].targetIdx].dy2 = 0;
			}

			IncScore (LANDER_SCORE);
			if (fLaserHit)
			{
				gsAliens++;
			}
			break;

		case MUTANTL:
		case MUTANTR:
         gusBadGuys--;
			stMutant[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[MUTANTL].cx / 2),
								 stDrawItem[sDrawIndex].y + (stBitmap[MUTANTL].cy / 2),
								 CLRRED);
			IncScore (MUTANT_SCORE);
			if (fLaserHit)
			{
				gsAliens++;
			}
			break;

		case JELLY1:
		case JELLY2:
		case JELLY3:
         gusBadGuys--;
			stPod[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[JELLY1].cx / 2),
								 stDrawItem[sDrawIndex].y + (stBitmap[JELLY1].cy / 2),
								 CLRCYAN);
			IncScore (POD_SCORE);
			if (fLaserHit)
			{
				gsAliens++;
			}

			/* Add 4 swarmers - released from the dead pod */
			AddSwarmers (4, stPod[i].x + (stBitmap[JELLY1].cx / 2),
							    stPod[i].y + (stBitmap[JELLY1].cy / 2),
								 (rand() & 1) ? SWARMER_DX : -SWARMER_DX);
         MakeSound (SOUND_POD_HIT);
			break;

		case SWARMER:
         gusBadGuys--;
			stSwarmer[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[SWARMER].cx / 2),
								 stDrawItem[sDrawIndex].y + (stBitmap[SWARMER].cy / 2),
								 CLRWHITE);
			IncScore (SWARMER_SCORE);
			if (fLaserHit)
			{
				gsAliens++;
			}
			break;

		case RING:
			IncScore (RING_SCORE);
			if (fLaserHit)
			{
            /* If the ring was shot then go into burst loop */
			   stRing[i].fShot = TRUE;
            stRing[i].usDirIdx = 0;
            stRing[i].dxMaster = stShip.fFacingRight ? 1 : -1;
            stRing[i].dx = astRingMove[0].dx * stRing[i].dxMaster;
            stRing[i].dy = astRingMove[0].dy;
            stRing[i].usChangeDir = astRingMove[0].count;
				gsAliens++;
            MakeSound (SOUND_RING_BURST);
			}
         else
         {
         	gusBadGuys--;
			   stRing[i].fActive = FALSE;
			   stDrawItem[sDrawIndex].fDraw = FALSE;
			   stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
            CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[RING].cx / 2),
				   				 stDrawItem[sDrawIndex].y + (stBitmap[RING].cy / 2),
					   			 CLRRED);
         }
			break;

		case BOMBERL:
		case BOMBERR:
		   if (--stBomber[i].sHits == 0)
			{
         	gusBadGuys--;
				stBomber[i].fActive = FALSE;
				stDrawItem[sDrawIndex].fDraw = FALSE;
				stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         	CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[BOMBERL].cx/2),
									 stDrawItem[sDrawIndex].y + (stBitmap[BOMBERL].cy/2),
									 CLRPALEGRAY);
				IncScore (BOMBER_SCORE);
				if (fLaserHit)
				{
					gsAliens++;
				}
			}
			break;

		case SKIMMERL:
		case SKIMMERR:
		   if (--stSkimmer[i].sHits == 0)
			{
         	gusBadGuys--;
				stSkimmer[i].fActive = FALSE;
				stDrawItem[sDrawIndex].fDraw = FALSE;
				stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
         	CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[SKIMMERL].cx/2),
									 stDrawItem[sDrawIndex].y + (stBitmap[SKIMMERL].cy/2),
									 CLRPALEGRAY);
				IncScore (SKIMMER_SCORE);
				if (fLaserHit)
				{
					gsAliens++;
				}
			}
			break;

		case BOMB:
			stBomb[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			break;

		case BULLET:
			stBullet[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			gusNumBullets--;
			break;

		case MERMAID1:
		case MERMAID2:
		case MERMAID3:
		case MERMAID4:
		case MERMAID5:
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
			InitMermaid ();
			CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[MERMAID3].cx/2),
								 stDrawItem[sDrawIndex].y + (stBitmap[MERMAID3].cy/2),
								 CLRPINK);

			/* Subtract 1000 points! */
			IncScore (-MERMAID_SCORE);
         MakeSound (SOUND_MERMAID_KILLED);
			break;

		case HUMAN:
			stHuman[i].fActive = FALSE;
			stDrawItem[sDrawIndex].fDraw = FALSE;
			stMarkerItem[stDrawItem[sDrawIndex].sRadarIdx].fDraw = FALSE;
			CreateExplosion(stDrawItem[sDrawIndex].x + (stBitmap[HUMAN].cx / 2),
								 stDrawItem[sDrawIndex].y + (stBitmap[HUMAN].cy / 2),
								 CLRYELLOW);
         MakeSound (SOUND_HUMAN_HIT);
	      if (--usNumHumans == 0)
			{
			   /* This was the last human - mutate all the landers */
				MutateAll (TRUE);
			}
			else
			{
				/*
				** Look for any landers which had this human as a target and
				** assign a new human to these landers
				*/
				for (j = 0;  j < NUM_LANDERS;  j++)
				{
					if (stLander[j].fActive && stLander[j].targetIdx == i)
					{
         			SetLanderTarget (j);
					}
				}
			}
			break;

		default:
			break;
	}

	/* See if there are any baddies left */
   if (gusBadGuys == 0 && stDrawItem[sDrawIndex].bitmap != HUMAN)
	{
		/* Change the next event to be end of wave */
		stEvent[gusNextEvent].eEvent = WAVE_END;
		stEvent[gusNextEvent].ulTime = gulWaveTimer + END_OF_WAVE_DELAY;
      MakeSound (SOUND_LASTONE_HIT);
	}
}

/**************************************************************************
*
* FUNCTION NAME: MutateAll
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
VOID MutateAll (BOOL fFlash)
{
   INT i;
	RECTL rcl;

   /* Set the land active flag to FALSE */
   stLandDraw.fActive = FALSE;

	/* Turn all the landers into mutants */
	for (i = 0;  i < NUM_LANDERS;  i++)
	{
		if (stLander[i].fActive)
		{
			AddMutant (stLander[i].x, stLander[i].y, FALSE);
			stLander[i].eState = MUTATE;
		}
	}

	/* Flash the screen a few times to indicate we're in space */
	if (fFlash)
	{
		WinQueryWindowRect (hwndDefClient, &rcl);
		for (i = 0;  i < 10;  i++)
		{
   		WinFillRect (hps, &rcl, LASERCOLOR);
      	WinInvalidateRect (hwndDefClient, &rcl, TRUE);
			DrawScreen (hps);
		}

		PAINT (REFRESH_SCREEN);
	}
}

/**************************************************************************
*
* FUNCTION NAME: MoveHumans
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
VOID MoveHumans (VOID)
{
   INT  i;
   CHAR szString[5];

	for (i = 0;  i < NUM_HUMANS;  i++)
	{
		if (stHuman[i].fActive && stHuman[i].eState != CARRIED)
		{
			/* Erase the old radar position */
			DrawMarker (stHuman[i].rx, stHuman[i].ry, CLRBLACK, TRUE);
			
	   	/* Erase the old bitmap if it was on screen */
			if (stHuman[i].fDraw)
			{
			   Draw (stHuman[i].screenx, stHuman[i].y, TRUE, HUMAN,
																	FALSE, FALSE, 0, gsMarker);
			}
			
			/* Ascend if the human has been grabbed by a lander */
			if (stHuman[i].eState == GRABBED)
			{
			   /*
				** Note: no need to check for limit since lander will hit top
				** of screen before the human does
				*/
				stHuman[i].y += ASCEND_SPEED;
			}
			else if (stHuman[i].eState == CAPTURED)
			{
				stHuman[i].fActive = FALSE;
			   if (--usNumHumans == 0)
				{
					MutateAll (TRUE);
				}
				continue;
		   }
			else if (stHuman[i].eState == LANDING)
			{
				/* Accelerate to the ground unless terminal velocity reached */
				if ((stHuman[i].dy2 = ++stHuman[i].dy2 % HUMAN_DY2) == 0 &&
															stHuman[i].dy < HUMAN_TERMVEL)
				{
					stHuman[i].dy++;
				}

				/* Has the human hit the ground yet? */
				if ((stHuman[i].y -= stHuman[i].dy) < height[stHuman[i].x])
				{
					/* If the humans speed is high then he will die */
					if (stHuman[i].dy >= HUMAN_DEATHVEL)
					{
						stHuman[i].fActive = FALSE;
			         if (--usNumHumans == 0)
						{
							MutateAll (TRUE);
						}

						if (ON_SCREEN (stHuman[i].x))
						{
			   			CreateExplosion (SCREENX(stHuman[i].x),
												  height[stHuman[i].x],
												  CLRCYAN);
						}
						continue;
					}
					else
					{
						/* The human can reestablish on the ground */
						stHuman[i].eState = LANDED;
						stHuman[i].y = height[stHuman[i].x] - HUMAN_YOFFSET;
						stHuman[i].fTargetted = FALSE;
						IncScore (DROPPED_HUMAN_SCORE);
	   				sprintf (szString, "%d", DROPPED_HUMAN_SCORE);
	   				DisplayText (szString, 
										 (SCREENX(stHuman[i].x) - 2 * gsCharcx), 
		             				 (stHuman[i].y + stBitmap[HUMAN].cy), 
										 CLR_PINK, FALSE, FALSE);
					}
				}
			}
			
			/* Calculate the new radar position and draw marker in cyan */
			stHuman[i].rx = RADARX(stHuman[i].x);
			stHuman[i].ry = RADARY(stHuman[i].y);
			DrawMarker (stHuman[i].rx, stHuman[i].ry, CLRCYAN, FALSE);
			
			/* Draw the human if it is still on screen */
			if (stHuman[i].fDraw = ON_SCREEN(stHuman[i].x))
			{
			   stHuman[i].screenx = SCREENX(stHuman[i].x);
			   Draw (stHuman[i].screenx, stHuman[i].y, FALSE, HUMAN, TRUE,
									(stHuman[i].eState == LANDING), i, gsMarker);
			}
		}
	}		
}			

/**************************************************************************
*
* FUNCTION NAME: MoveCarriedHuman
*
* DESCRIPTION:
*
*
* INPUT PARAMETERS:
*     SHORT i;
*
* OUTPUT PARAMETERS:
*     None.
*
**************************************************************************/
VOID MoveCarriedHuman (SHORT i)
{
   CHAR szString[5];

	/* Erase the old radar position */
	DrawMarker (stHuman[i].rx, stHuman[i].ry, CLRBLACK, TRUE);
	
	/* Erase the old bitmap */
	Draw (stHuman[i].screenx, stHuman[i].y, TRUE, HUMAN,
															FALSE, FALSE, 0, gsMarker);
	
	stHuman[i].y = stShip.y - stBitmap[HUMAN].cy;
	stHuman[i].x = (gsWorld + stShip.x + (stBitmap[SHIPR].cx/2)) % PLANET_SIZE;

	/* Has the human hit the ground yet? */
	if (stHuman[i].y < height[stHuman[i].x])
	{
		/* The human can reestablish on the ground */
		stHuman[i].eState = LANDED;
		stHuman[i].y = height[stHuman[i].x] - HUMAN_YOFFSET;
		stHuman[i].fTargetted = FALSE;

		/* The ship no longer carries anything */
		stShip.sCarrying = NO_CARRY;
		IncScore (DROPPED_HUMAN_SCORE);
	   sprintf (szString, "%d", DROPPED_HUMAN_SCORE);
	   DisplayText (szString, (SCREENX(stHuman[i].x) - 2 * gsCharcx), 
	             (stHuman[i].y + stBitmap[HUMAN].cy), CLR_PINK, FALSE, FALSE);
	}
	
	/* Calculate the new radar position and draw marker in cyan */
	stHuman[i].rx = RADARX(stHuman[i].x);
	stHuman[i].ry = RADARY(stHuman[i].y);
	DrawMarker (stHuman[i].rx, stHuman[i].ry, CLRCYAN, FALSE);
	
	/* Draw the human if it is still on screen */
	stHuman[i].screenx = SCREENX(stHuman[i].x);
	Draw (stHuman[i].screenx, stHuman[i].y, FALSE, HUMAN, TRUE,
															 FALSE, i, gsMarker);
}			

/**************************************************************************
*
* FUNCTION NAME: MoveLand
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
VOID MoveLand (BOOL fForceDraw)
{
	PPOINTL  pDrawData;
	INT      i;


	/*
	** Need to find out which index in the land array represents the
	** point which is just off the screen at the left side - this is the
	** first point to be plotted (even though its off screen)
	*/

	/* land is scrolling to the left */
	if (gsWorldShift > 0)
	{
	   while ((land[gsLandIndex+1].x <= gsWorld) ||
						(land[gsLandIndex+1].x - gsWorld > MAX_UNDULATION))
		{
			/* If we're in the trailer part then readjust */
	   	if (++gsLandIndex >= (gusLandPoints + HEADER_POINTS))
			{
				gsLandIndex -= gusLandPoints;
			}
		}
	}
	/* land is scrolling to the right */
	else if (gsWorldShift < 0)
	{
	   while ((land[gsLandIndex].x > gsWorld) ||
						(gsWorld - land[gsLandIndex].x > MAX_UNDULATION))
		{
		   /* If we're in the header part then readjust */
		   if (--gsLandIndex < HEADER_POINTS)
		   {
			   gsLandIndex += gusLandPoints;
		   }
		}
	}

	/* Finally create the point array for use by the drawing routine */
	if (gsWorldShift == 0 && !fForceDraw)
	{
		/* If the world hasn't moved then no need to erase anything! */
		stLandDraw.lErasePts = 0;
	}
	else
	{
	   /* Switch the pointers to the erase and draw data */
		pDrawData = stLandDraw.pEraseData;
		stLandDraw.pEraseData = stLandDraw.pDrawData;
		stLandDraw.pDrawData = pDrawData;
		stLandDraw.lErasePts = stLandDraw.lDrawPts;

		/* Fill in the draw array */
		stLandDraw.lDrawPts = 0;
		i = gsLandIndex;
		do
		{
		   pDrawData->x = land[i].x - gsWorld;
		   (pDrawData++)->y = land[i++].y;
			stLandDraw.lDrawPts++;
		} while (land[i-1].x < gsWorld + SCREEN_WIDTH);
	}
}

