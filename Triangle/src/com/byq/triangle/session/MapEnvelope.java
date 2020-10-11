package com.byq.triangle.session;

import com.byq.triangle.creutil.Envelope;

public class MapEnvelope implements Envelope{
double minX, minY, maxX, maxY;
public MapEnvelope(double x0, double y0, double x1, double y1){
	this.minX = x0;
	this.minY = y0;
	this.maxX = x1;
	this.maxY = y1;
}
public double getMaxX() {
	return maxX;
}

public void setMaxX(double maxX) {
	this.maxX = maxX;
}

public double getMaxY() {
	return maxY;
}

public void setMaxY(double maxY) {
	this.maxY = maxY;
}

public double getMinX() {
	return minX;
}

public void setMinX(double minX) {
	this.minX = minX;
}

public double getMinY() {
	return minY;
}

public void setMinY(double minY) {
	this.minY = minY;
}
public double getMinZ() {
	// TODO Auto-generated method stub
	return 0;
}
public double getMaxZ() {
	// TODO Auto-generated method stub
	return 0;
}
}
