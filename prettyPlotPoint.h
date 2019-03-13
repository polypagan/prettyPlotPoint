#ifndef PPP_H
#define PPP_H

/*
 * DdelV 20181009
 * 
 * 
 * plot data point as single pixel on
 * Nokia 5110
 * 
 * Does scale (on right) and
 * adjusts to any integer data.
 * 
 */
 /*
  * This version converted to 
  * Adafruit_GFX methods.
  * 
  * DdelV 20181206
  */

 #include <Adafruit_GFX.h>
 
 #ifndef myGLCD
 #define myGLCD LCD
 #endif
 #ifndef MINX
 #define MINX 0
 #endif
 #ifndef MAXX
 #define MAXX 83
 #endif
 #ifndef MINY
 #define MINY 0
 #endif
 #ifndef MAXY
 #define MAXY 47
 #endif
 #ifndef H
 #define H 8   // height of characters
 #endif
 #ifndef W
 #define W 6   // width of characters
 #endif
 #ifndef lastCol
 #define lastCol (MAXX - (3*W) -1)
 #endif
 #ifndef bar
 #define bar false
 #endif
 #ifndef invert
 #define invert false
 #endif
 #ifndef COLOR
 #define COLOR 1
 #endif
 #ifndef BL
 #define BL true	// bottom left
 #endif
 
// NB: X = horizontal axis, Y = vertical axis.

#include "pixStore.h" // goofy pixel storage system for re-calculate and scale

// const uint8_t MINX = 0, MINY = 0;	// now defined above to support microOLED
int16_t MINval = MINY, MAXval = MAXY;   // to begin, assume real world fits on display
char Str[8];  // temporary buffer for conversions

uint16_t graphLCDpoint(uint16_t, int16_t);
size_t Gprint(const char*, int16_t, int16_t);
void drawAxes(void);
void setupDisplay(int16_t);
uint16_t plotNext(uint16_t, int16_t);
void LcdPanLeft1(int );

/*
 *  make a mark at x,y
 *  return new x (column address)
 *  
 *  supports bar & invert
 *  
 */
uint16_t graphLCDpoint(uint16_t x, int16_t yy) {     // horizontal unsigned, vertical signed.
int16_t y;

  if ((yy < MINval) || (yy > MAXval)) Serial << "MINval:" << MINval << " yy:" << yy << " MAXval:" << MAXval << endl;
  
  y = map(yy,MINval,MAXval,MINY,MAXY);   // convert virtual coordinates to hardware

 if (y>MAXY || y<MINY) {  // This should never happen
  Serial << "param y:" << yy << " mapped to:" << y << endl;
  return x;   // don't attempt to plot that noise.
 }

  if (invert) {
    myGLCD.drawPixel(x, MAXY-y, COLOR);
  } else {
    myGLCD.drawPixel(x,y,COLOR);  // invert y (vertical) axis
  }
  
  if (bar) {
    for(y--; y>=MINY; y--) {
      if (invert) {
        myGLCD.drawPixel(x,MAXY-y,COLOR);
      } else {
      myGLCD.drawPixel(x,y,COLOR);
      }
    }
  }

  if (++x > lastCol) {
    LcdPanLeft1(lastCol);
    x = lastCol;                 // keep plotting in right-most column
  }
 
/* somehow, this simple idea doesn't work  
  LCD.setCursor(0,H);
  LCD.print("     ");
  LCD.setCursor(2,H);
  LCD.print(yy);
 */
 
  return x;
}
/* this mess needs sorted
size_t Gprint(const char* s, int16_t x, int16_t y) {
  myGLCD.setCursor(x,y);
  return myGLCD.print(s);
}
size_t Gprint(int16_t n, int16_t x, int16_t y) {
  myGLCD.setCursor(x,y);
  return myGLCD.print(n);
}
*/
void Gprint(int16_t n, int16_t x, int16_t y) {	// this works iff class extends print
	LCD.setCursor(x, y);
	LCD.print(n);
	Serial << "-->" << n << "<-- at x:" << x << ", y:" << y << endl;
}
/*
 * (re-)draw vertical axis on right side of display
 * 
 */
