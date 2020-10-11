package com.byq.triangle.triangles;
import java.awt.*;
import java.util.*;

import com.byq.triangle.triangles.*;
class QueryList
{
  ListParam listp;
  Vector[][] list;
  Vector lnlist=new Vector();
  TrigonNet pNet;
  Vector TrigonIndex=new Vector();
  int index=-1;
  public QueryList()
  {
    listp=null;
    pNet=null;

  }
  public QueryList(TrigonNet net)
  {
    listp=null;
    pNet=net;
  }
  public void Init(ListParam p)
  {
    listp=p;
    list=new Vector[listp.nx][listp.ny];
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
        list[i][j]=new Vector();    


  }
  public void Insert(int r,int c, Trigon trigon)
  {
    Vector pv=list[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Object p=pv.elementAt(i);
      if(p==trigon)return;
      }
    pv.addElement(trigon);
  }
  public Trigon GetTrigon(Vertex vertex)
  {
    double x,y;
	  x=vertex.coord.x-listp.startx;
	  y=vertex.coord.y-listp.starty;
	  int r=(int)(x/listp.cellsize);
	  int c=(int)(y/listp.cellsize);
	  if(r<0||r>=listp.nx||c<0||c>=listp.ny)
		  return null;
    Vector pv=list[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Trigon p=(Trigon)pv.elementAt(i);
      if(p.VexPosition(vertex)==3)return p;
      }
  	return null;
  }
  public Trigon GetTrigonH(Vertex vertex)
  {
    double x,y;
	  x=vertex.coord.x-listp.startx;
	  y=vertex.coord.y-listp.starty;
	  int r=(int)(x/listp.cellsize);
	  int c=(int)(y/listp.cellsize);
	  if(r<0||r>=listp.nx||c<0||c>=listp.ny)
		  return null;

    Vector pv=list[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Trigon p=(Trigon)pv.elementAt(i);
      if(p.VexPosition(vertex)>=-3)return p;
      }

  	return null;
  }
  public void Insert(Trigon trigon)
  {
  	double x1,x2,y1,y2;
  	x1=trigon.minx()-listp.startx;
  	x2=trigon.maxx()-listp.startx;
  	y1=trigon.miny()-listp.starty;
  	y2=trigon.maxy()-listp.starty;

  	int nx1,nx2,ny1,ny2;
  	nx1=(int)(x1/listp.cellsize);
  	nx2=(int)(x2/listp.cellsize);
  	ny1=(int)(y1/listp.cellsize);
  	ny2=(int)(y2/listp.cellsize);

  	for(int i=nx1;i<=nx2;i++)
  		for(int j=ny1;j<=ny2;j++)
  			Insert(i,j,trigon);
    TrigonIndex.addElement(trigon);
    trigon.SetHeart();
   }
  protected  boolean Delete(int r,int c,Trigon trigon)
  {


    Vector pv=list[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Trigon p=(Trigon)pv.elementAt(i);
      if(p==trigon)
      {
        pv.remove(i);
      return true;    }
      }
      return false;
  }
  public void clear( int r,int c,Trigon trigon)
  {
    while(Delete(r,c,trigon));
  }

