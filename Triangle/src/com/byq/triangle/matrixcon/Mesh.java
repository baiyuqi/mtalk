package com.byq.triangle.matrixcon;
import com.byq.triangle.creutil.*;
public interface Mesh {
  public ContourCreator getContourCreator();
  public int getHeight();
  public int getWidth();
  public double getZ(int i, int j);
  public Envelope getEnvelope();
}
