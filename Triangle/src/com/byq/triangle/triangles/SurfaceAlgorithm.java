package com.byq.triangle.triangles;
import com.byq.triangle.geomath.*;
public class SurfaceAlgorithm {
  QueryList qlist;
  VertexSet vlist;
  NetParam np;

  public SurfaceAlgorithm(VertexSet vl,QueryList ql,NetParam npa) {
  qlist=ql;
  vlist=vl;
  np=npa;
  }
  private void setNormal()
  {
    qlist.setNormalandCenter();
    vlist.setNormal();
  }
  private void setCoef()
  {
    qlist.setCoef();

  }
  public void setSurface()
  {
    setNormal();
    setCoef();
  }
  public double Evalue(double x, double y, GVector normal)
  {
	  Vertex vertex=new Vertex();
	  vertex.coord.x=x;
	  vertex.coord.y=y;
	  Trigon pt=qlist.GetTrigonH(vertex);
    if(pt==null)
    {
    return -200000000;
    }
    return pt.Evalue(x, y, normal);
  }
}