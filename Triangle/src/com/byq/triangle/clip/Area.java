package com.byq.triangle.clip;
import java.util.*;
import java.awt.*;

import com.byq.triangle.creutil.*;
import com.byq.triangle.geomath.*;
/**
 * Title:        Your Product Name
 * Description:  Your description
 * Copyright:    Copyright (c) 1999
 * Company:      creation
 * @author byq
 * @version
 */

public class Area {
 Vector bpoly;
 Vector culls;
 double minz, maxz;
 int type;
 AreaSet areaSet;
 public Area(Vector bd, Vector cs, AreaSet as, int tp) {
 bpoly = bd;
 culls = cs;
 areaSet = as;
 type = tp;
 }
 void setZ()
 {
 minz = 2000000;
 maxz = -2000000;
 int n = bpoly.size();
 for(int i = 0; i < n; i++)
 {
   GVector v = (GVector)bpoly.elementAt(i);
   minz = minz > v.z? v.z :minz;
   maxz = maxz < v.z? v.z :maxz;
 }
 areaSet.minz = areaSet.minz > minz? minz:areaSet.minz;
 areaSet.maxz = areaSet.maxz < maxz? maxz:areaSet.maxz;

 if(culls == null)return;
 for(int j = 0; j < culls.size(); j++)
 {
   Area cull = (Area)culls.elementAt(j);
   cull.setZ();
  int k = cull.bpoly.size();
 for(int i = 0; i < k; i++)
 {
   GVector v = (GVector)cull.bpoly.elementAt(i);
   minz = minz > v.z? v.z :minz;
   maxz = maxz < v.z? v.z :maxz;
 }

 }
 }
 void modifyZ()
 {
    if(culls == null)return;
    for(int i = 0; i < culls.size(); i++)
    {
       Area cull = (Area)culls.elementAt(i);
       if(cull.maxz == cull.minz)
       {
         if(minz < cull.minz)cull.maxz += areaSet.delta;
         if(maxz > cull.minz)cull.minz -= areaSet.delta;
         areaSet.minz = areaSet.minz > cull.minz? cull.minz:areaSet.minz;
         areaSet.maxz = areaSet.maxz < cull.maxz? cull.maxz:areaSet.maxz;
       }
       cull.modifyZ();
    }
 }
 public void dissect(Vector line, boolean cld)
 {
  if(!cld)lineCut(line);
  else polyCut(line);
 }


 void lineCut(Vector line)
 {
   Vector result = new Vector();
   if(!GeoMath.Intersect(bpoly, line))
     return;
   areaSet.remove(this);
   Vector pset = GeoMath.cutPoly(bpoly, line, false);
   createArea(pset);
 }
 void createArea(Vector pset)
 {
   int n = pset.size();
   for(int i = 0; i < n; i++)
   {
     Vector poly = (Vector)pset.elementAt(i);
     Vector cls = getCulls(poly);
     areaSet.add(new Area(poly,cls,areaSet, 1));
   }
 }
 Vector getCulls(Vector poly)
 {
   if(culls == null)return null;
   Vector rst = new Vector();
   int n = culls.size();
   for(int i = 0; i < n; i++)
   {
     Area cull = (Area)culls.elementAt(i);
     if(GeoMath.pRp(cull.bpoly, poly) == 1)
       rst.addElement(cull);
    }
    return rst;
 }
 void polyCut(Vector poly)
 {
   int rel = GeoMath.pRp(bpoly, poly);
   if(rel == 0||rel == 1)
   return;

   if(rel == -1)
   {
     polyInt(poly);
     return;
   }

   polyInc(poly);


 }
 void polyInt(Vector poly)
 {
   Vector pset = GeoMath.cutPoly(bpoly, poly, true);
   areaSet.remove(this);
   createArea(pset);

 }
 void polyInc(Vector poly)
 {
   if(clear(poly))
   {
     if(culls == null)culls = new Vector();
     culls.add(new Area(poly, null, areaSet, 0));
     return;
   }

   Vector cs = getCulls(poly);
   if(cs == null||cs.size() == 0)
   {
     Area ar = getIncMeArea(poly);
     ar.dissect(poly, true);
     return;
   }

   Area ar = new Area(poly, cs, areaSet, 0);
   for(int i = 0; i < cs.size(); i++)
    culls.removeElement(cs.elementAt(i));
   culls.addElement(ar);
 }


