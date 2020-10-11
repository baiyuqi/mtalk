package com.byq.triangle.triangles;
import com.byq.triangle.geomath.*;
/**
 * Title:        Your Product Name
 * Description:  Your description
 * Copyright:    Copyright (c) 1999
 * Company:      creation
 * @author byq
 * @version
 */

public class MyMath {

 public MyMath() {
 }

  static public double Height(Vertex vertex,Trigon trigon)
  {
	  if(trigon==null)return -100000.;
	  GVector[] v=new GVector[3];
    double r=1.0/trigon.dim();
    double x0=trigon.minx();
    double y0=trigon.miny();
	  for(byte i=0;i<3;i++)
	  {
      v[i]=new GVector();
		  v[i].x=(trigon.vex[i].coord.x-x0)*r;
		  v[i].y=(trigon.vex[i].coord.y-y0)*r;
		  v[i].z=trigon.vex[i].coord.z;
	  }
	  double x,y;
	  x=(vertex.coord.x-x0)*r;
	  y=(vertex.coord.y-y0)*r;
	  double z1=trigon.minz();
	  double z2=trigon.maxz();
	  double z=GeoMath._Height(v[0],v[1],v[2],x,y);
	  z=z>z2?z2:z;
	  z=z<z1?z1:z;
	  return z;
  }

}