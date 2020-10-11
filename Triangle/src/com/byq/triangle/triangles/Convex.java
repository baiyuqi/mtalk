package com.byq.triangle.triangles;
import java.awt.*;

import com.byq.triangle.geomath.*;
class Convex {
  TrigonNet pNet;
  Vertex[] ppVertex;
  int nov,noc;
  /*
    nov:Nomber of vertice from which to get the convex. noc:Number of vertice
    in the resultant convex;
  */
  public Convex(TrigonNet net)
  {
    pNet = net;
  }
  public void SetData()
  {
    ppVertex = new Vertex[pNet.np.nov+2];
    pNet.vlist.SetConvex(this);
  }
  public void AddVertex(Vertex vertex)
  {
    ppVertex[nov++]=vertex;
  }
  private int GetIndexOfMinY()
  {
  	double y=ppVertex[0].coord.y;
  	int index=0;
  	for(int i=0;i<nov;i++)
   		if(y>ppVertex[i].coord.y)
  		{
  			y=ppVertex[i].coord.y;
  			index=i;
  		}
  	return index;
  }
  public void GetConvex()
  {
/*  	Vector v;
  	int n=nov;
  	int j=GetIndexOfMinY();
  	double th,th1,thmin=0;
	  ppVertex[n]=ppVertex[j];
  	int k=-1;
  	Vertex t=null;
  	do
  	{
  		k++;
  		t=ppVertex[k];
  		ppVertex[k]=ppVertex[j];
		  ppVertex[j]=t;
		  th=thmin;
		  thmin=7;
		  j=0;
		  for(int i=k+1;i<n+1;i++)
		  {
			  v=GeoMath.differance(ppVertex[i].coord,ppVertex[k].coord);
			  th1=v.theta();
  			if(th1<thmin&&th1>=th)
  			{
  				j=i;
  				thmin=th1;
  			}
  		}
  		int y=1;
  	}while(j!=n);
  	k++;
  	t=ppVertex[k];
  	ppVertex[k]=ppVertex[j];
	  ppVertex[j]=t;

	  noc=k;
    Vertex[] p=new Vertex[noc];
    for(int i=0;i<noc;i++)
    {
      p[i]=ppVertex[i];
      p[i].inserted=1;
    }
    ppVertex = p;


*/




	GVector v;
	int n=nov;
	int j=this.GetIndexOfMinY();
	double th,th1,thmin=0;
	ppVertex[n]=ppVertex[j];
	int k=-1;
	Vertex t=null;
	do
	{
		k++;
		t=ppVertex[k];
		ppVertex[k]=ppVertex[j];
		ppVertex[j]=t;
		th=thmin;
		thmin=11;
		j=0;
		double dist=1e20;
		int pi=-1;
		for(int i=k+1;i<n+1;i++)
		{
			v=GeoMath.differance(ppVertex[i].coord,ppVertex[k].coord);
			th1=v.theta();
			if(th1==th)
			{
				if(v.Distance()<dist)
				{
					dist=v.Distance();
					j=i;
					thmin=th1;
				}
			}
			if(th1<thmin&&th1>th)
			{
				j=i;
				thmin=th1;
			}

		}
//u:;
		int y=1;
	}while(j!=n);
	k++;
	t=ppVertex[k];
	ppVertex[k]=ppVertex[j];
	ppVertex[j]=t;

	noc=k;

  }
  public void draw(Graphics g,DrawParam p)
  {
    for(int i=0;i<noc;i++)
    {
      double x0,y0,x1,y1;
      x0=ppVertex[i].coord.x;
      y0=ppVertex[i].coord.y;
      x1=ppVertex[(i+1)%noc].coord.x;
      y1=ppVertex[(i+1)%noc].coord.y;
      g.drawLine(
                (int)((x0+p.x0)*p.r),
                (int)((y0+p.y0)*p.r),
                (int)((x1+p.x0)*p.r),
                (int)((y1+p.y0)*p.r)
                );
    }
  }
  public void reset()
  {
    ppVertex = null;
    noc=nov=0;
  }
  public void ExportData(Polygon poly)
  {
    for(int i=0;i<noc;i++)
      poly.insertNode(ppVertex[i]);
    reset();

  }
}