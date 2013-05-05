.************************************************
.*
.*   Filename:      DEF.IPF
.*
.************************************************
.*
.************************************************
.*   GENERAL HELP
.************************************************
:userdoc.
:h1 res=1002.Help for Aquanaut!
:p.
:hp2.Aquanaut!:ehp2. is an OS/2 Presentation Manager
aquatic arcade game.
:p.
Assuming your machine has a sufficiently fast processor, this 
game plays faster than you could possibly hope for!  The pace of the game may
be slowed down through the :link refid=GAME_SPEED reftype=hd.Game Speed:elink.
setting.
:p.
Related Information&colon.
:ul compact.
:li.:link refid=OBJECT_OF_THE_GAME reftype=hd.Object of the Game:elink.
:li.:link refid=PLAYING_THE_GAME reftype=hd.Playing the Game:elink.
:li.:link refid=REGISTRATION reftype=hd.Registering Aquananut!:elink.
:li.:link refid=WAVES reftype=hd.Waves:elink.
:li.:link refid=YOUR_SHIP reftype=hd.Your Sub:elink.
:li.:link refid=KNOWING_YOUR_ENEMY reftype=hd.Knowing your enemy:elink.
:li.:link refid=SCORING reftype=hd.Scoring:elink.
:li.:link refid=SOUND reftype=hd.Sound Effects:elink.
:li.:link refid=MUSIC reftype=hd.Background Music:elink.
:li.:link refid=HELPFUL_HINTS reftype=hd.Helpful Hints:elink.
:eul.
.**********************************
:h2 res=1011.Start
:i1.Start
:p.Selecting this option from the menu, or by pressing :hp2.F2:ehp2. starts
the game.
.**********************************
:h2 res=1012.Stop
:i1.Stop
:p.Selecting this option from the menu, or by pressing :hp2.F4:ehp2. aborts 
the current game but does not exit the program.
.**********************************
:h2 res=1014.Pause
:i1.Pause
:p.Selecting this option from the menu, or by pressing :hp2.F5:ehp2. pauses
the game.  The option is now checked on the menu.  Selecting this option 
a second time resumes the game.  If the game window loses focus or is 
minimized, the game will also be paused.
.**********************************
:h2 res=1013.Exit
:i1.Exit
:p.Selecting this option from the menu, or by pressing :hp2.F3:ehp2. exits
the program.
.**********************************
:h2 res=1015.Enemy Bullets
:i1.Enemy bullets
:p.Selecting this option from the menu, or by pressing :hp2.Ctrl-B:ehp2. 
toggles the enemy bullets option.  If this item is checked (this is the 
default) then enemy bullets will be fired at your sub by some of your
enemies.  If it is not checked, no enemy bullets will be fired.
.**********************************
:h2 res=1016.Submarines
:i1.Submarines
:p.Selecting this option from the menu allows the starting number of 
submarines to be set to either 3 (the default), 4 or 5.
.**********************************
:h2 res=1017.Scatter bombs
:i1.Scatter bombs
:p.Selecting this option from the menu allows the starting number of scatter 
bombs to be set to either 3 (the default), 5 or 7.
.**********************************
:h2 res=1018.Frame Controls
:i1.Frame Controls
:p.Selecting this option from the menu, or by pressing :hp2.Ctrl-F:ehp2. 
toggles between frame controls being shown and frame controls being hidden.
Some people may find the frame controls distracting and therefore wish to
hide them.  Note that once hidden, they can only be redisplayed by
pressing :hp2.Ctrl-F:ehp2.
.**********************************
:h2 res=1019.Object Size
:i1.Object Size
:p.Selecting this option from the menu, or pressing :hp2.Ctrl-O:ehp2. brings
up a dialog box which allows the size of the screen objects (your sub, all
nastie lifeforms, bullets etc) to be changed.  
This option is really quite silly, since the default size works very well.  
However, if your eyes are 
tired after a hard days night then make those nasties truly enormous!  
Conversely, if you want to get a bit of sharp shooting practice, then scale 
the targets down.
:p.Whatever change you make will be saved and reused the next time that you
run the program.
.**********************************
:h2 res=1020.High Score Table
:i1.High Score Table
:p.Selecting this option from the menu brings
up a dialog box containing a notebook, each page of which contains data
about an entry in the high score table.  This data includes the score, name,
date on which the score was achieved, the number of nasties killed, the hit 
rate (the percentage of laser shots which killed nasties) and the wave 
reached.  It also lists the configuration parameters which applied to that
game.
.**********************************
:h2 res=1021.Sound
:i1.Sound
:p.If MMPM/2 (MultiMedia extensions for OS/2) is installed on your machine
this option is available.  If this item is checked (this is the default)
then sound will be heard.  If sound is undesirable, then either disconnect
your speakers or click on this item.
.**********************************
:h2 res=1022.Sonar
:i1.Sonar
:p.Selecting this option from the menu allows the sonar update to be
modified to one of three modes:
:dl compact break=all.
:dt.:hp2.Sweep/Fade:ehp2.
:dd.The sonar sweeps across the screen lighting up objects for a short time
before they fade away.  This is the default - it almost looks like a 
:hp2.REAL:ehp2. sonar!
:dt.:hp2.Sweep/Persist:ehp2.
:dd.The sonar still sweeps but objects do not fade.
:dt.:hp2.No Sweep:ehp2.
:dd.The annoying traversal of the sonar by a nasty looking green line is
disabled!
:edl.
:p.If you're finding the game too easy, then set the sonar to Sweep/Fade and
then see how you get on!
.**********************************
:h2 res=1023 id=REGISTRATION.Registration
:i1.Registration
:p.There are several reasons why you should register this game:
:ul.
:li.You can finally disable the annoying box which pops up every time you
play the game.
:li.You will encounter some new monsters and other nice features which are 
disabled in the unregistered game (wait till you see the mermaid...).  These
are well worth seeing!
:li.After much debate I decided to allow an unregistered user to save all the
settings.  This act of generosity deserves reward!
:li.I believe I deserve some reward for some painful hours spent on borrowed
computers in borrowed time to get this game ready.  You never know, if the
response is good enough, I may consider "Aquananut 2!" (where you have to
prevent electric eels from picking up anchors from the bottom of the ocean...)
or maybe even an original idea or two (surely not!).
:eul.
:p.To register send your registration name together with a check or money 
order for $10 (US) to:
:p.
:lines align=center.
:hp8.
Paul Stanley,
2003 N. Swinton Avenue,
Delray Beach,
Florida 33444.
:ehp8.
:elines.
:p.
:p.In return you shall receive your ID, my thanks and if you request it some
technical information and code snippets of some of the more interesting bits
of Presentation Manager programming used to implement Aquanaut!  A 
programmers guide if you will.  OS/2 deserves applications, so why not have
a go yourself?
.**********************************
:h2 res=1024.Mouse
:i1.Mouse
:p.Your craft may be controlled by moving the mouse.  Up and down, left and
right move the sub in the appropriate direction.  The left button fires your
laser, and the right button fires a smart bomb.  Click on this option or
press CTRL-M to enable the mouse.  Note that the keyboard is still used to
perform an aquawarp.
Note also, that you will not
be able to access the menu bar during a game since the mouse pointer is
hidden.  To show the pointer press CTRL-M to disable the mouse once more.
.**********************************
:h2 res=1025.Music
:i1.Music
:p.This option allows music to be enabled or disabled during the game.  See
:link refid=MUSIC reftype=hd.Background Music:elink. to find out how to
change the background tune.
.**********************************
:h2 res=1026.Bubbles
:i1.Bubbles
:p.This option allows bubbles to be turned on or off.  The bubbles may be
too distracting for some, or take up to much processing time for those
people with slower machines.
.**********************************
:h2 res=1027 id=DEMO_PLAY.Demo Play
:i1.Demo Play
:p.This option allows a demo which was previously recorded (through the
:link refid=DEMO_RECORD reftype=hd.Demo Record:elink. function) to be played.
The demo is played repeatedly until :hp2.Stop:ehp2. is selected 
(or :hp2.F4:ehp2. is pressed).
.**********************************
:h2 res=1028 id=DEMO_RECORD.Demo Record
:i1.Demo Record
:p.This option allows a demo game to be recorded.  Once this option is 
selected the game will start and every action is recorded until the game is
over.  The game can be played back by using the 
:link refid=DEMO_PLAY reftype=hd.Demo Play:elink. function.
.**********************************
:h2 res=1008 id=GAME_SPEED.Game Speed
:i1.Game Speed
:p.This control can be accessed at any time by pressing CTRL-S or from the
:hp2.Options:ehp2. menu item.  It has been provided for two reasons:
:ol compact.
:li.The pace can be altered to suit the processor you are running on
:li.If you can't handle the heat...
:eol.
:p.The slider can be moved to very the pace.  Once the :hp2.OK:ehp2. button
has been pressed the pace is set and will be used every time the game is run.
.**********************************
:h2 res=1010 id=KEYS_DLG.Keys Selection
:i1.Keys Selection
:p.This dialog can be accessed at any time by pressing CTRL-K or from the
:hp2.Options:ehp2. menu item.  It allows the game keys to be defined.
:p.Click on the radio button corresponding to the key that you wish to change.
Then press the desired key for that function.  Once the :hp2.OK:ehp2. button
has been pressed the keys are defined and will be used every time the game is 
run.
:p.The keys providing lateral movement can be defined in two ways.
:dl compact break=all.
:dt.:hp2.Turn/Thrust:ehp2.
:dd.This is the default.  If defined this way the turn key toggles the
direction of the submarine between facing left and right.  The thrust key
then provides thrust in the direction that the sub is facing.
:dt.:hp2.Left/Right:ehp2.
:dd.Pressing either left or right keys turns the sub if it is not currently 
pointing in the direction requested, and provides thrust in that direction.
:edl.
.**********************************
:h2 res=1009.Keys Help
:i1.Control Keys
:p.The control keys are fully definable through the
:link refid=KEYS_DLG reftype=hd.Keys:elink. option found under the 
:hp2.Options:ehp2. menu item.
:p.The keys, their default values and their usage are as follows:
:dl compact break=all.
:dt.:hp2.Up:ehp2.  (A)
:dd.Moves the submarine upwards
:dt.:hp2.Down:ehp2.  (Z)
:dd.Moves the submarine downwards
:dt.:hp2.Turn:ehp2.  (Space)
:dd.Toggles the submarine between left-facing and right-facing
:dt.:hp2.Thrust:ehp2.  (,)
:dd.Fires the engines to make the submarine move over the ocean floor
:dt.:hp2.Fire:ehp2.  (.)
:dd.Fires a laser bolt
:dt.:hp2.Scatter Bomb:ehp2.  (/)
:dd.Fires a scatter bomb, destroying all enemies currently on screen
:dt.:hp2.Aquawarp:ehp2.  (Shift(L))
:dd.Aquawarps the sub to a new location on the sea's bottom.
:edl.
:p.The 
:link refid=KEYS_DLG reftype=hd.Keys:elink. option also allows the turn/thrust
functions to be changed to left/right.
:p.Other function keys and CTRL key sequences activate menu bar items.
:p.The mouse may also be used to control the sub.
.**********************************
:h2 id=OBJECT_OF_THE_GAME.Object of the Game
:i1.Object of the game
:p.There is no ultimate goal in this game.  The object is
basically to stay alive as long as possible, amass as high
a score as possible and protect the sea's treasures for as long
as possible.
:p.The game stores the 10 highest scores, which are saved
each time the game is run.  The object is therefore to
have your name entered at the number 1 position!
.**********************************
:h2 id=PLAYING_THE_GAME.Playing the Game
:i1.Playing the game
:p.The ocean is under attack from various nasties who are bent on
capturing the treasure which poor wretches have lost over the centuries
when their ships went under.
It is your job as :hp2.Aquanaut!:ehp2.
to protect the treasure, and prevent it from being taken up to the surface
of the ocean by scary, ferocious and silly-looking giant Seahorses.
:p.You control a strange little submarine, scuttling around beneath the 
ocean's waves.  Scattered over the ocean's bottom are chests, bountiful in
all manner of gold and rubies.
You guide your sub around the ocean floor hunting down the Seahorses (see 
:link refid=KNOWING_YOUR_ENEMY reftype=hd.Knowing your enemy:elink.)
and destroying them before they capture your treasure.
:p.To make this job easier, the sonar at the top of the screen
shows the positions of all nasties and treasure, and is sophisticated
enough to display each in a different color!  Not only that, it has an
authentic 'beep' (if you're lucky enough to possess a sound card)!
:p.After a treasure chest has been picked up by a Seahorse, it may still be 
saved by a carefully aimed shot at the fleeing monster.  The chest
will then slowly sink to the ocean floor.  If this fall is not too
far, the treasure chest will survive.  If it is further, you still have a
chance to save the treasure, by guiding your sub towards it to pick it
up.  Once held tightly under the metal skin of your craft, you may 
deposit it on the surface at your leisure.
:p.As well as Seahorses there are a variety of other monsters that impede
your progress and generally make life quite difficult.  Most of these
have the sole intention of trying to destroy your sub.
:p.There are a couple of sea creatures that are there to help you, but only
if you've registered the game!
:p.Should all the treasure get captured...now you're doomed!  Once
this happens the nasties get nastier, and your chances of surviving for long 
are slim.
.**********************************
:h2 id=WAVES.Waves
:i1.Waves
:p.The nasties come to your ocean in waves.  Once you have destroyed all
nasties in a wave, then you will receive bonus points for any treasure that
remains (see
:link refid=SCORING reftype=hd.Scoring:elink.).
After every third wave, more treasure will appear.
:p.Each wave of nasties is nastier than the last!  The enenmy bullets get
more frequent and more accurate, Demonic Seahorses become more demonic,
metal objects become harder to kill, and all nasties become more populous.
.**********************************
:h2 id=YOUR_SHIP.Your Sub
:i1.Your Sub
:p.Your sub is the best available, and don't you forget it!  It
coasts effortlessly across the ocean floor and has the ability
to turn on a sixpence (or a dime).
:p.If your thruster is applied continuously, you will travel at a
dangerously fast speed, so monitor the sonar constantly to see what
lies ahead of you.
:p.You are armed with a powerful laser cannon which can fire multiple
beams of searing energy.  These beams will kill all nasties on contact,
and of course your own treasure too, so be careful where you aim!
:p.More devastating still is your barrage of scatter bombs.  You have a 
limited number of these at the start of the game but more can be picked up as 
the game progresses (see
:link refid=SCORING reftype=hd.Scoring:elink.).  These bombs instantly
destroy anything close to your sub (suspiciously, anything currently on the
screen).  However, they do not destroy the treasure due to their frequency of
vibration and nasty-molecular size, or something.  They do not destroy the 
U-Boats or Torpedo's, but they do damage them.
:p.If you're really in a tight spot, you can :hp2.aquawarp.:ehp2. to another 
place of the ocean floor.  This may get you out of trouble, but may throw you 
into more danger.  You may be placed right on top of a bunch of monsters, or 
worse still you may just blow up on re-entry.  This is due to a faulty 
component in your sub which has a tendency to fail about 30% of the time...
:p.The only other aid to your survival is the  Rubber-Ring.  You'll have to
work out how to use this yourself...
.**********************************
:h2 id=KNOWING_YOUR_ENEMY.Knowing your enemy
:i1 id=enemy.Knowing your enemy
:p.The following describes the different nasties that you will encounter:
:dl.
:i2 refid=enemy.Seahorse
:dt.:hp2.Seahorse:ehp2.
:dd.These little green guys look pretty harmless to me, but don't be lulled
into a false sense of security.
At the start of each wave, and 
periodically throughout a wave, more Seahorses will descend to the ocean
floor and hunt out the treasure chests.  Once they find some treasure they 
fancy, they grab it and carry it away to the ocean surface (which is luckily 
off-screen - didn't fancy animating those waves!).  Should they
succeed in their mission, they will immediately become possessed and 
become a...
:i2 refid=enemy.Demonic Seahorse
:dt.:hp2.Demonic Seahorse:ehp2.
:dd.Looking exactly like Seahorses (except that they're red, have horns and a
forked tail!), Demonic Seahorses have 
but one goal in life - to crash into your sub.  Haphazardly,
they head towards your sub, ducking and diving to make life difficult for
you.
:i2 refid=enemy.U-Boat
:dt.:hp2.U-Boat:ehp2.
:dd.These unimaginative looking craft trudge slowly near the surface of the
ocean dropping a trail of depth charges in their wake.
If you hit one of these charges it will destroy your craft - so don't do it!
The thick shell of the U-Boat requires several well-aimed shots to penetrate
it.
:i2 refid=enemy.Jellyfish
:dt.:hp2.Jellyfish:ehp2.
:dd.These slow moving monsters are harmless enough.  They do not fire at you
and do not move fast enough to collide into you, but when they are hit their
gelatinous body splits open and out pop 4 of these guys...
:i2 refid=enemy.Baby Jellyfish
:dt.:hp2.Baby Jellyfish:ehp2.
:dd.Baby Jellyfish move around in groups of 4 in a fast, dangerous formation.  They
are very small, and hence difficult to shoot down.  If they don't collide with
your sub, they're likely to hit it with a well-aimed shot.
:i2 refid=enemy.Torpedo
:dt.:hp2.Torpedo:ehp2.
:dd.Torpedoes only appear when a wave has gone on for some time.  They are
dropped from some invisible ship or something to try and finish you off.  
They're extremely fast, and being quite shallow are difficult to hit.  At least
they're kind enough to stop now and again to give you a chance to home in on
them.  A scatter bomb may be required to finish off these guys.  Like the 
U-Boats, a torpedo will not be destroyed by a single shot.
:i2 refid=enemy.Mermaid
:dt.:hp2.Mermaid (Registered game only):ehp2.
:dd.No details here, sorry! - read the information about the Mermaid that came
with your registration.
:i2 refid=enemy.Rubber Ring
:dt.:hp2.Rubber Ring (Registered game only):ehp2.
:dd.No details here, sorry! - read the information about the Rubber Ring 
that came with your registration.
:edl.
.**********************************
:h2 id=SCORING.Scoring
:i1.Scoring
:p.You will receive the following points for destroying each of the 
following nasties:
:dl compact break=all.
:dt.:hp2.Seahorse:ehp2.
:dd.100
:dt.:hp2.Demonic Seahorse:ehp2.
:dd.150
:dt.:hp2.U-Boat:ehp2.
:dd.100
:dt.:hp2.Jellyfish:ehp2.
:dd.200
:dt.:hp2.Baby Jellyfish:ehp2.
:dd.150
:dt.:hp2.Torpedo:ehp2.
:dd.300
:dt.:hp2.Mermaid:ehp2.
:dd.+/- 1000
:dt.:hp2.Rubber Ring:ehp2.
:dd.+/- 200
:edl.
:p.Points are also awarded for the following:
:dl compact break=all.
:dt.:hp2.Catching a falling treasure chest:ehp2.
:dd.250
:dt.:hp2.Returning treasure to the planet:ehp2.
:dd.250
:dt.:hp2.Each surviving chest at wave end:ehp2.
:dd.100
:edl.
:p.At every 10000 points you will be awarded with a bonus life and a
bonus scatter bomb.
.**********************************
:h2 id=SOUND.Sound Effects
:i1.Sound Effects
:p.This game is MMPM/2 enabled, which means that if you've got a suitable
sound card, then you're going to hear some whizzy sound effects!
:p.If you're not so fortunate then sound can easily be added by the 
adventurous (and sad) gamester!
:p.The most important sound to make is "sssssssswwwwwwooooosssssshhhhh" when
Seahorses appear on the screen.  Once you have
mastered this then the "pppppppwwwwoooooh, pppppppwwwwoooooh, 
pppppppwwwwoooooh" sound of your laser (and accompanying white noise when
it lands squarely on the shoulders of the target nasty) should follow
with little problem.
.**********************************
:h2 id=MUSIC.Background Music
:i1.Background Music
:p.This game is MMPM/2 enabled, which means that if you've got a suitable
sound card, then you're going to hear some classy tune or other as the game
is played.  The music is read from MIDI file AQUANAUT.MID, hence if you
don't like the provided tune then rename your own favorite MIDI file 
AQUANAUT.MID and hey presto!
:p.The music may be turned off from the options menu if it's beginning to
rattle your nerves!
.**********************************
:h2 id=HELPFUL_HINTS.Helpful Hints
:i1.Hints
:ul.
:li.Keep moving - enemy bullets are targeted at your sub, so if you're 
stationary, you're a sitting duck!
:li.At least on the early waves, nasties will not shoot at you if you're 
close by, so getting in close to Baby Jellyfish is pretty safe!
:li.Save scatter bombs for emergencies, or for when there are many nasties on
screen.
:li.Don't scatter bomb Jellyfish - they will disintegrate and release 4 more 
deadly Baby Jellyfish.  Instead, shoot the Jellyfish and immediately follow up 
with a scatter bomb to kill the Babies.
:li.Increase your score by letting Demonic Seahorses pick up treasure.  Then
shoot them and rescue the falling treasure - do it right and you're awarded
with 500 points.
:li.Cheat tip:  if an enemy bullet is heading your way and you can't avoid
it, press CTRL-B to turn off bullets!  Turn them on again afterwards of course.
:li.If you've registered the game then make the most of the Mermaid and 
Rubber Ring.  You need every help you can get, and these are just the job!
:eul.
.**********************************
:euserdoc.

