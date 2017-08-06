/* moreobj.c

Routines in this file:

oaltar()
othrone()
odeadthrone()
ochest()
ofountain()
fntchange()
fch()
drink_fountain()
wash_fountain()
enter()
remove_gems()
sit_on_throne()
up_stairs()
down_stairs()
open_something()
close_something()
desecrate_altar()
pray_at_altar()
*/

#include "../includes/action.h"
#include "../includes/main.h"
#include "config/data.h"
#include "templates/math.t.hpp"
#include "dungeon/dungeon.hpp"
#include "../includes/display.h"
#include "../includes/global.h"
#include "core/inventory.hpp"
#include "../includes/io.h"
#include "../includes/moreobj.h"
#include "../includes/object.h"
#include "../includes/spells.h"
#include "core/store.hpp"
#include "core/funcs.hpp"


static void fch ( int, long * );
static void specify_obj_nocurs ( void );
static void specify_obj_cursor ( void );
static void move_cursor ( int *, int *, int );

/*
* subroutine to process an altar object
*/
void
oaltar ( void )
{
    fl_display_message ( "\nDo you (p) pray  (d) desecrate" );
    iopts ();

    for ( ;; ) {
        switch ( ttgetch () ) {
        case 'p':
            fl_display_message ( " pray\nDo you (m) give money, (t) give thanks or (j) just pray? " );

            for ( ;; )
                switch ( ttgetch () ) {
                case 'j':
                    fl_display_message ( "\n" );
                    act_just_pray ();
                    return;

                case 't':
                    fl_display_message ( "\n" );
                    act_give_thanks();
                    fl_display_message("\nThe Altar descends into the floor and vanishes before your eyes!");
                    forget();
                    return;

                case 'm':
                    act_donation_pray ();
                    return;

                case '\33':
                    return;
                };

        case 'd':
            fl_display_message ( " desecrate" );
            act_desecrate_altar ();
            return;

        case 'i':
        case '\33':
            fl_ignore ();
            act_ignore_altar ();
            return;
        };
    }
}



/*
subroutine to process a throne object
*/
void
othrone ( int arg )
{
    fl_display_message ( "\nDo you (p) pry off jewels, (s) sit down" );
    iopts ();

    for ( ;; ) {
        for ( ;; ) {
            switch ( ttgetch () ) {
            case 'p':
                fl_display_message ( " pry off" );
                act_remove_gems ( arg );
                return;

            case 's':
                fl_display_message ( " sit down" );
                act_sit_throne ( arg );
                return;

            case 'i':
            case '\33':
                fl_ignore ();
                return;
            };
        }
    }
}



void
odeadthrone ( void )
{
    fl_display_message ( "\nDo you (s) sit down" );
    iopts ();

    for ( ;; ) {
        for ( ;; ) {
            switch ( ttgetch () ) {
            case 's':
                fl_display_message ( " sit down" );
                act_sit_throne ( 1 );
                return;

            case 'i':
            case '\33':
                fl_ignore ();
                return;
            };
        }
    }
}



/*
* subroutine to process a chest object
*/
void
ochest ( void )
{
    fl_display_message ( "\nDo you (t) take it, (o) try to open it" );
    iopts ();

    for ( ;; ) {
        switch ( ttgetch () ) {
        case 'o':
            fl_display_message ( " open it" );
            act_open_chest ( playerx, playery );
            return;

        case 't':
            fl_display_message ( " take" );

            if ( take ( OCHEST, object_argument[playerx][playery] ) == 0 ) {
                object_identification[playerx][playery] = been_here_before[playerx][playery] = 0;
            }

            return;

        case 'i':
        case '\33':
            fl_ignore ();
            return;
        };
    }
}



