package com.byq.triangle.triangles;
import java.awt.*;
import java.util.*;

import com.byq.triangle.geomath.*;
import com.byq.triangle.matrixcon.*;

public class Contour {
  double value;
  ContourSource pNet;
  Vector list=new Vector();
  Trigon StartTrigon;
  Trigon CurrentTrigon;
  GVector CurrentVector;
  byte CurrentVID;
  boolean reverse=false;
  public Contour(double va,  ContourSource net) {
    value=va;
    pNet=net;
    CurrentTrigon=StartTrigon=null;
    CurrentVector=null;
    CurrentVID=-1;
  }
  public boolean GetFirst()
  {
    Trigon p=pNet.GetCrossTrigon(value);
    if(p==null)return false;
    CurrentTrigon=StartTrigon=p;
    for(byte i=0;i<3;i++)
    {
      GVector v1,v2,temp;
      v1=p.vex[i].coord;
      v2=p.vex[(i+1)%3].coord;
      if(GeoMath.IsCross(v1,v2,value))
      {
        CurrentVector=GeoMath.GetSect(v1,v2,value);
        CurrentVID=(byte)((i+2)%3);
        break;
      }
    }
    Insert();
    return true;
  }
  public boolean GetNext()
  {
    GVector v=CurrentTrigon.vex[CurrentVID].coord;
    byte b1=(byte)((CurrentVID+1)%3);
    byte b2=(byte)((CurrentVID+2)%3);

    GVector v1=CurrentTrigon.vex[b1].coord;
    GVector v2=CurrentTrigon.vex[b2].coord;
    Trigon temp=CurrentTrigon;
    if(GeoMath.IsCross(v,v1,value))
    {
      CurrentVector=GeoMath.GetSect(v,v1,value);
      byte nid=CurrentTrigon.nbvex[b2];
//      if(CurrentTrigon.visited)return false;
      CurrentTrigon=CurrentTrigon.nbtri[b2];
      CurrentVID=nid;
//      if(reverse)CurrentTrigon.visited=true;
      if(CurrentTrigon==null&&!reverse)
      {
        CurrentTrigon=StartTrigon=temp;
        CurrentVID=b2;
        reset();Insert();
        reverse=true;
        go();
//        ExtractSegment();
        return false;
      }
    }
    else  if(GeoMath.IsCross(v2,v,value))
    {
      CurrentVector=GeoMath.GetSect(v2,v,value);
      byte nid=CurrentTrigon.nbvex[b1];
      temp=CurrentTrigon;
      CurrentTrigon=CurrentTrigon.nbtri[b1];
//      if(CurrentTrigon.visited)return false;
      CurrentVID=nid;
//      if(reverse)CurrentTrigon.visited=true;
      if(((CurrentTrigon==null)||(CurrentTrigon==StartTrigon))&&!reverse)
      {
        CurrentTrigon=StartTrigon=temp;
        CurrentVID=b2;
        reset();Insert();
        reverse=true;
        go();
//        ExtractSegment();
        return false;
      }
    }
    Insert();
    if(CurrentTrigon==StartTrigon)
      return false;

    if(CurrentTrigon==null&&reverse)
      return false;
    return true;
  }
  public void go()
  {
    while(GetNext());
  }
  public boolean ExtractSegment()
  {
    if(!GetFirst())return false;
    go();
    return true;
  }
  public void reset()
  {
    list.clear();
  }
  public void Insert()
  {
    //cnode p=new cnode(CurrentVector.x, CurrentVector.y);
    list.addElement(CurrentVector);
  }
  public void draw(Graphics g,DrawParam p)
  {
    ContourNode p1,p2;

    for(int i=0;i<list.size()-1;i++)
    {
      p1=(ContourNode)list.elementAt(i);
      p2=(ContourNode)list.elementAt((i+1)%list.size());
             g.drawLine(
                    (int)((p1.coord.x+p.x0)*p.r),
                    (int)((p1.coord.y+p.y0)*p.r),
                    (int)((p2.coord.x+p.x0)*p.r),
                    (int)((p2.coord.y+p.y0)*p.r)
                    );
                    }
  }
}