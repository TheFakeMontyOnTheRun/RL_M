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

#include "dungeon.hpp"
#include "../templates/math.t.hpp"
#include "../../includes/main.h"
#include "../config/data.h"
#include "../../includes/io.h"
#include "../core/sysdep.hpp"
#include "../core/scores.hpp"
#include "../../includes/display.h"
/*
 *  fl_effect_of_sphere_detonation(x,y)    Function to perform the effects of a sphere detonation
 *      int x,y;
 *
 *  Enter with the coordinates of the blast, Returns no value
 */
void
fl_effect_of_sphere_detonation ( int x, int y )
{
    int i, j;

    if ( cdesc[FL_HOLDMONST] ) {
        cdesc[FL_HOLDMONST] = 1;
    }

    if ( cdesc[FL_CANCELLATION] ) {
        cdesc[FL_CANCELLATION] = 1;
    }

    for ( j = TMathMax ( 1, x - 2 ); j < TMathMin ( x + 3, MAXX - 1 ); j++ )
        for ( i = TMathMax ( 1, y - 2 ); i < TMathMin ( y + 3, MAXY - 1 ); i++ ) {
            object_identification[j][i] = monster_identification[j][i] = 0;
            fl_show_designated_cell_only ( j, i );

            if ( playerx == j && playery == i ) {
                fl_termcap_cursor_position(1,24);
                fl_display_message ( "\nYou were too close to the sphere!" );
                fl_wait ( FL_WAIT_DURATION );
                fl_player_death ( 283 );		/* player killed in explosion */
            }
        }
}