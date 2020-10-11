package com.byq.triangle.triangles;
import java.util.*;
public interface VoronoiGraph {

  public int GetNumOfTrigon();
  public int GetNumOfVertex();

  public Vertex GetVertex(int id);
  public Vector GetPolygon(int id);

  public Vertex GetIndexedVertex(int ind);
  public Vector GetIndexedPoly(int ind);

}