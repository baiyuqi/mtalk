package com.byq.triangle.render;

import java.util.Vector;

import com.byq.triangle.clip.Area;
import com.byq.triangle.clip.AreaSet;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.triangles.NetFactory;

public interface NetSource {
	NetFactory getNetFactory();
	AreaSet getAreaSet();
	Vector<Vector<GVector>> getSmoothContours();
	Vector<Vector<GVector>> getContours();
	Vector<GVector> getBoundPoly();
	Area getCurrentArea();
}
