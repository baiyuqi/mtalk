package com.byq.triangle.geomath;
import java.util.*;

import com.byq.triangle.triangles.*;
//import clip.*;
public class GeoMath
{

  public GeoMath()
  {
  }
  static   public GVector differance(GVector v1,GVector v2)
  {
  	GVector v=new GVector();
    v.x=v1.x-v2.x;
    v.y=v1.y-v2.y;
    v.z=v1.z-v2.z;
    if(Math.abs(v.x)<1e-8)v.x=0.;
    if(Math.abs(v.y)<1e-8)v.y=0.;
    if(Math.abs(v.z)<1e-8)v.z=0.;
  	return v;
  }
static    public GVector add(GVector v1,GVector v2)
  {
  	GVector v=new GVector();
    v.x=v1.x+v2.x;
    v.y=v1.y+v2.y;
    v.z=v1.z+v2.z;
  	return v;
  }
  static private boolean InterSect(Line l1,Line l2)
  {
    return !l1.SameSide(l2.start,l2.end)&&!l2.SameSide(l1.start,l1.end);
  }
  static public boolean InterSect1(Line l1,Line l2)
  {
    return !l1.SameSide1(l2.start,l2.end)&&!l2.SameSide1(l1.start,l1.end);
  }
  static public boolean InterSect(GVector v1,GVector v2,GVector l1,GVector l2)
  {
    return !SameSide(v1,v2,l1,l2)
                        &&!SameSide(l1,l2,v1,v2);
  }
  static public boolean InterSect1(GVector v1,GVector v2,GVector l1,GVector l2)
  {
    return !SameSide1(v1,v2,l1,l2)
                        &&!SameSide1(l1,l2,v1,v2);
  }
  static public boolean SameSide(GVector v1,GVector v2,GVector l1,GVector l2)
  {
   	double dx,dy,dx1,dy1,dx2,dy2;
   	dx=l2.x-l1.x;
  	dy=l2.y-l1.y;
  	dx1=v1.x-l1.x;
	  dy1=v1.y-l1.y;
	  dx2=v2.x-l2.x;
    dy2=v2.y-l2.y;
    return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2)>0);
  }
  static public boolean SameSide1(GVector v1,GVector v2,GVector l1,GVector l2)
  {
   	double dx,dy,dx1,dy1,dx2,dy2;
  	dx=l2.x-l1.x;
  	dy=l2.y-l1.y;
  	dx1=v1.x-l1.x;
	  dy1=v1.y-l1.y;
	  dx2=v2.x-l2.x;
    dy2=v2.y-l2.y;
    return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2)>=0);
  }
  static public boolean OnLine(GVector v,GVector l1,GVector l2)
  {
  	double dx,dy,dx1,dy1;
  	dx=l2.x-l1.x;
  	dy=l2.y-l1.y;
  	dx1=v.x-l1.x;
  	dy1=v.y-l1.y;
  	double a=Math.abs(dx*dy1-dy*dx1);
  	double b=dx1*(dx1-dx);
  	double c=dy1*(dy1-dy);
  	boolean b0,b1,b2;
  	b0=(a<=1e-8);/*1e-8*/
  	b1=(b<0);
  	b2=(c<0);
  	return (b0)&&(b1||b2);
  }
  static public GVector GetSect(GVector l1,GVector l2,double z)
  {
    GVector t;
    if(l1.z>l2.z)
    {
      t=l1;
      l1=l2;
      l2=t;
     }
    GVector v=new GVector();
 /*
    if(z==l1.z&&z==l2.z)
    {
      v.x=(l2.x-l1.x)/2.0 + l1.x;
      v.y=(l2.y-l1.y)/2.0 + l1.y;
      v.z=(l2.z-l1.z)/2.0 + l1.z;
      return v;
    }
    */
    if(z==l1.z)
    {
      v.copy(l1);
      return v;
    }
    if(z==l2.z)
    {
      v.copy(l2);
      return v;
    }
    double k=(z-l1.z)/(l2.z-l1.z);
    v.x=k*(l2.x-l1.x)+l1.x;
    v.y=k*(l2.y-l1.y)+l1.y;
    return v;
  }
