package com.byq.triangle.matrixcon;
import java.util.*;

import com.byq.triangle.creutil.*;

public interface ContourCreator {
  public Vector getContourSet(double z);
  public Envelope getEnvelope();
}
