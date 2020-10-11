package com.byq.triangle.triangles;
import com.byq.triangle.geomath.*;
public interface Analize {
  public double getHeight(double x,double y);
  public double Evalue(double x,double y,GVector normal);
  public void recreateSurface();
}