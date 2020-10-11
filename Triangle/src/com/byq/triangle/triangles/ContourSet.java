package com.byq.triangle.triangles;

import java.awt.*;
import java.util.*;
public class ContourSet extends Object{
  double value;
  ContourSource pNet;
  Vector list=new Vector();
  int noc;
  public ContourSet(double v,  ContourSource net) {
  noc=0;
  pNet=net;
  value=v;
  GetAll();
  }
  public boolean GetSeg()
  {
    Contour seg=new Contour(value,  pNet);
    if(seg.ExtractSegment())
    {
      Insert(seg);
      return true;
    }
    return false;
  }
  public void Insert(Contour con)
  {
    list.add(con);
  }
  private void GetAll()
  {
    while(GetSeg());
    pNet.resetvisit();
  }
  public void draw(Graphics g,DrawParam p)
  {

  int n=list.size();
  for(int i=0;i<n;i++)
  {
    Contour pc=(Contour)list.get(i);
     pc.draw(g,p);
   }
  }
}