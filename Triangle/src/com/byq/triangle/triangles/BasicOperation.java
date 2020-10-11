package com.byq.triangle.triangles;
import java.awt.Image.*;

import com.byq.triangle.geomath.*;
class BasicOperation {
  QueryList qlist;
  VertexSet vlist;
  NetParam np;

  public BasicOperation(VertexSet vl,QueryList ql,NetParam npa) {
  qlist=ql;
  vlist=vl;
  np=npa;

  }
  public void DeleteTrigon(Trigon trigon){
  	qlist.Delete(trigon);

  	Trigon[] p=new Trigon[3];
  	byte[] vid=new byte[3];
  	for(byte i=0;i<3;i++)
    {
   		p[i]=trigon.nbtri[i];
  		vid[i]=trigon.nbvex[i];
  		DeleteEdge(trigon,i);
  		if(p[i]!=null)
  		{
  			p[i].nbvex[vid[i]]=-1;
  			p[i].nbtri[vid[i]]=null;
  		}
  	}
    np.not--;
  }
  public void InsertTrigon(Trigon trigon)
  {
   	Trigon[] p=new Trigon[3];
  	byte[] vid=new byte[3];
  	for(byte i=0;i<3;i++)
  	{
  		p[i]=trigon.nbtri[i];
  		vid[i]=trigon.nbvex[i];
  		if(p[i]!=null)
  		{
  			p[i].nbvex[vid[i]]=i;
  			p[i].nbtri[vid[i]]=trigon;
  		}
  	}
    qlist.Insert(trigon);
    np.not++;
  }
  private void DeleteEdge(Trigon trigon,byte vid)
  {
  	if(trigon.etc[vid]!=null)
  		Queue.Delete(trigon.etc[vid]);

  	Trigon pt=trigon.nbtri[vid];
  	if(pt==null)
  		return;

  	byte vid1=trigon.nbvex[vid];

  	if(pt.etc[vid1]!=null)
  		Queue.Delete(pt.etc[vid1]);
  }
  public void CheckTrigonEdge(Trigon trigon)
  {
  	Trigon[] pn=new Trigon[3];
  	byte[] vid=new byte[3];
  	QueueNode[] ec=new QueueNode[3];

  	for(byte i=0;i<3;i++)
  	{
  		vid[i]=trigon.nbvex[i];
  		pn[i]=trigon.nbtri[i];
		  if(pn[i]!=null)
		  {
			  ec[i]=pn[i].etc[vid[i]];
			  if(trigon.etc[i]==null&&ec[i]==null)
			  {
				  QueueNode edge=new QueueNode();
				  trigon.etc[i]=edge;
				  edge.trigon=trigon;
				  edge.vid=i;
				  Queue.Enter(edge);
			  }
		  }
	  }
  }
  public double GetHeight(double x, double y)
  {
	  Vertex vertex=new Vertex();
	  vertex.coord.x=x;
	  vertex.coord.y=y;
	  Trigon pt=qlist.GetTrigonH(vertex);
    if(pt==null)return -100000;
	  return MyMath.Height(vertex,pt);
  }
}