void drawAxes() {
	
  if (H != 0 && W != 0) {	
    myGLCD.drawLine(lastCol+1, MAXY, lastCol+1, MINY, COLOR);            // draw vertical line
    myGLCD.drawLine(lastCol+1, MAXY, lastCol+1+(3*W), MAXY, COLOR);          // add 5 tick marks
    myGLCD.drawLine(lastCol+1, 3*MAXY/4, lastCol+1+(3*W), 3*MAXY/4, COLOR);
    myGLCD.drawLine(lastCol+1, MAXY/2, lastCol+1+(3*W), MAXY/2, COLOR);
    myGLCD.drawLine(lastCol+1, MAXY/4, lastCol+1+(3*W), MAXY/4, COLOR);
    myGLCD.drawLine(lastCol+1, MINY, lastCol+1+(3*W), MINY, COLOR);

    int Mid = MINval + ((MAXval-MINval)/2);

#if BL	// font char origin at bottom left
    Gprint(MINval, lastCol+2, MAXY-1);        // label scale at bottom
    Gprint(Mid, lastCol+2, ((MAXY-MINY)/2)-1);      // middle
    Gprint(MAXval, lastCol+2, MINY+H+1);      // and top    
#else   // font char origin at top left  
    Gprint(MINval, lastCol+2, MAXY-H-1);        // label scale at bottom
    Gprint(Mid, lastCol+2, ((MAXY-MINY)/2)-H-1);      // middle
    Gprint(MAXval, lastCol+2, MINY+1);      // and top    
#endif
  }
}

/*
 * (re-)compute display limits
 * 
 */
void setupDisplay(int16_t d) {
// uint16_t rSave;
int16_t pmin, pmax;
int i;
int y;

  myGLCD.clearDisplay();          // wipe what went before (then replot all)

  savePtr();
  
  pmin = 32767;                   // min = MAXINT
  pmax = -32768;                  // max = MININT
 
  for (i=getStoreCount(); i > 0; i--) {    // calculate min/max over saved pixels (if any)
    minmax(pop(),&pmin,&pmax);
  }

// reset logical limits
    MAXval = pmax;      // these may need some fudge
    MINval = pmin;

    if (MAXval == MINval) {
      MAXval += MAXY/2;
      MINval -= MAXY/2;
    }
/*
    if (2*(MAXval - MINval) < MAXY) {
      MAXval = 2*((MAXY-MINY)/2) + ((pmax - pmin)/2);
      MINval = 2*((MAXY-MINY)/2) - ((pmax - pmin)/2);
    }
*/
  Serial << "pmin:" << pmin << " pmax:" << pmax << endl;
  
  restorePtr();         // restore pointer for re-reading/plotting


  for (i=getStoreCount(); i>0; i--) {
    graphLCDpoint(i,y=pop());
    if (y <= 0)Serial << "(suD) Hey, popped " << y << " from store!" << endl;
  }

  restorePtr();         // restore pointer like nothing happened
  
  drawAxes();                 // and re-calculated scale
  
}

uint16_t plotNext(uint16_t x, int16_t d) {  // returns new x
static boolean first = true;
  
  push(d);      // save raw datum in case of redraw

	if(first) {
	  drawAxes();
    first = false;
    x = MINX;
	}

	if((d > MAXval) || d < (MINval)) {
		setupDisplay(d);                 // erase and draw axes
	}

  
	return graphLCDpoint(x, d);         // plot the point

}
// experimental panLeft1() saves modifying library...
/*
 *  erase screen,
 *  move each pixel left by one,
 *  then restore axes
 */
void LcdPanLeft1(int column) {
int i;
int y;

  myGLCD.clearDisplay();

//  Serial << "LcdPanLeft1(" << column << ") " << endl;
  
  savePtr();
  
  for (i=column; i>0; i--) {
    graphLCDpoint(i-1, y=pop());
    if (y <= 0)Serial << "(LPL1) Hey! popped " << y << " from store!" << endl;
  }

  restorePtr();

  drawAxes();
  
}
#endif
