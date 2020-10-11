package com.byq.triangle.triangles;
import java.util.*;
public interface Delaunny {
  public Trigon GetFirstTrigon();
  public Trigon GetNext();
  public Vector GetComVerTri(int id);
  public int GetNumOfTrigon();
  public int GetNumOfVertex();

}