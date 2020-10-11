package com.byq.triangle.triangles;

class RawDataInfo {
  public double minx,maxx,miny,maxy,minz,maxz,meanz;
  public int pnumber;
  public RawDataInfo() {
    minx=100000;
    maxx=-100000;
    miny=100000;
    maxy=-100000;
    minz=100000;
    maxz=-100000;    
    pnumber=0;
  }
   public void reset() {
    minx=100000;
    maxx=-100000;
    miny=100000;
    maxy=-100000;
    minz=100000;
    maxz=-100000;
    pnumber=0;
  }
} 