package com.byq.triangle.clip;
import java.util.*;
import java.awt.*;

import com.byq.triangle.creutil.*;
/**
 * Title:        Your Product Name
 * Description:  Your description
 * Copyright:    Copyright (c) 1999
 * Company:      creation
 * @author byq
 * @version
 */

public class AreaSet {
 Vector set = new Vector();
 int layer;
 double delta;
 public AreaSet(double delt) {
 delta = delt;
 }
 public void add(Area ar)
 {
   set.addElement(ar);
 }
 public void remove(Area ar)
 {
   set.removeElement(ar);
 }
 public void LineCut(Vector line, boolean closed)
 {
    Vector temp = copy(set);

    int n = temp.size();
    for(int i = 0; i < n; i++)
    {
      Area ar = (Area)temp.elementAt(i);
      ar.dissect(line, closed);
    }

 }

 double minz, maxz;
 public void setZ()
 {
   minz = 2000000;
   maxz = -2000000;
   int n = set.size();
   for(int i = 0; i < n; i++)
   {
    Area ar = ((Area)set.elementAt(i));
    ar.setZ();
    ar.modifyZ();
    }
 }
 public Area getArea(double x, double y)
 {
   int n = set.size();
   for(int i = 0; i < n; i++)
   {
     Area rst, ar;
     ar = (Area)set.elementAt(i);
     rst = ar.getArea(x,y);
     if(rst!=null)return rst;
   }
   return null;
 }
 Vector copy(Vector v)
 {
 if(v == null)return null;
 Vector r = new Vector();
 int n = v.size();
 for(int i = 0; i < n; i++)
  r.addElement(v.elementAt(i));
 return r;
 }
 public void draw(Graphics g, MapWindowTranslator tran)
 {
   int n = set.size();
   for(int i = 0; i < n; i++)
     ((Area)set.elementAt(i)).draw(g, tran);

//   ((Area)set.elementAt(ind%set.size())).drawW(g, tran);
 }

}