  void polyIncN(Vector poly)
 {
   if(clear(poly))
   {
     if(culls == null)culls = new Vector();
     Area ar = new Area(poly, null, areaSet, 0);
     culls.addElement(ar);
     areaSet.add(ar);
     return;
   }

   Vector cs = getCulls(poly);
   if(cs == null||cs.size() == 0)
   {
//     Area ar = getIncMeArea(poly);
//     ar.dissect(poly, true);
     return;
   }

   Area ar = new Area(poly, cs, areaSet, 0);
   areaSet.add(ar);
   for(int i = 0; i < cs.size(); i++)
    culls.removeElement(cs.elementAt(i));
   culls.addElement(ar);
 }
 boolean clear(Vector poly)
 {
   if(culls == null||culls.size() == 0)
    return true;
   int n = culls.size();
   for(int i = 0; i < n; i++)
   {
     Area cull = (Area)culls.elementAt(i);
     if(GeoMath.pRp(poly, cull.bpoly)!= 0)return false;
   }
   return true;

 }
 public void draw(Graphics g, MapWindowTranslator tran)
 {
     if(bpoly == null)return;
     float blue = (float)((maxz - areaSet.minz)/(areaSet.maxz - areaSet.minz));
     float  red=(float)((1.0-blue)/2.0);//(1.0f-blue)/4.0f;
     float green=red;//red;

    fillPoly(g, bpoly,new Color(red, green, blue), tran);

    if(culls == null)return;
    for(int i = 0; i < culls.size(); i++)
    {
      Area cull = (Area)culls.elementAt(i);
//      drawPoly(g, cull.bpoly, new Color(0,240,0),tran);
      cull.draw(g,tran);

    }
    }
 public void drawW(Graphics g, MapWindowTranslator tran)
 {
     if(bpoly == null)return;
    fillPoly(g, bpoly,new Color(200,200,50),tran);
    if(culls == null)return;
    for(int i = 0; i < culls.size(); i++)
    {
      Area cull = (Area)culls.elementAt(i);
 //     fillPoly(g, cull.bpoly, new Color(0,0,240),tran);
      cull.draw(g,tran);

    }
 }
 public Area getArea(double x, double y)
 {
   GVector v = new GVector(x, y, 0);
   if(!GeoMath.InsidePolygon(bpoly, v))return null;
   if(culls == null)return this;
   int n = culls.size();
   for(int i = 0; i < n; i++)
   {
     Area cull = (Area)culls.elementAt(i);
     if(GeoMath.InsidePolygon(cull.bpoly,v))return cull.getArea(x, y);
   }
   return this;
 }
 void drawPoly(Graphics g, Vector poly,Color c, MapWindowTranslator tran)
 {
    if(poly == null)return;
    g.setColor(c);
    for(int i = 0; i < poly.size(); i++)
    {
       GVector p1 = (GVector)poly.elementAt(i),
       p2 = (GVector)poly.elementAt((i+1)%poly.size());

       g.drawLine(tran.getWindowX((float)p1.x), tran.getWindowY((float)p1.y),
       tran.getWindowX((float)p2.x), tran.getWindowY((float)p2.y));

    }
//    g.fillPolygon();
 }
 void fillPoly(Graphics g, Vector poly,Color c, MapWindowTranslator tran)
 {
    if(poly == null)return;
    g.setColor(c);
    int n = poly.size();
    int x[] = new int[n];
    int y[] = new int[n];
    for(int i = 0; i < poly.size(); i++)
    {
       GVector p = (GVector)poly.elementAt(i);
       x[i] = tran.getWindowX((float)p.x);
       y[i] = tran.getWindowY((float)p.y);
    }
    g.fillPolygon(x, y, n);
 }
 Area getIncMeArea(Vector poly)
 {
   int n = culls.size();
   for(int i = 0; i < n; i++)
   {
     Area cull = ((Area)culls.elementAt(i));;
     if(GeoMath.pRp(poly, cull.bpoly)== 1)return cull;
   }
   return null;

 }
}