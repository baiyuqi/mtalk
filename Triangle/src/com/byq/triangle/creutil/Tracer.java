package com.byq.triangle.creutil;

import java.awt.event.*;
import java.awt.*;

public class Tracer {
	Graphics g;

	Polygon pre = new Polygon();

	public Tracer(Graphics g) {
		this.g = g;

	}

	public void start(MouseEvent e) {
		pre.addPoint(e.getX(), e.getY());
		pre.addPoint(e.getX(), e.getY());
		pre.addPoint(e.getX(), e.getY());
		pre.addPoint(e.getX(), e.getY());
	}

	public void trace(MouseEvent e) {
		g.setXORMode(new Color(100, 100, 100));
		g.drawPolygon(pre);
		pre.xpoints[2] = pre.xpoints[3] = e.getX();
		pre.ypoints[1] = pre.ypoints[2] = e.getY();
		g.drawPolygon(pre);
	}

	public void draw() {
		g.setXORMode(new Color(100, 100, 100));
		g.drawPolygon(pre);
	}

	public Polygon getResult() {
		return pre;
	}

	public void stop(MouseEvent e) {
		trace(e);
	}
}