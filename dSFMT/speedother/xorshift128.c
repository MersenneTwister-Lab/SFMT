void init(unsigned int x) {
}

double generate(){
  static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
  unsigned long t;
  t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) ) * 2.32830643653869628906e-10;
}

#include "test_time2.c"
