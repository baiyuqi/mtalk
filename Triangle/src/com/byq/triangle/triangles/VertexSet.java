package com.byq.triangle.triangles;
import java.awt.*;
import java.util.*;
class VertexSet
{ ListParam listp;
  Vector[][] vlist;
  Vector lnlist=new Vector();
  TrigonNet pNet;
  int current=-1;
  public VertexSet()
  {
    pNet=null;
  }
  public VertexSet(TrigonNet net)
  {
    pNet=net;
  }
  public void Init(ListParam p)
  { listp = p;

    vlist=new Vector[listp.nx][listp.ny];
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
        vlist[i][j]=new Vector();
  }

  public void InsertVertex(Vertex vertex)
  {

  double x,y;
	  x=vertex.coord.x-listp.startx;
	  y=vertex.coord.y-listp.starty;
	  int r=(int)(x/listp.cellsize);
	  int c=(int)(y/listp.cellsize);
	  if(r<0||r>=listp.nx||c<0||c>=listp.ny)
	    return;

	  Vector pv=vlist[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Object p=pv.elementAt(i);
      if(p==vertex)return;
      }
    pv.add(vertex);pNet.np.nov++;lnlist.add(vertex);

  }
  public void SetConvex(Convex con)
  {
    for(int i=0;i<lnlist.size();i++)
    {
       Vertex v=(Vertex)lnlist.elementAt(i);
          con.AddVertex(v);
          }
  }
 public void CreateNet()
 {
    for(int i=0;i<lnlist.size();i++)
    {
       Vertex v=(Vertex)lnlist.elementAt(i);
       pNet.IA.InsertToNet(v);
     }
 }

public Vertex PickVertex(double x,double y)
{
  double dx,dy;
	  dx=x-listp.startx;
	  dy=y-listp.starty;
	  int r=(int)(dx/listp.cellsize);
	  int c=(int)(dy/listp.cellsize);
	  if(r<0||r>=listp.nx||c<0||c>=listp.ny)
	    return null;

	  Vector pv=vlist[r][c];
    int n=pv.size();

		int x1,x2,y1,y2;
		x1=(r-2)>=0?r-2:0;
		x2=(r+2)<listp.nx?r+2:listp.nx;
		y1=(c-2)>=0?c-2:0;
		y2=(c+2)<listp.ny?c+2:listp.ny;

		for(int i=x1;i<x2;i++)
		for(int j=y1;j<y2;j++)
		{  	Vector p=vlist[i][j];
        for(int k=0;k<p.size();k++)
        {
          Vertex v=(Vertex)p.elementAt(k);
  				if(Math.abs(v.coord.x-x)<=0.0005&&Math.abs(v.coord.y-y)<=0.0005)
  				return (Vertex)v;
			}

		}
		return null;
}
public boolean GotoFirst()
{
if(lnlist.size()==0)return false;
 current=0 ; return true;
/*


return (Vertex)lnlist.elementAt(0);*/
}
public boolean Next()
{  if(current==-1)return false;
   if(current==lnlist.size()-1){current = -1;return false; }
   current++;return true;

}
public Vertex GetCurrent()
{
if(current==-1)return null;
   return (Vertex)lnlist.elementAt(current);
}
public Vertex GetVertex(int id)
{
    for(int i=0;i<lnlist.size();i++)
    {
       Vertex v=(Vertex)lnlist.elementAt(i);
          if(v.id==id)
          return v;
     }
      return null;
}
public Vertex GetIndexedVertex(int ind)
{
  if(ind>=lnlist.size())
    return null;
  return (Vertex)lnlist.elementAt(ind);
}
  public void setNormal()
  {
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
       	Vector p=vlist[i][j];
        for(int k=0;k<p.size();k++)
        {
          Vertex vex=(Vertex)p.elementAt(k);
          ((Vertex)p.elementAt(k)).setNormal(pNet.CA);
			  }
      }
 
  }
}
