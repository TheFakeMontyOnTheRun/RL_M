/* Copyright 2017 Gibbon aka 'atsb'

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <setjmp.h>
#include <curses.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#if defined WINDOWS
#include <io.h>
#endif

#if defined NIX || NIX_LOCAL
#include <unistd.h>
#include <fcntl.h>
#endif

#include "../includes/bill.h"
#include "config/larncons.h"
#include "config/data.h"
#include "templates/math.t.hpp"
#include "terminal/term.hpp"
#include "../includes/inventory.h"
#include "../includes/scores.h"
#include "../includes/sysdep.h"
#include "../includes/io.h"
#include "strings/utf8.h"

using std::cout;

struct scofmt  		/*  This is the structure for the scoreboard        */
{
    int score;			/* the score of the player                          */
    int what;			/* the number of the monster that killed player     */
    int level;			/* the level player was on when he died             */
    int order;			/* the relative ordering place of this entry        */
    char who[LOGNAMESIZE];	/* the name of the character                        */
    int sciv[26][2];		/* this is the inventory list of the character      */
};

struct wscofmt  		/* This is the structure for the winning scoreboard */
{
    int score;			/* the score of the player                          */
    int timeused;			/* the time used in mobuls to win the game          */
    long taxes;			/* taxes he owes to LRS                             */
    int order;			/* the relative ordering place of this entry        */
    int hasmail;			/* 1 if mail is to be read, 0 otherwise */
    char who[LOGNAMESIZE];	/* the name of the character                        */
};

struct log_fmt  		/* 102 bytes struct for the log file                */
{
    long score;			/* the players score                                */
    time_t diedtime;		/* time when game was over                          */
    int cavelev;			/* level in caves                                   */
    int diff;			/* difficulty player played at                      */
    int elapsedtime;		/* real time of game in seconds                     */
    int bytout;			/* bytes input and output                           */
    int bytin;
    int moves;			/* number of moves made by player                   */
    int ac;			/* armor class of player                            */
    int hp, hpmax;		/* players hitpoints                                */
    int killed,
        spused;		/* monsters killed and spells cast                  */
    int usage;			/* usage of the cpu in %                            */
    int lev;			/* player level                                     */
    char who[12];			/* player name                                      */
    char what[46];		/* what happened to player                          */
};

static int sortboard ( void );
static void newscore ( int, char *, int, int );
static void new1sub ( int, int, char *, int );
static void new2sub ( int, int, char *, int );
static int readboard ( void );
static int writeboard ( void );
static struct scofmt
    sco[SCORESIZE];	/* the structure for the scoreboard  */
static struct wscofmt
    winr[SCORESIZE];	/* struct for the winning scoreboard */

const char *whydead[31] = {
    "",
    "",
    "self - annihilated",
    "shot by an arrow",
    "hit by a dart",
    "fell into a pit",
    "fell into a bottomless pit",
    "a winner",
    "trapped in solid rock",
    "",
    "",
    "",
    "",
    "failed",
    "erased by a wayward finger",
    "fell through a bottomless trap door",
    "fell through a trap door",
    "drank some poisonous water",
    "fried by an electric shock",
    "slipped on a volcano shaft",
    "",
    "attacked by a revolting demon",
    "hit by own magic",
    "demolished by an unseen attacker",
    "fell into the dreadful sleep",
    "killed by an exploding chest",
    "",
    "annihilated in a sphere",
	"starved to death",
    "",
    ""
};

/*
*  readboard()     Function to read in the scoreboard into a static buffer
*
*  returns -1 if unable to read in the scoreboard, returns 0 if all is OK
*/
static int
readboard ( void )
{
    FILE *pFile;
    int b;
    pFile = fopen ( scorefile, "rb" );

    if ( pFile == NULL ) {
        cout << "ERROR: scorefile is not readable \n";
        lflush();
        return ( -1 );
    }

    b = fread ( sco, sizeof ( struct scofmt ) * SCORESIZE, 1, pFile );

    if ( b != 1 ) {
        cout << "ERROR: Loosers scoreboard is not readable\n";
        fclose ( pFile );
        return ( -1 );
    }

    b = fread ( winr, sizeof ( struct wscofmt ) * SCORESIZE, 1, pFile );

    if ( b != 1 ) {
        cout << "ERROR: Winners scoreboard is not readable\n";
        fclose ( pFile );
        return ( -1 );
    }

    fclose ( pFile );
    return 0;
}