static public boolean InsidePolygon(Vector poly, GVector point)
{
	int c=0;
	GVector faraway = new GVector();
	faraway.x=1000000;
	GVector p1,p2,p3,p4;

 int size = poly.size();
	for(int i=0;i<size;i++)
	{
		p1=(GVector)poly.elementAt(i%size);
		p2=(GVector)poly.elementAt((i+1)%size);
		p3=(GVector)poly.elementAt((i+2)%size);
		p4=(GVector)poly.elementAt((i+3)%size);

		boolean b1,b2,b3,b4,b5,b6,b7,b8,b;
		b1=InterSect(point,faraway,p1,p2);
		b2=OnLine(p2,point,faraway);
		b3=OnLine(p3,point,faraway);
		b4=SameSide(p1,p3,point,faraway);
		b5=OnLine(p3,point,faraway);
		b6=SameSide(p1,p4,point,faraway);
		b7=!b3&&!b4;
		b8=b5&&!b6;
		b=b1||(b2&&(b7||b8));
		if(b)c++;
	}

	return (c%2)!=0;
}

  static GVector GetSectZ(GVector l1,GVector l2,double x)
  {
    GVector t;
    if(l1.x>l2.x)
    {
      t=l1;
      l1=l2;
      l2=t;
     }
    GVector v=new GVector();
    if(x==l1.x)
    {
      v.copy(l1);
      return v;
    }
    if(x==l2.x)
    {
      v.copy(l2);
      return v;
    }
    double k=(x-l1.x)/(l2.x-l1.x);
    v.z=k*(l2.z-l1.z)+l1.z;
    v.y=k*(l2.y-l1.y)+l1.y;
    return v;
  }
  static public boolean IsCross(GVector l1,GVector l2,double z)
  {
    GVector t;
    if(l1.z>l2.z)
    {
      t=l1;
      l1=l2;
      l2=t;
    }
    return z>=l1.z&&z<=l2.z;
  }
  static public GVector CrossMult(GVector v1,GVector v2)
  {
  	GVector r=new GVector();
  	r.x = v1.y*v2.z-v1.z*v2.y;
  	r.y = v1.z*v2.x-v1.x*v2.z;
  	r.z = v1.x*v2.y-v1.y*v2.x;
  	return r;
  }
  static public double DotMult(GVector v1,GVector v2)
  {
  	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
  }
  static boolean equal(GVector v1, GVector v2)
  {
    double dx,dy, dz;
    dx=v1.x-v2.x;dy=v1.y-v2.y;dz=v1.z-v2.z;
    if(Math.abs(dx)<1e-8&&Math.abs(dy)<1e-8&&Math.abs(dz)<1e-8)
      return true;
    return false;
  }
   static public boolean toonear(GVector v, double x,double y)
  {
    double dx,dy;
    dx=v.x-x;dy=v.y-y;
    if(Math.abs(dx)<1e-8&&Math.abs(dy)<1e-8)
      return true;
    return false;
  }
  static public double _Height(GVector v1, GVector v2, GVector v3, double x, double y)
  {
    if(toonear(v1,x,y))return v1.z;
    if(toonear(v2,x,y))return v2.z;
    if(toonear(v3,x,y))return v3.z;
    GVector v=new GVector(x,y,0);GVector vz1,vz2,vz3;
    if(GeoMath.OnLine(v,v1,v2)){
      vz1=GetSectZ(v1,v2,x);
      return vz1.z;
      }
    if(GeoMath.OnLine(v,v2,v3)){
      vz2=GetSectZ(v2,v3,x);
      return vz2.z;
      }
    if(GeoMath.OnLine(v,v3,v3)){
      vz3=GetSectZ(v3,v3,x);
      return vz3.z;
      }

  	GVector N = GeoMath.CrossMult(GeoMath.differance(v3,v1),GeoMath.differance(v2,v1));
  	if(N.z == 0.0000000)
  		return (v1.z+v2.z+v3.z)/3;
  	return (GeoMath.DotMult(N, v1)-(N.x*x+N.y*y))/N.z;
  }
  static public boolean overlap(double start1,double end1,double start2,double end2)
  {
     if(start1<start2&&end1>end2
        ||start1>start2&&end1<end2
        ||start1<start2&&end1>start2
        ||start1>start2&&end1<start2)
        return true;
     return false;
  }
