package com.byq.triangle.triangles;
 import java.util.*;
class GetComVexAlgorithm {
  QueryList qlist;
  VertexSet vlist;
  NetParam np;
  BasicOperation bop;
  public GetComVexAlgorithm(VertexSet vl,QueryList ql,NetParam npa,BasicOperation bo) {
  qlist=ql;
  vlist=vl;
  np=npa;
  bop=bo;
  }
public Vector GetCommonVertexTrigon(Vertex vertex)
{
	Trigon t=qlist.GetOneTrigon(vertex);
  if(t==null)
		return null;
	byte vid=t.GetVertex(vertex);
	Vector header=new Vector();
	header.addElement(GoToLeft(vertex,t,vid,t,vid));

	SearchRight(header);
	return header;

}

public void SearchLeft(Vector header)
{
 ComVexNode cv=(ComVexNode)header.elementAt(header.size()-1);
	ComVexNode next;

	Trigon t=cv.trigon;
	byte vid=(byte)((cv.vid+2)%3);

	Trigon left=t.nbtri[vid];
	byte vid0=(byte)(t.nbvex[vid]);
	byte v=(byte)((vid0+2));
  ComVexNode h=(ComVexNode)header.elementAt(0);
	if(left!=null&&left!=h.trigon)
	{
		next=new ComVexNode();
		next.trigon=left;
		next.vid=v;
		header.addElement(next);
	}
	else
		return;
	SearchLeft(header);
}

public void SearchRight(Vector header)
{ ComVexNode cv=(ComVexNode)header.elementAt(header.size()-1);
	ComVexNode next;

	Trigon t=cv.trigon;
	byte vid=(byte)((cv.vid+1)%3);

	Trigon right=t.nbtri[vid];
 	byte vid0=t.nbvex[vid];
	byte v=(byte)((vid0+1)%3);

  ComVexNode h=(ComVexNode)header.elementAt(0);
	if(right!=null&&right!=h.trigon)
	{
		next=new ComVexNode();
		next.trigon=right;
		next.vid=v;
		header.addElement(next);
	}
	else
		return;

	SearchRight(header);

}

public ComVexNode GoToLeft(Vertex vertex,Trigon start,byte vids,Trigon trigon,byte vid)
{
  ComVexNode cvn=null;
	Trigon pt=trigon.nbtri[(vid+2)%3];
	byte vid1=trigon.nbvex[(vid+2)%3];
	byte vn=(byte)((vid1+2)%3);
	
	if(pt==null)
	{
		cvn=new ComVexNode();
		cvn.trigon=trigon;
		cvn.vid=vid;
		return cvn;
	}
	if(pt==start)
	{
		cvn=new ComVexNode();
		cvn.trigon=start;
		cvn.vid=vids;
		return cvn;
	}
	return GoToLeft(vertex,start, vids,pt,vn);


}
public ComVexNode GoToRight(Vertex vertex,Trigon start,byte vids,Trigon trigon,byte vid)
{
  ComVexNode cvn=null;
	Trigon pt=trigon.nbtri[(vid+1)%3];
		byte vid1=trigon.nbvex[(vid+1)%3];
			byte vn=(byte)((vid1+1)%3);
	
	if(pt==null)
	{
		cvn=new ComVexNode();
		cvn.trigon=trigon;
		cvn.vid=vid;
		return cvn;
	}
	if(pt==start)
	{
		cvn=new ComVexNode();
		cvn.trigon=start;
		cvn.vid=vids;
		return cvn;
	}
	return GoToRight(vertex,start,vids,pt,vn);

}


} 