/*
*  writeboard()    Function to write the scoreboard from readboard()'s buffer
*
*  returns -1 if unable to write the scoreboard, returns 0 if all is OK
*/
static int
writeboard ( void )
{
    FILE *pFile;
    int b;
    set_score_output();
    pFile = fopen ( scorefile, "wb" );

    if ( pFile == NULL ) {
        cout << "ERROR: scorefile is not writable \n";
        lflush();
        return ( -1 );
    }

    b = fwrite ( sco, sizeof ( struct scofmt ) * SCORESIZE, 1, pFile );

    if ( b != 1 ) {
        cout << "ERROR: Loosers scoreboard is not writable\n";
        fclose ( pFile );
        return ( -1 );
    }

    b = fwrite ( winr, sizeof ( struct wscofmt ) * SCORESIZE, 1, pFile );

    if ( b != 1 ) {
        cout << "ERROR: Winners scoreboard is not writable\n";
        fclose ( pFile );
        return ( -1 );
    }

    fclose ( pFile );
    return 0;
}

/*
*  makeboard()         Function to create a new scoreboard (wipe out old one)
*
*  returns -1 if unable to write the scoreboard, returns 0 if all is OK
*/
int
makeboard ( void )
{
    int i;

    for ( i = 0; i < SCORESIZE; i++ ) {
        winr[i].taxes = winr[i].score = sco[i].score = 0;
        winr[i].order = sco[i].order = ( short ) i;
    }

    if ( writeboard() ) {
        cout << "ERROR: unable to write a new scoreboard\n";
        return ( -1 );
    }

    /* Why bother redefining a function?  Just use standard *NIX functions
    	and be done with it. Windows won't even need to set permissions anyway
    	so no need for 'cross platform' here. Sheesh.. ~Gibbon
    */
#if defined NIX || NIX_LOCAL
    chmod ( scorefile, 0666 );
#endif
    return ( 0 );
}

void
checkmail ( void )
{
    int i, gold, taxes;
    if ( readboard () < 0 ) {
        return;  /* can't find scoreboard */
    }
    /* search through winners scoreboard */
    for ( i = 0; i < SCORESIZE;
            i++ ) /* search through winners scoreboard */
        if ( utf8cmp ( winr[i].who, logname ) == 0 && winr[i].score > 0
                && winr[i].hasmail ) {
            winr[i].hasmail = 0;
            gold = taxes = winr[i].taxes;
            writeboard ();

            /* Intuit the amount of gold -- should have changed
             * the score file, but ...  TAXRATE is an fraction.
             */
            while ( ( gold * TAXRATE ) < taxes ) {
                gold += taxes;
            }

            larn_read_junk_mail(gold);
        }
}

/*
*  int paytaxes(x)         Function to pay taxes if any are due
*
*  Enter with the amount (in gp) to pay on the taxes.
*  Returns amount actually paid.
*/

int
paytaxes ( int x )
{
    int i, amt;

    if ( x < 0 ) {
        return ( 0L );
    }

    if ( readboard () < 0 ) {
        return ( 0L );
    }

    /* search for a winning entry for the player */
    for (i=0; i<SCORESIZE; i++)
        if (utf8cmp(winr[i].who, logname) == 0)
            if (winr[i].score > 0) {
                amt = winr[i].taxes;
                if (amt > x) amt = x;
                winr[i].taxes -= amt;
                writeboard();
                return amt;
            }
    return 0;
}

/*
*  showscores()        Function to show the scoreboard on the terminal
*
*  Returns nothing of value
*/

