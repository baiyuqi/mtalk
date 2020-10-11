package com.byq.triangle.triangles;
import java.util.*;

import com.byq.triangle.geomath.*;

class VoronoiAlgorithm {
    GetComVexAlgorithm ci;
  public VoronoiAlgorithm(GetComVexAlgorithm cip) {
  ci=cip;
  }
  public Vector GetVorPoly(Vertex vertex)
  {
     if(vertex==null)return null;

     if(vertex.onbound)return this.GetBoundVorPoly(vertex);
     else return this.GetInsideVorPoly(vertex);
  }
private Vector GetSE(Vertex vertex)
{ Vector           cv ;
	ComVexNode start,end;
	cv=ci.GetCommonVertexTrigon(vertex);
  start=(ComVexNode)cv.elementAt(0);
  end=(ComVexNode)cv.elementAt(cv.size()-1);
	GVector vs,ve;
	byte svid,evid;
	svid=start.vid;
	evid=end.vid;
	vs=GeoMath.differance(start.trigon.vex[svid].coord,start.trigon.vex[(svid+1)%3].coord);
	ve=GeoMath.differance(end.trigon.vex[(evid+2)%3].coord,end.trigon.vex[evid].coord);
	vs.Rotate(3.1415926/2.);
	ve.Rotate(3.1415926/2.);
	ve.z=0;
	vs.z=0;
	vs.unitize();
	ve.unitize();
//	se[0]=vs;
//	se[1]=ve;
//	ClearComNodeList(cv);
  Vector se=new Vector();
  se.add(vs);se.add(ve);
  return se;
}
private Vector GetInsideVorPoly(Vertex vertex)
{   Vector ts=ci.GetCommonVertexTrigon(vertex);
    if(ts==null)return null;
    Vector poly=new Vector();
    int n=ts.size();
    for(int i=0;i<n;i++)
        poly.add(((ComVexNode)ts.elementAt(i)).trigon.heart);
    return poly;
}
private Vector GetBoundVorPoly(Vertex vertex)
{
    Vector poly=new Vector();
    Vector ts=ci.GetCommonVertexTrigon(vertex);
    if(ts==null)return null;
    int n=ts.size();
    for(int i=0;i<n;i++)
        poly.add(((ComVexNode)ts.elementAt(i)).trigon.heart);


	GVector StartHeart,EndHeart,Heart;
  Vector se;
  StartHeart=(GVector)poly.elementAt(0);
  EndHeart=(GVector)poly.elementAt(poly.size()-1);

	se=GetSE(vertex);
  GVector start,end;
  start=(GVector)se.elementAt(0);
  end= (GVector)se.elementAt(1);
  GVector vs,ve;
  start.scale(2000);
  end.scale(2000);
  vs=GeoMath.add(start,StartHeart);
  ve=GeoMath.add(end,EndHeart);
  poly.add(0,vs);
  poly.add(ve);
  return poly;
}
}