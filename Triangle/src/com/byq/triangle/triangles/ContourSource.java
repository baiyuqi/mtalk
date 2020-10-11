package com.byq.triangle.triangles;

import com.byq.triangle.creutil.*;
public interface ContourSource {
  public void resetvisit();
  public Trigon GetCrossTrigon(double z);
  public Envelope getEnvelope();
} 