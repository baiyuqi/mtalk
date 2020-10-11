package com.byq.triangle.geomath;

import java.lang.*;
public class GVector implements ConstVector
{
  public double x;
  public double y;
  public double z;
  byte tag;
  public GVector()
  {
    x=y=z=0.0;
  }
  public GVector(double x,double y,double z)
  {
    this.x=x;
    this.y=y;
    this.z=z;
  }
  public void Rotate(double angle)
  {
  	double t = x;
  	double Cos = Math.cos(-angle);
  	double Sin = Math.sin(-angle);
  	x = x*Cos + y*Sin;
  	y =-t*Sin + y*Cos ;
  }
  public byte DirectZ(GVector v1,GVector v2)
  {
  	double dPos=(x-v1.x)*(v2.y-v1.y)
			     -(v2.x-v1.x)*(y-v1.y);
  	if(java.lang.Math.abs(dPos)<1e-10)
      return 0;
  	if (dPos>0) return 1;
    return -1;
  }
  public void reverse()
  {
    x=-x;
    y=-y;
    z=-z;
  }
  public double theta()
  {
  	if(x==0)
  		if(y==0)
  			return 10;//0向量的方向角；
  		else
  			if(y>0)
  				return 3.1415926/2.;
  			else
  				return 3.1415926*1.5;
  	else
  	{
  		double  k=y/x;
  		double  ang=Math.atan(k);
  		if(y>=0)//1.2象限；
  		{
  			if(x>0)//第1象限；
  				return ang;
  			else//第2象限；
  				return 3.1415926+ang;
  		}
  		else//3.4象限；
  		{
  			if(x<0)//第3象限；
  				return 3.1415926+ang;
  			else//第4象限；
  				return 3.1415926*2+ang;
  		}
  	}
  }
   public void copy(GVector sv)
  {
    x=sv.x;
    y=sv.y;
    z=sv.z;
  }
  public void add(GVector v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
  }
   public void scale(double sc)
  {
    x*=sc;
    y*=sc;
    z*=sc;
  }
  public void Unitize()
  {
    x/=this.Distance();
    y/=this.Distance();
    z/=this.Distance();
  }
   public double Distance()
  {
    return Math.sqrt(x*x+y*y+z*z);
  }
     public void unitize()
  {
    double r=Math.sqrt(x*x+y*y+z*z);
    x/=r;y/=r;z/=r;
  }
  public double getX()
  {
  return x;
  }
  public double getY()
  { return y;
  }
  public double getZ()
  {  return z;
  }

}