void
showscores (int scoreflag)
{
    int count, i, j;

    lstandout("Score  Time   Winners\n");

    count = 0;

    for (i=0; i<SCORESIZE; i++) for (j=0; j<SCORESIZE; j++) if (winr[j].order == i) {
                if (winr[j].score) {
                    count++;
                    lprintf("%-10d      %5d Mobuls     %s\n", winr[j].score, winr[j].timeused, winr[j].who);
                }
                break;
            }

    lprc('\n');

    lstandout("Score    Died\n");

    count = 0;

    for (i=0; i<SCORESIZE; i++) for (j=0; j<SCORESIZE; j++) if (sco[j].order == i) {
                if (sco[j].score) {
                    count++;

                    lprintf("%-10d  %s", sco[j].score, sco[j].who);

                    if (sco[j].what < 256) {
                        const char *p = monster[sco[j].what].name;

                        lprintf("killed by %s %s",((*p == 'a' || *p == 'e' || *p == 'i' || *p == 'o' || *p == 'u') ? "an" : "a"), p);
                    } else lprintf("%s", whydead[sco[j].what - 256]);

                    lprintf(" on level %s\n", levelname[sco[j].level]);

                    if (scoreflag) {
                        int k, n;

                        for (k=1; k < MAXOBJECT; k++) for (n=0; n < MAXINVEN; n++) if (k == sco[j].sciv[n][0]) {

                                    lprintf("%c) %s", 'a' + n, objectname[sco[j].sciv[n][0]]);

                                    switch (sco[j].sciv[n][0]) {
                                    case OPOTION:
                                        lprintf("%s", potionname[sco[j].sciv[n][1]]);
                                        break;

                                    case OSCROLL:
                                        lprintf("%s", scrollname[sco[j].sciv[n][1]]);
                                        break;

                                    case ODIAMOND:
                                    case ORUBY:
                                    case OEMERALD:
                                    case OSAPPHIRE:
                                    case OCHEST:
                                    case OBOOK:
                                    case OCOOKIE:
                                    case OLARNEYE:
                                    case OSPIRITSCARAB:
                                    case OCUBEofUNDEAD:
                                    case ONOTHEFT:
                                        break;

                                    default:
                                        if (sco[j].sciv[n][1] > 0) lprintf(" +%d", sco[j].sciv[n][1]);
                                        else if (sco[j].sciv[n][1] < 0) lprintf( " %d", sco[j].sciv[n][1]);
                                        break;
                                    }

                                    lprc('\n');
                                }
                    }
                }
                break;
            }
}

/*
*  sortboard()     Function to sort the scoreboard
*
*  Returns 0 if no sorting done, else returns 1
*/
static int
sortboard ( void )
{
    int i, pos, j = 0, jdat;

    for (i=0; i<SCORESIZE; i++) sco[i].order = winr[i].order = -1;
    pos = 0;
    while (pos < SCORESIZE) {
        jdat = 0;
        for (i=0; i<SCORESIZE; i++) if ((sco[i].order < 0) && (sco[i].score >= jdat)) {
                j = i;
                jdat = sco[i].score;
            }
        sco[j].order = pos++;
    }
    pos = 0;
    while (pos < SCORESIZE) {
        jdat = 0;
        for (i=0; i<SCORESIZE; i++) if ((winr[i].order < 0) && (winr[i].score >= jdat)) {
                j = i;
                jdat = winr[i].score;
            }
        winr[j].order = pos++;
    }
    return 1;
}

/*
*  newscore(score, whoo, whyded, winner)   Function to add entry to scoreboard
*      int score, winner, whyded;
*      char *whoo;
*
*  Enter with the total score in gp in score,  players name in whoo,
*      died() reason # in whyded, and TRUE/0 in winner if a winner
*  ex.     newscore(1000, "player 1", 32, 0);
*/
static void
newscore ( int score, char *whoo, int whyded, int winner )
{
    int i, taxes;

    /* if a winner then delete all non-winning scores */
    if (winner) {
        for (i=0; i<SCORESIZE; i++) if (utf8cmp(sco[i].who, logname) == 0) sco[i].score = 0;

        taxes = score*TAXRATE;
        score += 100000; /* bonus for winning */

        /* if he has a slot on the winning scoreboard update it if greater score */
        for (i=0; i<SCORESIZE; i++) if (utf8cmp(winr[i].who, logname) == 0) {
                new1sub(score, i, whoo, taxes);
                return;
            }

        /* he had no entry. look for last entry and see if he has a greater score */
        for (i=0; i<SCORESIZE; i++) if (winr[i].order == SCORESIZE-1) {
                new1sub(score, i, whoo, taxes);
                return;
            }
    } else { /* for not winning scoreboard */
        /* if he has a slot on the scoreboard update it if greater score */
        for (i=0; i<SCORESIZE; i++) if (utf8cmp(sco[i].who, logname) == 0) {
                new2sub(score, i, whoo, whyded);
                return;
            }

        /* he had no entry. look for last entry and see if he has a greater score */
        for (i=0; i<SCORESIZE; i++) if (sco[i].order == SCORESIZE-1) {
                new2sub(score, i, whoo, whyded);
                return;
            }
    }
}