/*
* process a fountain object
*/
void
ofountain ( void )
{
    fl_termcap_cursor_position(1,24);
    fl_display_message ( "\nDo you (d) drink, (w) wash yourself" );
    iopts ();

    for ( ;; ) {
        switch ( ttgetch () ) {
        case 'd':
            act_drink_fountain ();
            return;

        case '\33':
        case 'i':
            fl_ignore ();
            return;

        case 'w':
            act_wash_fountain ();
            return;
        }
    }
}



/*
* a subroutine to raise or lower character levels
* if x > 0 they are raised   if x < 0 they are lowered
*/
void
fntchange ( int how )
{
    FLCoreFuncs CoreFuncs;
    int j;
    lprc ( '\n' );

    switch ( TRnd ( 9 ) ) {
    case 1:
        fl_display_message ( "Your strength" );
        fch ( how, &cdesc[FL_STRENGTH] );
        break;

    case 2:
        fl_display_message ( "Your intelligence" );
        fch ( how, &cdesc[INTELLIGENCE] );
        break;

    case 3:
        fl_display_message ( "Your wisdom" );
        fch ( how, &cdesc[WISDOM] );
        break;

    case 4:
        fl_display_message ( "Your constitution" );
        fch ( how, &cdesc[CONSTITUTION] );
        break;

    case 5:
        fl_display_message ( "Your dexterity" );
        fch ( how, &cdesc[FL_DEXTERITY] );
        break;

    case 6:
        fl_display_message ( "Your charm" );
        fch ( how, &cdesc[FL_CHARISMA] );
        break;

    case 7:
        j = TRnd ( level + 1 );

        if ( how < 0 ) {
            lprintf ( "You lose %d hit point", ( int ) j );

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }

            FL_LOSEMAXHEALTH ( ( int ) j );
        }

        else {
            lprintf ( "You gain %d hit point", ( int ) j );

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }

            FL_RAISEMAXHEALTH((int)j);
        }

        bottomline ();
        break;

    case 8:
        j = TRnd ( level + 1 );

        if ( how > 0 ) {
            lprintf ( "You just gained %d spell", ( int ) j );
            FL_RAISEMAXHEALTH((int)j);

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }
        }

        else {
            lprintf ( "You just lost %d spell", ( int ) j );
            FL_LOSEMAXSPELLS ( ( int ) j );

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }
        }

        bottomline ();
        break;

    case 9:
        j = 5 * TRnd ( ( level + 1 ) * ( level + 1 ) );

        if ( how < 0 ) {
            lprintf ( "You just lost %d experience point", ( int ) j );

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }

            CoreFuncs.DecreaseExperience(j);
        }

        else {
            lprintf ( "You just gained %d experience point", ( int ) j );

            if ( j > 1 ) {
                fl_display_message ( "s!" );
            }

            else {
                lprc ( '!' );
            }

            CoreFuncs.IncreaseExperience( j );
        }

        break;
    }

    fl_termcap_cursor_position(1,24);
}



/*
subroutine to process an up/down of a character attribute for ofountain
*/
static void
fch ( int how, long *x )
{
    if ( how < 0 ) {
        fl_display_message ( " went down by one!" );
        -- ( *x );
    }

    else {
        fl_display_message ( " went up by one!" );
        ( *x )++;
    }

    bottomline ();
}



/*
For command mode.  Perform drinking at a fountain.
*/
void
drink_fountain ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] == ODEADFOUNTAIN ) {
        fl_display_message ( "\nThere is no water to drink!" );
    }

    else if ( object_identification[playerx][playery] != OFOUNTAIN ) {
        fl_display_message ( "\nI see no fountain to drink from here!" );
    }

    else {
        act_drink_fountain ();
    }

    return;
}




/*
For command mode.  Perform washing (tidying up) at a fountain.
*/
void
wash_fountain ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] == ODEADFOUNTAIN ) {
        fl_display_message ( "\nThere is no water to wash in!" );
    }

    else if ( object_identification[playerx][playery] != OFOUNTAIN ) {
        fl_display_message ( "\nI see no fountain to wash at here!" );
    }

    else {
        act_wash_fountain ();
    }

    return;
}