 protected void Delete(Trigon trigon)
 {
   	double x1,x2,y1,y2;
  	x1=trigon.minx()-listp.startx;
  	x2=trigon.maxx()-listp.startx;
  	y1=trigon.miny()-listp.starty;
  	y2=trigon.maxy()-listp.starty;

  	int nx1,nx2,ny1,ny2;
  	nx1=(int)(x1/listp.cellsize);
  	nx2=(int)(x2/listp.cellsize);
  	ny1=(int)(y1/listp.cellsize);
  	ny2=(int)(y2/listp.cellsize);

  	for(int i=nx1;i<=nx2;i++)
  		for(int j=ny1;j<=ny2;j++)
  			Delete(i,j,trigon);
    TrigonIndex.remove(trigon);
 }
  public void draw(Graphics g,DrawParam dp)
 {
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
        Vector pv=list[i][j];
        int n=pv.size();
        for(int k=0;k<n;k++)
        {
          Trigon p=(Trigon)pv.elementAt(k);
          p.draw(g,dp);
        }
      }
 }
 public void drawv(Graphics g,DrawParam dp)
 {
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
         Vector pv=list[i][j];
         int n=pv.size();
         for(int k=0;k<n;k++)
         {
            Trigon p=(Trigon)pv.elementAt(k);
            p.DrawVoronoi(g,dp);
          }
      }
 }
 public Trigon GetCrossTrigon(double z)
 {
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
        double zmin,zmax;

         Vector pv=list[i][j];
         int n=pv.size();
         for(int k=0;k<n;k++)
         {
            Trigon p=(Trigon)pv.elementAt(k);
          zmin=p.minz();
          zmax=p.maxz();
          if(z>=zmin&&z<=zmax&&!p.visited)
          {
            p.visited=true;
            return p;
          }
         }
      }
    return null;
 }
 public void reset()
 {
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
         Vector pv=list[i][j];
         int n=pv.size();
         for(int k=0;k<n;k++)
         {
            Trigon p=(Trigon)pv.elementAt(k);
            p.visited=false;
          }
       }
 }
public Trigon GetOneTrigon(Vertex vertex)
{
    if(vertex==null)return null;
    double x,y;
	  x=vertex.coord.x-listp.startx;
	  y=vertex.coord.y-listp.starty;
	  int r=(int)(x/listp.cellsize);
	  int c=(int)(y/listp.cellsize);
	  if(r<0||r>=listp.nx||c<0||c>=listp.ny)
		  return null;
    Vector pv=list[r][c];
    int n=pv.size();
    for(int i=0;i<n;i++)
    {
      Trigon p=(Trigon)pv.elementAt(i);
      if(p.IsVertex(vertex))return p;
      }
  	return null;

/*
	if((nx-1)>=0)
	{
		p=m_querylist[nx-1][ny].head;
		if(p==NULL)
			return NULL;
		while(p!=NULL)
		{
			if(p->trigon->IsVertex(vertex))
				return p->trigon;
			p=p->pNext;
		}
	}
	if((nx+1)<m_x)
	{
		p=m_querylist[nx+1][ny].head;
		if(p==NULL)
			return NULL;
		while(p!=NULL)
		{
			if(p->trigon->IsVertex(vertex))
				return p->trigon;
			p=p->pNext;
		}
	}

	if((ny-1)>=0)
	{
		p=m_querylist[nx][ny-1].head;
		if(p==NULL)
			return NULL;
		while(p!=NULL)
		{
			if(p->trigon->IsVertex(vertex))
				return p->trigon;
			p=p->pNext;
		}
	}
	if((ny+1)<m_y)
	{
		p=m_querylist[nx][ny+1].head;
		if(p==NULL)
			return NULL;
		while(p!=NULL)
		{
			if(p->trigon->IsVertex(vertex))
				return p->trigon;
			p=p->pNext;
		}
	}



//	AfxMessageBox("发现孤立点，将其删除。");

	return NULL;  */

}
public Trigon GetFirstTrigon()
{ if(TrigonIndex.size()==0)return null;
  index=1;
  return (Trigon)TrigonIndex.elementAt(0);
}
public Trigon GetNext()
{ if(index==-1)return null;
  if(index==TrigonIndex.size()){index=-1;return null;}
  return (Trigon)TrigonIndex.elementAt(index++);

}
 public void setNormalandCenter()
 {
     reset();
    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
         Vector pv=list[i][j];
         int n=pv.size();
         for(int k=0;k<n;k++)
         {

            Trigon p=(Trigon)pv.elementAt(k);
            if(p.visited == true)
            continue;
            p.visited = true;
            p.setNormal();
            p.ComputeICenter();
          }
       }
       reset();
 }
  public void setCoef()
 {

    for(int i=0;i<listp.nx;i++)
      for(int j=0;j<listp.ny;j++)
      {
         Vector pv=list[i][j];
         int n=pv.size();
         for(int k=0;k<n;k++)
         {

            Trigon p=(Trigon)pv.elementAt(k);
            if(p.visited == true)
            continue;
            p.visited = true;
            p.prepareSurf();
          }
       }
       reset();
 }

}
