# prettyPlotPoint
Plot series of graphic points as scaled point or bar graph

I'm amazed how long it took me to get this to work, even
to the limited extent that it now does.
I needed this functionality (to visualize a series of data points
as a 2-dimensional graph) and couldn't find compact code that
implemented it simply and for Arduino and ESP boards.
So, I developed it.

This is UNRELEASED, meaning not ready for prime time. You are welcome to use and develop. I would be 
very  interested to learn of any bugs, improvements, enhancements, etc.

Installation:

Clone or download ZIP.

Include in your sketch.

There are a number of variables used by the routines, some of which may need to be defined before 
including the code. (These are defined, therefore don't really have type.)

"type"  name  default purpose
class   myGLCD  LCD   class methods (display)
int     MINX      0   left-hand side of screen
int     MAXX     83   right-hand side of screen
int     MINY      0   top (or bottom) of screen
int     MAXY     47   bottom (or top) of screen
int     H         8   height of a character
int     W         6   width of a character
int     lastCol   (MAXX - (3*W) -1)
bool    BAR       false draw bar graph (else point)
bool    INVERT    false make MINY bottom of screen
int     COLOR     1   this usually works
bool    BL        true  if character origin is on Bottom Left

This code expects all the above to be defined and initialized, else defaults are used. Note that there is not default for the display class.

Each call to plotLCDpoint(x, y) plots one point (point or bar graph) and returns new x (which may very often be the same as the last call. Routine handles shifting display left and replotting at new scale if needed.
