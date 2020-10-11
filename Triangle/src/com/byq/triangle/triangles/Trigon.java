package com.byq.triangle.triangles;
import java.awt.*;

import com.byq.triangle.geomath.*;
public class Trigon
{
  private int id;
  public GVector heart;
  public QueueNode[] etc;
  public Vertex[] vex;
  public byte[] nbvex;
  public Trigon[] nbtri;
  public Trigon next,prev;
  boolean visited;
  boolean passed;

  GVector normal;
	GVector center;
	double[] cenBary;
	GVector[] intersect;
	double[] edgeBary;
	GVector[] coef;

  public Trigon()
  { passed=false;
    visited=false;
    heart=new GVector();
    etc=new QueueNode[3];
    nbvex=new byte[3];
    nbtri=new Trigon[3];
    cenBary = new double[3];
    intersect = new GVector[3];
    edgeBary = new double[3];
    vex=new Vertex[3];
    for(int i=0;i<3;i++)
    {
      nbvex[i]=-1;
      nbtri[i]=null;
    }
  }
  public Trigon(GVector[] co)
  {
    passed=false;
    visited=false;
    heart=new GVector();
    etc=new QueueNode[3];
    nbvex=new byte[3];
    nbtri=new Trigon[3];
    cenBary = new double[3];
    intersect = new GVector[3];
    edgeBary = new double[3];
    vex=new Vertex[3];
    for(int i=0;i<3;i++)
    {
      nbvex[i]=-1;
      nbtri[i]=null;
    }
    for(int i = 0;i<3;i++)
    {
    vex[i] = new Vertex(co[i]);
    }
  }
  public void draw(Graphics g,DrawParam p)
  {
    int x1,x2,y1,y2;
    for( byte i=0;i<3;i++)
    {
      x1=(int)((vex[i].coord.x+p.x0)*p.r);
      y1=(int)((vex[i].coord.y+p.y0)*p.r);
      x2=(int)((vex[(i+1)%3].coord.x+p.x0)*p.r);
      y2=(int)((vex[(i+1)%3].coord.y+p.y0)*p.r);
      g.drawLine(x1,y1,x2,y2);
    }
  }
  public byte VexPosition(Vertex vertex)
  {
  	double x,y;
  	x=vertex.coord.x;
	  y=vertex.coord.y;

  	GVector V=new GVector(x,y,0.0);

  	GVector v0=vex[0].coord;
  	GVector v1=vex[1].coord;
  	GVector v2=vex[2].coord;
  	byte dir01=V.DirectZ(v0,v1);
  	byte dir12=V.DirectZ(v1,v2);
  	byte dir20=V.DirectZ(v2,v0);
  	if(dir20==0 && dir01==0)return -1;    //On aVertex[0]
  	if(dir01==0 && dir12==0)return -2;   //On aVertex[1]*rx*ry*rx
  	if(dir12==0 && dir20==0)return -3;    //On aVertex[2*rx*ry*ry]
  	if(dir01==0 && dir12*dir20>0)return 2;//On side opps*rx*ryite aVertex[2]
  	if(dir12==0 && dir20*dir01>0)return 0;//On side oppsite aVertex[0]
	  if(dir20==0 && dir01*dir12>0)return 1;//On side oppsite aVertex[1]

	  if(dir01>0 && dir12>0 && dir20>0 ||
	   dir01<0 && dir12<0 && dir20<0)return 3;//>2: Inside
  	return -4;							      //<-3:Outside
	  //判断点在多边形内部；

  }
  public double maxx()
  {
    double max=vex[0].coord.x;
    for(int i=1;i<3;i++)
     if(max<vex[i].coord.x)max= vex[i].coord.x;

    return max;
  }
   public double maxy()
  {
    double may=vex[0].coord.y;
    for(int i=1;i<3;i++)
     if(may<vex[i].coord.y)may= vex[i].coord.y;

    return may;
  }
    public double minx()
  {
    double minx=vex[0].coord.x;
    for(int i=1;i<3;i++)
     if(minx>vex[i].coord.x)minx= vex[i].coord.x;

    return minx;
  }
   public double miny()
  {
    double miny=vex[0].coord.y;
    for(int i=1;i<3;i++)
     if(miny>vex[i].coord.y)miny= vex[i].coord.y;

    return miny;
  }
   public double minz()
  {
    double minz=vex[0].coord.z;
    for(int i=1;i<3;i++)
     if(minz>vex[i].coord.z)minz= vex[i].coord.z;

    return minz;
  }
   public double maxz()
  {
    double maz=vex[0].coord.z;
    for(int i=1;i<3;i++)
     if(maz<vex[i].coord.z)maz= vex[i].coord.z;

    return maz;
  }
   public double dim()
  {
    double dim;
    double dx=maxx()-minx();
    dim=dx;
    double dy=maxy()-miny();
    if(dx>dy)return dx;
    else
    return dy;
  }
  public void SetHeart()
  {
  	double k11,k12,k21,k22,d1,d2;
	double[] x=new double[3],y=new double[3];
	for(int i=0;i<3;i++)
	{
		x[i]=vex[i].coord.x;
		y[i]=vex[i].coord.y;
	}
	k11=(x[0]-x[1])*2.;
	k12=(y[0]-y[1])*2.;
	k21=(x[0]-x[2])*2.;
	k22=(y[0]-y[2])*2.;

	d1=(x[0]-x[1])*(x[0]+x[1])+(y[0]-y[1])*(y[0]+y[1]);
	d2=(x[0]-x[2])*(x[0]+x[2])+(y[0]-y[2])*(y[0]+y[2]);

	heart.x=(d1*k22-d2*k12)/(k11*k22-k12*k21);
	heart.y=(d1*k21-d2*k11)/(k12*k21-k22*k11);
  }
  public void DrawVoronoi(Graphics g,DrawParam p)
  {


	GVector v1,v2,v;
	v1=heart;
	Trigon pn;
	for(byte i=0;i<3;i++)
	{
  v=this.vex[i].coord;
   g.drawLine((int)((v.x+p.x0)*p.r),(int)((v.y+p.y0)*p.r),(int)((v.x+p.x0)*p.r),(int)((v.y+p.y0)*p.r));
		pn=nbtri[i];
		if(pn!=null)
		{
 			v2=pn.heart;
			g.drawLine((int)((v1.x+p.x0)*p.r),(int)((v1.y+p.y0)*p.r),(int)((v2.x+p.x0)*p.r),(int)((v2.y+p.y0)*p.r));

		}
	} }
  public boolean IsVertex(Vertex vertex)
  {
  for(int i=0;i<3;i++)
  {
    if(this.vex[i]==vertex)return true;
  }
  return false;
  }
  public byte GetVertex(Vertex vertex)
  {
    for(byte i=0;i<3;i++)
    if(vex[i]==vertex)return i;
    return -1;
  }
/*
	Vector s,e;
	s=heart;
//	End.Unitize();
	e=s+End*0.01;

	v1.m_vertex=s;
	v2.m_vertex=e;
	DrawLine(pDC,&v1,&v2,sr,da);
	pDC->SelectObject(oldpen);*/
 public Vertex get(int index)
 {
 if(index>=0&&index<=2)
 return vex[index];
 else return null;
 }
 public void setNormal()
 {
 	GVector v0,v1,v2;
	v0=vex[0].coord;
	v1=vex[1].coord;
	v2=vex[2].coord;


	GVector vector1,vector2;
	vector1=GeoMath.differance(v1, v0);
	vector1.unitize();
	vector2=GeoMath.differance(v2, v1);
	vector2.unitize();
	normal=GeoMath.CrossMult(vector1, vector2);
	normal.unitize();
 }
private void ComputeCoef()
{
  coef = new GVector[19];
  for(int i = 0; i< 19;i++)
    coef[i] = new GVector();
   double[] p = new double[3];
  GVector[] v = new GVector[3];
  GVector[] n = new GVector[3];
  for(int i = 0; i< 3;i++)
  {
  v[i] = new GVector();
  v[i].copy(vex[i].coord);
  n[i] = new GVector();
  n[i].copy(vex[i].normal);
  p[i] = v[i].z;

  coef[2*(i+1)].copy(v[i]);
  v[i].z = 0;
  }

	GVector target;
	double dot;


	// CV0, VE01, VE02

  for(int i = 0; i<3; i++)
  {
	GVector base = GeoMath.differance(center,v[i]);
  base.scale(0.5);
	target =GeoMath.add(v[i] , base);
	double z = GeoMath.getZ(n[i], base);
	target.z = p[i] + z;
	coef[14+2*i].copy(target);

	base = GeoMath.differance(intersect[(i+2)%3] , v[i]);
  base.scale(0.5);
	target = GeoMath.add (v[i], base);
	z = GeoMath.getZ(n[i], base);
	target.z = p[i] + z;
	coef[8+2*i].copy(target);

	base = GeoMath.differance(intersect[(i+1)%3] , v[i]);
  base.scale(0.5);
	target = GeoMath.add (v[i], base);
	z = GeoMath.getZ(n[i], base);
	target.z = p[i] + z;
	coef[7+2*i].copy(target);
  }
  for(int i = 0; i<3; i++)
  {
  int k = (i+2)%3;
  GVector t1 = GeoMath.mult(coef[7+2*i], 1.0-edgeBary[(1+i)%3]),
          t2 = GeoMath.mult(coef[8+2*k], edgeBary[(1+i)%3]);
  coef[1+2*i] =GeoMath.add(t1,t2);
  t1 = GeoMath.mult(coef[14+2*i], 1.0-edgeBary[(1+i)%3]);
  t2 = GeoMath.mult(coef[14+2*k], edgeBary[(1+i)%3]);
  coef[13+2*i] =GeoMath.add(t1,	t2);

  }
  GVector t1 = GeoMath.mult(coef[14], cenBary[0]),
          t2 = GeoMath.mult(coef[16], cenBary[1]),
          t3 = GeoMath.mult(coef[18], cenBary[2]);
  coef[0].copy(GeoMath.add(GeoMath.add(t1,t2),t3));


}
public void ComputeIntersect()
{
	for(int i = 0; i< 3; i++)
	{
		GVector cen = new GVector();
    cen.copy(center);
		cen.z = 0;
		GVector p1 = new GVector(),p2  = new GVector();
    p1.copy( vex[(i+1)%3].coord    );

    p2.copy(vex[(i+2)%3].coord);
		p1.z = p2.z = 0;
		if(nbtri[i]==null)
    {
			intersect[i]=GeoMath.add(p1,p2);
      intersect[i].scale(.5);
      }
		else
		{
			GVector adjc = new GVector();
      adjc.copy(nbtri[i].center);
			adjc.z = 0;
			intersect[i] = GeoMath.GetSect(p1,p2,cen,adjc);
		}
		GVector p = intersect[i];
		if((p2.x - p1.x)!=0)
			edgeBary[i] = (p.x - p2.x)/(p1.x - p2.x);
		else
			edgeBary[i] = (p.y - p2.y)/(p1.y - p2.y);
	}
}
private GVector clone(GVector v)
{
  GVector r = new GVector();
  r.copy(v);
  return r;
}
void ComputeICenter()
{
	GVector p0 = clone(vex[0].coord);
	GVector p1 = clone(vex[1].coord);
	GVector p2 = clone(vex[2].coord);

	// edges of the triangle
	p0.z = 0;
	p1.z = 0;
	p2.z = 0;

	GVector e0 = GeoMath.differance(p2,p1), e1 = GeoMath.differance(p0,p2),
          e2 = GeoMath.differance(p1,p0);


	// lengths of the edges
	double len0 = e0.Distance();
	double len1 = e1.Distance();
	double len2 = e2.Distance();

	// barycentric coordinates of center
	double perimeter = len0+len1+len2;
	len0 /= perimeter;
	len1 /= perimeter;
	len2 /= perimeter;

	// center of inscribed circle
  p0.scale(len0);
  p1.scale(len1);
  p2.scale(len2);

  center = GeoMath.add(GeoMath.add(p0,p1),p2);
	cenBary[0] = len0;
	cenBary[1] = len1;
	cenBary[2] = len2;
}
private GVector[] getControlPoint(int index)
{
  GVector bez[] = new GVector[6];
   bez[0] = clone(coef[0]);
	 bez[1] = clone(coef[12+index]);
	 bez[2] = clone(coef[13+index%6]);
	 bez[3] = clone(coef[index]);
	 bez[4] = clone(coef[6+index]);
	 bez[5] = clone(coef[1+index%6]);
   return bez;
}
private double getValue(GVector[] bez, double[] bary, GVector normal)
{
  GVector F = null;
	GVector XU = matrixMult(bez[0],bez[1],bez[2],bary[0],bary[1],bary[2]);
	GVector XV = matrixMult(bez[1],bez[3],bez[4],bary[0],bary[1],bary[2]);
	GVector XW = matrixMult(bez[2],bez[4],bez[5],bary[0],bary[1],bary[2]);
  F = matrixMult(XU,XV,XW,bary[0], bary[1],bary[2]);

	// evaluate barycentric derivatives of F and get normal to surface
	GVector Tan1 = GeoMath.differance(XU,XW), Tan2 = GeoMath.differance(XV,XW);
	normal = GeoMath.CrossMult(Tan1,Tan2);
  normal.unitize();
  return F.z;
}
GVector[] getSub(int index)
{
  GVector[] sub= null;
  try{
  sub = new GVector[3];
  sub[0] = clone(coef[0]);
  sub[1] = clone(coef[index]);
	sub[2] = clone(coef[1+index%6]);
  }
  catch(Exception e)
  {
  e = e;
  }
  return sub;
}

private int getIndex(double u, double v, GVector[] tri)
{
  GVector test = new GVector(u,v,0);
	Vertex p = new Vertex();;
	p.coord = test;

	int index = 0;
  GVector[] sub = null;
	for (index = 1; index <= 6; index++)
	{

		sub = getSub(index);
		Trigon dum = new Trigon(sub);
		if ( dum.VexPosition(p)>=-3)
			break;

	}
  for(int i = 0;i<3;i++)
  tri[i] = sub[i];
  return index;
}
double Evalue(double u, double v,GVector normal)
{

  GVector test = new GVector(u,v,0);
	double bary[] = new double[3];


	// determine which of the six subtriangles contains the target point
  GVector[] tri = new GVector[3];
  int index = getIndex(u,v,tri);
	if ( index == 7 )  // point not in triangle
		return 0;

	// compute barycentric coordinates with respect to subtriangle
	GeoMath.ComputeBarycenter(tri,test,bary);
	// fetch Bezier control points
  GVector bez[] = getControlPoint(index);

	// evaluate Bezier quadratic
  double z = getValue(bez, bary, normal);

  return z;


}
private GVector matrixMult(GVector v1, GVector v2,GVector v3,double b1,double b2, double b3)
{
  GVector t1 = GeoMath.mult(v1,b1),
          t2 = GeoMath.mult(v2,b2),
          t3 = GeoMath.mult(v3,b3);
  return GeoMath.add(GeoMath.add(t1,t2),t3);
}
public void prepareSurf()
{
//  this.ComputeICenter();
  this.ComputeIntersect();
  this.ComputeCoef();
}
}