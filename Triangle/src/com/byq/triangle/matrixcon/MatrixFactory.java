package com.byq.triangle.matrixcon;
import java.util.*;

import com.byq.triangle.triangles.Analize;
public class MatrixFactory {
  public MatrixFactory() {
  }
static  public Mesh createMesh(Analize alz,double x0,double y0,double x1,double y1,int r,int c)
  {
    Matrix ma = new Matrix();
    ma.createMesh(alz, x0, y0, x1, y1, r, c);
    return ma;
  }
}