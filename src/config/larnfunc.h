#include <cstdlib>

using namespace std;

/* Templates
 * I am very happy to get rid of these crappy macros!
 * ~Gibbon
 */
template<typename T>
inline T TRnd(const T& x)
{
	return static_cast<T>(rand() % (x)) + 1;
}

template<typename T>
inline T TRund(const T& x)
{
	return static_cast<T>(rand() % (x));
}

template<typename T>
inline T TMathMin(const T& x, const T& y)
{
	return x > y ? y : x;
}

template<typename T>
inline T TMathMax(const T& x, const T& y)
{
	return x > y ? x : y;
}

/* macro to create scroll #'s with probability of occurrence */
#define NewScroll() (scprob[TRund(81)])

/* macro to return a potion # created with probability of occurrence */
#define newpotion() (potprob[TRund(41)])

/* macro to return the + points on created leather armor */
#define newleather() (nlpts[TRund(cdesc[HARDGAME]?13:15)])

/* macro to return the + points on chain armor */
#define newchain() (nch[TRund(10)])

/* macro to return + points on plate armor */
#define newplate() (nplt[TRund(cdesc[HARDGAME]?4:12)])

/* macro to return + points on new daggers */
#define newdagger() (ndgg[TRund(13)])

/* macro to return + points on new swords */
#define newsword() (nsw[TRund(cdesc[HARDGAME]?6:13)])

/* macro to destroy object at present location */
#define forget() (item[playerx][playery]=know[playerx][playery]=0)

/* macro to turn on bold display for the terminal */
#define setbold() (*lpnt++ = ST_START)

/* macro to turn off bold display for the terminal */
#define resetbold() (*lpnt++ = ST_END)

/* macro to setup the scrolling region for the terminal */
#define setscroll() enable_scroll=1

/* macro to clear the scrolling region for the terminal */
#define resetscroll() enable_scroll=0

/* macro to clear the screen and home the cursor */
#define screen_clear() (*lpnt++ =CLEAR, regen_bottom=1)

/*
*
* function declarations
*
*/

#ifndef WINDOWS
	#define _getch ansiterm_getch
#endif
