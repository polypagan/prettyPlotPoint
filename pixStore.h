#ifndef PIXSTOR_H
#define PIXSTOR_H
/*
 * specialized pixel storage system
 * 
 * Performs the following
 *  enqueues up to storeSize pixels (int) using push()
 *  maintains count of pixels stored
 *  pops pixels off queue (reverse order)
 *  reports count
 *  allows update of count
 *  
 *  utilities:
 *    minmax() updates min & max of pixel
 *    dumpStore() displays pointer, count and storage.
 *    
 *  void push(int16_t);
 *  int16_t pop(void);
 *  int16_t getStoreCount(void);
 *  int16_t getReadPtr(void);
 *  void putReadPtr(int16_t);
 *  void minmax(int16_t, int16_t*, int16_t*);
 *  void dumpStore(void);
 *  
 */
// DdelV 20181007
//       20181008 changed r, w pointers to one, added count c
//       20181020 added dumpStore() for debug, corrections
//       20181021 added s, savePtr(), restorePtr()

const int16_t storeSize = lastCol;		// just right?

static int16_t p=0, c=0, s;
int16_t	Store[storeSize];

void push(int16_t d) {
	
	Store[p++] = d;
  c++;
	if (p >= storeSize)p = 0;           // wrap around
  if (c > storeSize)c = storeSize;   // cannot possibly hold more, if wrapped, we're over-writing
}

int16_t pop() { // does not alter count c

  p--;
  if (p < 0)p = storeSize-1;
	return Store[p];
}
// discover how many items are in store
int16_t getStoreCount() {
  return c;
}
// ability to read, save and restore read pointer allows multiple reads
int16_t getReadPtr() { return p; }
void putReadPtr(int16_t v) { p = v; }
void savePtr() { s = p; }
void restorePtr() { p = s; }

// compute min/max for a data point
void minmax(int16_t d, int16_t* min, int16_t* max) {

	if(d < *min)*min = d;
	if(d > *max)*max = d;
}
void dumpStore() {
int t;

  Serial << "p: " << p << " c: " << c << endl;
  Serial << "Store[-1]: " << Store[-1] << " Store[storeSize]: " << Store[storeSize] << endl;
  t = p;
  for (int i=0; i<storeSize; i++) {
    Serial << Store[i] << " ";
    if(--t < 0)t=storeSize-1;
    if(!(i % 40))Serial << endl;
  }
}
#endif