static public boolean InTriangle (GVector v0, GVector v1,
								 GVector v2, GVector test)
{
	int b = test.DirectZ(v1, v0)*test.DirectZ(v2,v1)*test.DirectZ(v0,v2);
	if(b<=0)return false;
	return true;
}
static public double getZ(GVector nor, GVector base)
{
	GVector sn = CrossMult(base, nor);
	GVector pl =  CrossMult(nor , sn);
	double sign = DotMult(pl, base);
	double z = pl.z*base.Distance()/Math.sqrt(pl.x*pl.x+pl.y*pl.y);
	if(sign>=0)
	return z;
	return -z;
}
static public GVector mult(GVector v, double s)
{
  GVector r = new GVector();
  r.copy(v);
  r.scale(s);
  return  r;
}
static public int ComputeBarycenter(GVector[] v,GVector p,double c[])
{


	GVector e0 = differance(v[2],v[1]), e1 = differance(v[0],v[2]), e2 = differance(v[1],v[0]);
	e0.z = 0;
	e1.z = 0;
	e2.z = 0;

	GVector f0 = differance(p , v[1]), f1 = differance(p , v[2]), f2 =differance( p , v[0]);

	f0.z = 0;
	f1.z = 0;
	f2.z = 0;
  double a0 = CrossMult(e0,f0).Distance(),
          a1 = CrossMult(e1,f1).Distance(),
          a2 = CrossMult(e2,f2).Distance(),
          a =  CrossMult(e0,e2).Distance();

	c[0] = a0/a;
	c[1] = a1/a;
	c[2] = a2/a;

	return 1;
}
static public GVector GetSect(GVector v1, GVector v2, GVector l1, GVector l2)
{
   	double k11,k12,d1,k21,k22,d2;

	k11=v2.y-v1.y;
	k12=-(v2.x-v1.x);
	d1=(v2.y-v1.y)*v1.x-(v2.x-v1.x)*v1.y;

	k21=l2.y-l1.y;
	k22=-(l2.x-l1.x);
	d2=(l2.y-l1.y)*l1.x-(l2.x-l1.x)*l1.y;

	double x=(d1*k22-d2*k12)/(k11*k22-k12*k21);
	double y=(k11*d2-k21*d1)/(k11*k22-k12*k21);
	return new GVector(x,y,0.);
}
 static public Vector cutPoly(Vector poly, Vector line, boolean closed)
 {
   Vector lset = null;
   poly = trans(poly);
   line = trans(line);
   presect(poly, line,closed);
   poly = reTrans(poly);
   line = reTrans(line);
   setTag(poly, line, closed);
   if(closed)
   lset = extractCLineSeg(line);
   else
   lset = extractLineSeg(line);
   Vector pset = new Vector();
   pset.addElement(poly);
   return cutLine(pset, lset);
 }
 static void presect(Vector poly, Vector line, boolean closed)
 {
   int m = poly.size(),n = line.size();
   int k = n;
   if(!closed)n--;
   for(int i = 0; i < m; i++)
    for(int j = 0; j < n; j++)
    {
      Segment p1 = (Segment)poly.elementAt(i),
             p2 = (Segment)poly.elementAt((i+1)%m),
             l1 = (Segment)line.elementAt(j),
             l2 = (Segment)line.elementAt((j+1)%k);

      if(InterSect(p1.vex, p2.vex, l1.vex, l2.vex))
      {
         GVector p = GetSect(p1.vex, p2.vex, l1.vex, l2.vex);
         p.z = l1.vex.z;
         p1.add(p);
         l1.add(p);
      }
    }

 }
 static void setTag(Vector poly, Vector line, boolean closed)
 {
   if(poly == null||line == null)return;
   int n = line.size();
   int k = n;
   if(!closed)n--;
   for(int i = 0;i < n; i++)
   {
     GVector v1 = (GVector)line.elementAt(i),
     v2 = (GVector)line.elementAt((i+1)%k);
     GVector v = GeoMath.differance(v2,v1);
     v.scale(0.5);
     GVector p = GeoMath.add(v1, v);

     if(InsidePolygon(poly, p))
     v1.tag = 1;
     else v1.tag = 0;
   }

 }
 static Vector reTrans(Vector l)
 {
   if(l == null)return null;
   Vector result = new Vector();
   int n = l.size();
   for(int i = 0; i < n; i++)
   {
     Segment s = (Segment)l.elementAt(i);
     result.addElement(s.vex);
     for(int j = 0;j < s.sa.size(); j++)
     result.addElement((GVector)s.sa.elementAt(j));
   }
   return result;
 }
 static Vector trans(Vector l)
 {
   if(l == null)return null;
   Vector result = new Vector();
   int n = l.size();
   for(int i = 0; i < n;i++)
   result.addElement(new Segment((GVector)l.elementAt(i)));
   return result;
 }
 static Vector extractLineSeg(Vector line)
 {
 if(line == null) return null;
 Vector result = new Vector();
 Vector cur = null;
 byte pre = 0;
 int n = line.size();
 for(int i = 0;i < n-1;i++)
 {
   GVector v = (GVector)line.elementAt(i);
   if(pre == 1&&v.tag == 0)
   {
     cur.addElement(v);
     result.addElement(cur);
     pre = 0;
     continue;
   }
   if(pre == 0&&v.tag == 1)
   cur = new Vector();

   if(v.tag ==1)
   cur.addElement(v);
   pre = v.tag;

 }
 return result;
 }
  static Vector extractCLineSeg(Vector line)
 {
 if(line == null) return null;
 Vector result = new Vector();
 Vector cur = null;
 byte pre = 0;
 int n = line.size();
 int k = 0;
 while(((GVector)line.elementAt(k)).tag == 1)k++;k++;
 for(int i = 0;i <= n;i++)
 {
   GVector v = (GVector)line.elementAt((i+k)%n);
   if(pre == 1&&v.tag == 0)
   {
     cur.addElement(v);
     result.addElement(cur);
     pre = 0;
     continue;
   }
   if(pre == 0&&v.tag == 1)
   cur = new Vector();

   if(v.tag ==1)
   cur.addElement(v);
   pre = v.tag;

 }


 return result;
 }

  static Vector cutOne(Vector poly, Vector line)
 {
   int n = poly.size(), m = line.size();
   Object first = line.elementAt(0),
   last = line.elementAt(m-1);

   int f = 0, l = 0;
   for(int i = 0;i< n; i++)
   {
   if(poly.elementAt(i) == first)f = i;
   if(poly.elementAt(i) == last)l = i;
   }
   Vector vf = new Vector();
   for(int j = f; j!=l;j=(1+j)%n)
   vf.addElement(poly.elementAt(j));

   Vector vl = new Vector();
   for(int j = l; j!=f;j=(1+j)%n)
   vl.addElement(poly.elementAt(j));

   for(int j = 0;j<m;j++)
   {
     vf.addElement(line.elementAt(m - 1 - j));
     vl.addElement(line.elementAt(j));
   }
   Vector result = new Vector();
   result.addElement(vf);
   result.addElement(vl);
   return result;
 }


 static Vector cutLine(Vector pset, Vector lset)
 {
   int n = lset.size();
 //  int k = 0;

   Vector temp = pset;
//   temp.addElement(poly);
   for(int i = 0; i < n; i++)
   {
       Vector ttemp = new Vector();
       Vector line = (Vector)lset.elementAt(i);
       for(int j = 0; j < temp.size(); j++)
       {
          Vector tp = (Vector)temp.elementAt(j);
          if(intsectpoly(tp, line))
          {
          temp.removeElement(tp);
          Vector tttemp = cutOne(tp, line);
          ttemp.addElement(tttemp.elementAt(0));
          ttemp.addElement(tttemp.elementAt(1));
          break;
          }

       }
       for(int k = 0; k<ttemp.size();k++)
       temp.addElement(ttemp.elementAt(k));
   //    temp = ttemp;
   }
   return temp;
 }
