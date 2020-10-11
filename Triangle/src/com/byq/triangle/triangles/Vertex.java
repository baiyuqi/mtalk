package com.byq.triangle.triangles;
import java.util.Vector;

import com.byq.triangle.geomath.*;
public class Vertex
{
  byte inserted;
  public int id;
  public GVector coord;
  public GVector normal;
  boolean onbound;
  public Vertex(GVector coord,int id)
  {
  this.coord = coord;
  this.id= id;
       inserted=0;
       onbound=false;
  }
  public Vertex()
  {   coord=new GVector();
       inserted=0;
       onbound=false;
       id=-1;
  }
    public Vertex(GVector co)
  {   coord=new GVector();
       inserted=0;
       onbound=false;
       id=-1;
       coord = co;
  }
  public double getX()
  {
    return coord.x;
  }
  public double getY()
  {
    return coord.y;
  }
  public double getZ()
  {
    return coord.z;
  }
  public int getID()
  {
  return id;
  }
  public void setNormal(GetComVexAlgorithm ca)
  {
      normal = new GVector(0,0,0);
      Vector v = ca.GetCommonVertexTrigon(this);
      if(v == null)return;
      ComVexNode cn;
      for(int i= 0; i < v.size(); i++)
      {
        cn = (ComVexNode)v.elementAt(i);
        Trigon t = cn.trigon;
        normal.add(t.normal);
      }
      normal.unitize();
  }

}