void
enter ( void )
{
    fl_termcap_cursor_position(1,24);
    switch ( object_identification[playerx][playery] ) {
    case OSCHOOL:
        oschool ();
        break;
    case OBANK:
        obank ();
        break;
    case OBANK2:
        obank2 ();
        break;
    case ODNDSTORE:
        dndstore ();
        break;
    case OENTRANCE:
        /* place player in front of entrance on level 1.  newcavelevel()
           prevents player from landing on a monster/object.
         */
        playerx = 33;
        playery = MAXY - 2;
        newcavelevel ( 1 );
        been_here_before[33][MAXY - 1] = KNOWALL;
        monster_identification[33][MAXY - 1] = 0;
        draws ( 0, MAXX, 0, MAXY );
        showcell ( playerx, playery );	/* to show around player */
        bot_linex ();
        break;
    case OTRADEPOST:
        otradepost ();
        break;
    case OLRS:
        olrs ();
        break;
    case OHOME:
        ohome ();
        break;	
	case FL_OBJECT_TEMPLE_IN:
		fl_act_enter_temple();
		break;
	case FL_OBJECT_TEMPLE_OUT:
		fl_act_exit_temple();
		break;
    default:
        fl_display_message ( "\nThere is no place to enter here!\n" );
        break;
    }
}

/*
For command mode.  Perform removal of gems from a jeweled throne.
*/
void
remove_gems ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] == ODEADTHRONE ) {
        fl_display_message ( "\nThere are no gems to remove!" );
    }

    else if ( object_identification[playerx][playery] == OTHRONE ) {
        act_remove_gems ( 0 );
    }

    else if ( object_identification[playerx][playery] == OTHRONE2 ) {
        act_remove_gems ( 1 );
    }

    else {
        fl_display_message ( "\nI see no throne here to remove gems from!" );
    }

    return;
}



/*
For command mode.  Perform sitting on a throne.
*/
void
sit_on_throne ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] == OTHRONE ) {
        act_sit_throne ( 0 );
    }

    else if ( ( object_identification[playerx][playery] == OTHRONE2 ) ||
              ( object_identification[playerx][playery] == ODEADTHRONE ) ) {
        act_sit_throne ( 1 );
    }

    else {
        fl_display_message ( "\nI see no throne to sit on here!" );
    }

    return;
}

/*
For command mode.  Checks that player is actually standing at a set up
up stairs or volcanic shaft.
*/
void
up_stairs ( void )
{
    fl_termcap_cursor_position(1,24);
    if ( object_identification[playerx][playery] == OSTAIRSDOWN ) {
        fl_display_message ( "\nThe stairs don't go up!" );
    }
    else if ( object_identification[playerx][playery] != OSTAIRSUP ) {
        fl_display_message ( "\nI see no way to go up here!" );
    }
    else {
        act_up_stairs ();
    }
}

/*
Checks that player is actually standing at a set of
down stairs.
*/
void
down_stairs ( void )
{
    fl_termcap_cursor_position(1,24);
    if ( object_identification[playerx][playery] == OSTAIRSUP ) {
        fl_display_message ( "\nThe stairs don't go down!" );
    }
    else if ( object_identification[playerx][playery] != OSTAIRSDOWN ) {
        fl_display_message ( "\nI see no way to go down here!" );
    }
    else {
        act_down_stairs ();
    }
}

