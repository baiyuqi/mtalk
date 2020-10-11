package com.byq.triangle.triangles;

public class PolygonNode {
  Vertex vertex;
	PolygonNode next;
	PolygonNode prev;
	Trigon trigon;
	byte vid;

  public PolygonNode() {
    vertex=null;
    trigon=null;
    next=prev=null;
    vid=-1;
  }
} 