static boolean intsectpoly(Vector poly, Vector line)
 {
   for(int i = 0; i< line.size();i++)
   for(int j = 0;j< poly.size(); j++)
   if(line.elementAt(i)==poly.elementAt(j))return true;
   return false;
 }
 static public boolean Intersect(Vector poly, Vector line)
 {

   int m = poly.size(), n = line.size();
   for(int i = 0; i< m;i++)
   for(int j = 0; j < n-1; j++)
   {
         GVector p1 = (GVector)poly.elementAt(i),
             p2 = (GVector)poly.elementAt((i+1)%m),
             l1 = (GVector)line.elementAt(j),
             l2 = (GVector)line.elementAt((j+1));

      if(InterSect(p1, p2, l1, l2))return true;
     }
     return false;
 }
 static public Vector cutPolySet(Vector pset, Vector lset)
 {
    if(pset == null||lset == null)return null;
    Vector result = new Vector();
    result = pset;
    Vector temp ;
    Vector ttemp;

    for(int j = 0; j < lset.size(); j++)
    {
      Vector line = (Vector)lset.elementAt(j);
      for(int i = 0; i< result.size();i++)
      {
        Vector poly = (Vector)result.elementAt(i);
        if(Intersect(poly, line))
        {
         result.removeElement(poly);
         temp = cutPoly(poly, line, false);
         for(int k = 0; k < temp.size(); k++)
           result.addElement(temp.elementAt(k));
        }
            }
    }
    return result;


 }
 static boolean pIntp(Vector p1, Vector p2)
 {
   int m = p1.size(), n = p2.size();
    for(int i = 0; i < m; i++)
      for(int j = 0; j < n; j++)
      {
        GVector v1 = (GVector)p1.elementAt(i),
                v2 = (GVector)p1.elementAt((i+1)%m),
                v3 = (GVector)p2.elementAt(j),
                v4 = (GVector)p2.elementAt((j+1)%n);
        if(InterSect(v1,v2,v3,v4))return true;
       }
    return false;
 }
 static boolean pAInsp(Vector p1, Vector p2)
 {
   int n = p1.size();
   for(int i = 0; i < n; i++)
   if(!InsidePolygon(p2, (GVector)p1.elementAt(i)))return false;

   return true;
 }
  static boolean pInsp(Vector p1, Vector p2)
 {
   GVector v = (GVector)p1.elementAt(0);
   if(InsidePolygon(p2, v))return true;

   return false;
 }

 static boolean empty(Vector v)
 {
   if(v == null)return true;
   if(v.size() == 0)return true;
   return false;
 }
 static public int pRp(Vector p1, Vector p2)
 {
    if(empty(p1)||empty(p2))return -2;
    if(pIntp(p1, p2))return -1;
    if(pInsp(p1, p2))return 1;
    if(pInsp(p2, p1))return 2;
    return 0;
 }
 static public boolean toonear(GVector v1, GVector v2)
 {
   return toonear(v1, v2.x, v2.y);
 }
 static public boolean closed(Vector line)
 {
   return toonear((GVector)line.elementAt(0),(GVector)line.elementAt(line.size()-1));
 }
 static public boolean trim(Vector line)
 {
   boolean closed = closed(line);
   if(closed)line.removeElementAt(line.size()-1);
   return closed;
 }
}