/*
For command mode.  Perform opening an object (door, chest).
*/
void
open_something ( void )
{
    int x, y;			/* direction to open */
    char tempc;			/* result of prompting to open a chest */
    fl_termcap_cursor_position(1,24);

    /* check for confusion.
     */
    if ( cdesc[FL_CONFUSE] ) {
        fl_display_message ( "You're too confused!" );
        return;
    }

    /* check for player standing on a chest.  If he is, prompt for and
       let him open it.  If player ESCs from prompt, quit the Open
       command.
     */
    if ( object_identification[playerx][playery] == OCHEST ) {
        fl_display_message ( "There is a chest here.  Open it?" );

        if ( ( tempc = getyn () ) == 'y' ) {
            act_open_chest ( playerx, playery );
            dropflag =
                1;		/* prevent player from picking back up if fail */
            return;
        }

        else if ( tempc != 'n' ) {
            return;
        }
    }

    /* get direction of object to open.  test 'openability' of object
       indicated, call common command/prompt mode routines to actually open.
     */
    fl_direction ( &x, &y );

    switch ( object_identification[x][y] ) {
    case OOPENDOOR:
        fl_display_message ( "The door is already open!" );
        break;

    case OCHEST:
        act_open_chest ( x, y );
        break;

    case OCLOSEDDOOR:
        act_open_door ( x, y );
        break;

    /* This message is rephrased to handle other scenarios. -Gibbon */
    default:
        fl_display_message ( "\nNothing happens.." );
        break;
    }
}




/*
For command mode.  Perform the action of closing something (door).
*/
void
close_something ( void )
{
    int x, y;
    fl_termcap_cursor_position(1,24);

    /* check for confusion.
     */
    if ( cdesc[FL_CONFUSE] ) {
        fl_display_message ( "You're too confused!" );
        return;
    }

    /* get direction of object to close.  test 'closeability' of object
       indicated.
     */
    fl_direction ( &x, &y );

    switch ( object_identification[x][y] ) {
    case OCLOSEDDOOR:
        fl_display_message ( "The door is already closed!" );
        break;

    case OOPENDOOR:
        if ( monster_identification[x][y] ) {
            fl_display_message ( "Theres a monster in the way!" );
            return;
        }

        object_identification[x][y] = OCLOSEDDOOR;
        been_here_before[x][y] = 0;
        object_argument[x][y] = 0;
        break;

    default:
        fl_display_message ( "You can't close that!" );
        break;
    }
}




/*
* For command mode.  Perform the act of descecrating an altar.
*/
void
desecrate_altar ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] == OALTAR ) {
        act_desecrate_altar ();
    }

    else {
        fl_display_message ( "\nI see no altar to desecrate here!" );
    }
}





/*
For command mode.  Perform the act of praying at an altar.
*/
void
pray_at_altar ( void )
{
    fl_termcap_cursor_position(1,24);

    if ( object_identification[playerx][playery] != OALTAR ) {
        fl_display_message ( "\nI see no altar to pray at here!" );
    }

    else {
        act_donation_pray ();
    }

    prayed = 1;
}



/*
Identify objects for the player.
*/
void
specify_object ( void )
{
    fl_termcap_cursor_position(1,24);
    fl_display_message ( "\nIdentify unknown object by cursor [ynq]?" );

    for ( ;; ) {
        switch ( ttgetch () ) {
        case '\33':
        case 'q':
            return;

        case 'y':
        case 'Y':
            specify_obj_cursor ();
            return;

        case 'n':
        case 'N':
            specify_obj_nocurs ();
            return;
        }
    }
}



/* perform the actions of identifying the object/monster associated with a
character typed by the user.  assumes cursors().
*/
static void
specify_obj_nocurs ( void )
{
    char i;
    int j, flag;
    fl_display_message ( "\nType object character:" );

    switch ( i = ttgetch () ) {
    case '\33':
    case '\n':
        return;

    case '@':
        lprintf ( "\n@: %s", logname );
        return;

    case ' ':
        lprintf ( "\n : An as-yet-unseen place in the dungeon" );
        return;

    default:
        if ( i == floorc ) {
            lprc ( '\n' );
            lprc ( floorc );
            lprintf ( ": the floor of the dungeon" );
            return;
        }

        flag = 0;

        for ( j = 0; j < MAXMONST + 8; j++ )
            if ( i == monstnamelist[j] ) {
                lprintf ( "\n%c: %s", i, monster[j].name );
                flag = 1;
            }

        /* check for spurious object character
         */
        if ( i != '_' )
            for ( j = 0; j < MAXOBJECT; j++ )
                if ( i == objnamelist[j] ) {
                    lprc ( '\n' );
                    lprc ( i );
                    lprintf ( ": %s", objectname[j] );
                    flag = 1;
                }

        if ( !flag ) {
            lprintf ( "\n%c: unknown monster/object", i );
        }

        return;
    }
}


