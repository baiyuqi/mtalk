package com.byq.triangle.triangles;
//import Triangles.TriangleBasic.*;
class QueueNode extends Object{
  Trigon trigon;
  byte vid;
//  QueueNode next,prev;
  public QueueNode() {
  }
  public  boolean Exchangable()
  {
    Trigon trigon1=trigon;
  	byte vid1=vid;
  	Trigon trigon2=trigon1.nbtri[vid1];

  	byte vid2=trigon.nbvex[vid1];
  	Vertex vertex=trigon2.vex[vid2];

  	double dx0=(trigon1.vex[0].coord.x-vertex.coord.x);
  	double dx1=(trigon1.vex[1].coord.x-vertex.coord.x);
  	double dx2=(trigon1.vex[2].coord.x-vertex.coord.x);
  	double dz0=(trigon1.vex[0].coord.y-vertex.coord.y);
  	double dz1=(trigon1.vex[1].coord.y-vertex.coord.y);
  	double dz2=(trigon1.vex[2].coord.y-vertex.coord.y);
  	double t = (dx0*dz1-dx1*dz0)*(dx2*dx2+dz2*dz2)
			  -(dx0*dz2-dx2*dz0)*(dx1*dx1+dz1*dz1)
			  +(dx1*dz2-dx2*dz1)*(dx0*dx0+dz0*dz0);
  	if(t==0.0)return true;
  	if(t<0)return false;
  	return true;
  }
  public void RemoveEdge()
  {
  	Trigon t=trigon;
  	byte vid=this.vid;
  	if(t.etc[vid]==this)
  	t.etc[vid]=null;
  	byte vid1=t.nbvex[vid];
  	Trigon t1=t.nbtri[vid];
  	if(t1.etc[vid1]==this)
  	t1.etc[vid1]=null;
  }

}
