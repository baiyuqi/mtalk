package com.byq.triangle.triangles;
import java.util.*;
class Queue {
  static Vector list = new Vector();
  static InsertAlgorithm tp;
  public Queue() {
  }

  public static void Delete(QueueNode node)
  {
  	Trigon trigon1=node.trigon;
  	byte vid1=node.vid;
  	if(trigon1!=null)
  		trigon1.etc[vid1]=null;

  	Trigon trigon2=trigon1.nbtri[vid1];
  	int vid2=trigon1.nbvex[vid1];
  	if(trigon2!=null)
  		trigon2.etc[vid2]=null;
    list.removeElement(node);
  }
  public static void Enter(QueueNode node)
  {
  	Trigon p1=node.trigon;
  	if(p1==null)
  		return;

  	byte vid1=node.vid;
  	Trigon p2=p1.nbtri[vid1];

  	if(p2==null)
  	{
  		p1.etc[vid1]=null;
  		return;
  	}
    list.addElement(node);
  }
  public static boolean Empty()
  {
    return list.size()==0;
  }
  public static QueueNode Out()
  {
  	if(Empty())
		return null;
    QueueNode node=(QueueNode)list.elementAt(0);
    list.removeElementAt(0);
  	return node;
  }
  public static void Process()
  {
   	QueueNode edge;
  	while(!Empty())
  	{
  		edge=Out();
  		if(edge.Exchangable())
  			tp.ChangeEdge(edge);
  		else
  			edge.RemoveEdge();;
  	}
  }
}