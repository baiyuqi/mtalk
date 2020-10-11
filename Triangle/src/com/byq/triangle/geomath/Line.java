package com.byq.triangle.geomath;
import java.lang.*;
public class Line
{
  public GVector start,end;
  public Line()
  {
  }
   public Line(GVector start,GVector end)
  {
    this.start=start;
    this.end=end;
  }
  public boolean On(GVector v)
  {
  	double dx,dy,dx1,dy1;
	  dx=end.x-start.x;
	  dy=end.y-start.y;
	  dx1=v.x-start.x;
  	dy1=v.y-start.y;
  	double a=java.lang.Math.abs(dx*dy1-dy*dx1);
  	double b=dx1*(dx1-dx);
  	double c=dy1*(dy1-dy);
  	boolean b0,b1,b2;
	  b0=(a==0.000);/*1e-8*/
  	b1=(b<0);
  	b2=(c<0);
  	return (b0)&&(b1||b2);
  }
  public boolean SameSide(GVector v1,GVector v2)
  {
   	double dx,dy,dx1,dy1,dx2,dy2;
  	dx=end.x-start.x;
  	dy=end.y-start.y;
  	dx1=v1.x-start.x;
	  dy1=v1.y-start.y;
	  dx2=v2.x-end.x;
    dy2=v2.y-end.y;
    return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2)>0);
  }
  public boolean SameSide1(GVector v1,GVector v2)
  {
   	double dx,dy,dx1,dy1,dx2,dy2;
  	dx=end.x-start.x;
  	dy=end.y-start.y;
  	dx1=v1.x-start.x;
	  dy1=v1.y-start.y;
	  dx2=v2.x-end.x;
    dy2=v2.y-end.y;
    return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2)>=0);
  }
}