static void
specify_obj_cursor ( void )
{
    int objx, objy;
    int i;
    fl_display_message ( "\nMove the cursor to an unknown object_identification." );
    fl_display_message ( "\n(For instructions type a ?)" );
    objx = playerx;
    objy = playery;
    fl_termcap_cursor_position( objx + 1, objy + 1 );

    /* make cursor visible.
     */
    for ( ;; ) {
        switch ( ttgetch () ) {
        case '?':
            fl_termcap_cursor_position(1,24);
            fl_display_message
            ( "\nUse [hjklnbyu] to move the cursor to the unknown object." );
            fl_display_message ( "\nType a . when the cursor is at the desired place." );
            fl_display_message ( "\nType q, Return, or Escape to exit." );
            fl_termcap_cursor_position( objx + 1, objy + 1 );
            break;

        case '\33':
        case 'q':
        case '\n':
            /* reset cursor
             */
            fl_termcap_cursor_position( playerx + 1, playery + 1 );
            return;

        case '.':
            /* reset cursor
             */
            fl_termcap_cursor_position( playerx + 1, playery + 1 );
            fl_termcap_cursor_position(1,24);

            if ( ( objx == playerx ) && ( objy == playery ) ) {
                lprintf ( "\n@: %s", logname );
                return;
            }

            i = monster_identification[objx][objy];

            if ( i && ( been_here_before[objx][objy] & KNOWHERE ) )

                /* check for invisible monsters and not display
                 */
                if ( monstnamelist[i] != floorc ) {
                    lprintf ( "\n%c: %s", monstnamelist[i], monster[i].name );
                    return;
                }

            /* handle floor separately so as not to display traps, etc.
             */
            i = object_identification[objx][objy];

            if ( i == 0 ) {
                lprc ( '\n' );
                lprc ( floorc );
                lprintf ( ": the floor of the dungeon" );
                return;
            }

            if ( been_here_before[objx][objy] & HAVESEEN ) {
                lprc ( '\n' );
                lprc ( objnamelist[i] );
                lprintf ( ": %s", objectname[i] );
                return;
            }

            lprintf ( "\n : An as-yet-unseen place in the dungeon" );
            return;

        case 'H':
        case 'h':
            move_cursor ( &objx, &objy, 4 );
            break;

        case 'J':
        case 'j':
            move_cursor ( &objx, &objy, 1 );
            break;

        case 'K':
        case 'k':
            move_cursor ( &objx, &objy, 3 );
            break;

        case 'L':
        case 'l':
            move_cursor ( &objx, &objy, 2 );
            break;

        case 'B':
        case 'b':
            move_cursor ( &objx, &objy, 8 );
            break;

        case 'N':
        case 'n':
            move_cursor ( &objx, &objy, 7 );
            break;

        case 'Y':
        case 'y':
            move_cursor ( &objx, &objy, 6 );
            break;

        case 'U':
        case 'u':
            move_cursor ( &objx, &objy, 5 );
            break;

        default:
            break;
        }
    }
}



static void
move_cursor ( int *xx, int *yy, int cdir )
{
    *xx += diroffx[cdir];
    *yy += diroffy[cdir];

    if ( *yy < 0 ) {
        *yy = MAXY - 1;
    }

    if ( *yy > MAXY - 1 ) {
        *yy = 0;
    }

    if ( *xx < 0 ) {
        *xx = MAXX - 1;
    }

    if ( *xx > MAXX - 1 ) {
        *xx = 0;
    }

    fl_termcap_cursor_position( *xx + 1, *yy + 1 );
}
