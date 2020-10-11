package com.byq.triangle.geomath;
import java.util.*;

import com.byq.triangle.triangles.*;
/**
 * Title:        Your Product Name
 * Description:  Your description
 * Copyright:    Copyright (c) 1999
 * Company:      creation
 * @author byq
 * @version
 */

public class Segment {
 public GVector vex;
 public Vector sa = new Vector();
 public Segment(GVector v) {
 vex = v;
 }
 public void add(GVector v)
 {
   GVector r = GeoMath.differance(v, vex);
   double rm = r.Distance();
   int i;
   for(i = 0; i < sa.size(); i++)
   {
   GVector p = (GVector)sa.elementAt(i);
   GVector dp = GeoMath.differance(p, vex);
   double pm = dp.Distance();
   if(rm < pm)break;
   }
   sa.add(i, v);
 }
}