/*
*  new1sub(score,i,whoo,taxes)       Subroutine to put player into a
*      int score,i,whyded,taxes;         winning scoreboard entry if his score
*      char *whoo;                       is high enough
*
*  Enter with the total score in gp in score,  players name in whoo,
*      died() reason # in whyded, and TRUE/0 in winner if a winner
*      slot in scoreboard in i, and the tax bill in taxes.
*  Returns nothing of value
*/
static void
new1sub ( int score, int i, char *whoo, int taxes )
{
    struct wscofmt *p;

    p = &winr[i];
    p->taxes += taxes;

    if ((score >= p->score)) {
        utf8cpy(p->who, whoo);
        p->score = score;
        p->timeused = gtime/100;
        p->hasmail = 1;
    }
}

/*
*  new2sub(score,i,whoo,whyded)          Subroutine to put player into a
*      int score,i,whyded,taxes;         non-winning scoreboard entry if his
*      char *whoo;                       score is high enough
*
*  Enter with the total score in gp in score,  players name in whoo,
*      died() reason # in whyded, and slot in scoreboard in i.
*  Returns nothing of value
*/
static void
new2sub ( int score, int i, char *whoo, int whyded )
{
    int j;
    struct scofmt *p;

    p = &sco[i];

    if ((score >= p->score)) {
        utf8cpy(p->who, whoo);
        p->score = score;
        p->what = whyded;
        p->level = level;

        for (j=0; j<26; j++) {
            p->sciv[j][0] = iven[j];
            p->sciv[j][1] = ivenarg[j];
        }
    }
}

/*
*  died(x)     Subroutine to record who played larn, and what the score was
*      int x;
*
*  if x < 0 then don't show scores
*  died() never returns! (unless cdesc[LIFEPROT] and a reincarnatable death!)
*
*      < 256   killed by the monster number
*      256     quit
*      257     suspended
*      258     self - annihilated
*      259     shot by an arrow
*      260     hit by a dart
*      261     fell into a pit
*      262     fell into a bottomless pit
*      263     a winner
*      264     trapped in solid rock
*      265     killed by a missing save file
*      266     killed by an old save file
*      267     caught by the greedy cheater checker trap
*      268     killed by a protected save file
*      269     killed his family and killed himself
*      270     erased by a wayward finger
*      271     fell through a bottomless trap door
*      272     fell through a trap door
*      273     drank some poisonous water
*      274     fried by an electric shock
*      275     slipped on a volcano shaft
*      276     killed by a stupid act of frustration
*      277     attacked by a revolting demon
*      278     hit by his own magic
*      279     demolished by an unseen attacker
*      280     fell into the dreadful sleep
*      281     killed by an exploding chest
*      282     killed by a missing maze data file
*      283     killed by a sphere of annihilation
*      284     died a post mortem death
*      285     malloc() failure
*      300     quick quit -- don't put on scoreboard
*/

void
died ( int x )
{
    int win = 0;
    if (cdesc[LIFEPROT] > 0) {
        switch (x) {
        case 262:
        case 263:
        case 269:
        case 271:
            goto cantprotect;
        };

        --cdesc[LIFEPROT];
        cdesc[HP] = cdesc[HPMAX];
        --cdesc[CONSTITUTION];
        cursors ();
        fl_display_message ( "\nYou feel wiiieeeeerrrrrd all over! " );
        lflush ();
        nap ( 3000 );
        return;     /* only case where died() returns */
    }

cantprotect:
    nap ( 3000 );
    cursors ();
    fl_display_message ( "\nPress any key to continue. " );
    ttgetch ();
    lflush ();
    screen_clear();
    enable_scroll = 0;

    cdesc[GOLD] += cdesc[BANKACCOUNT];
    cdesc[BANKACCOUNT] = 0;
    newscore(cdesc[GOLD], logname, x, win);
    if (x == 263) win = 1;

    if (WIZID == false)
        if (sortboard()) {
            writeboard();
        }
    lflush ();
    screen_clear();
    enable_scroll = 0;
    showscores (x);    /* if we updated the scoreboard */
    cursors ();
    fl_display_message ( "\nPress any key to exit. " );
    scbr ();
    ttgetch ();
    clearvt100 ();
    exit(EXIT_SUCCESS);
}
