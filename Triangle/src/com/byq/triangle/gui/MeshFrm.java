//Title:        Your Product Name
//Version:
//Copyright:    Copyright (c) 1999
//Author:       byq
//Company:      creation
//Description:  Your description

package com.byq.triangle.gui;

import java.awt.*;
import javax.swing.JFrame;

import com.byq.triangle.clip.*;
import com.byq.triangle.creutil.*;
import com.byq.triangle.geomath.*;
import com.byq.triangle.matrixcon.*;
import com.byq.triangle.triangles.*;

import java.util.*;

public class MeshFrm extends JFrame{
	MapWindowTranslator tran;

	Mesh mesh;

	Vector smoset;

	Vector bpoly;

	AreaSet aset;

	public MeshFrm(Mesh ms, Vector bp) {
		mesh = ms;
		bpoly = bp;
		createContour();
	}

	void createContour() {
		smoset = new Vector();
		ContourCreator cc;
		cc = mesh.getContourCreator();
		Envelope env = cc.getEnvelope();
		for (int i = 0; i < 20; i++) {
			double z = i;// env.getMinZ()+i*(env.getMaxZ()-env.getMinZ())/20;
			smoset.addElement(cc.getContourSet(z));
		}
		clip();
	}

	void clip() {
		aset = new AreaSet(0);
		Area ar = new Area(bpoly, null, aset, 1);
		aset.add(ar);
		int n = smoset.size();
		for (int i = 0; i < n; i++) {
			Vector lset = (Vector) smoset.elementAt(i);
			int m = lset.size();
			for (int j = 0; j < m; j++) {
				Vector line = (Vector) lset.elementAt(j);
				boolean closed = GeoMath.trim(line);
				aset.LineCut(line, closed);
			}
		}
	}

	public void paint(Graphics g) {
		super.paint(g);
		drawCon(g);
		// aset.draw(g,tran);
	}

	private void drawCon(Graphics g) {
		if (smoset == null)
			return;
		int n = smoset.size();
		for (int i = 0; i < n; i++) {
			g.setColor(new Color(1.0f - i * 1.0f / n, 1.0f - i * 1.0f / n, i
					* 1.0f / n));
			drawcset(g, (Vector) smoset.get(i));
		}
	}

	void drawcset(Graphics g, Vector cset) {

		int ss = cset.size();
		for (int i = 0; i < ss; i++) {

			Vector cont = (Vector) cset.get(i);
			int cs = cont.size();
			for (int j = 0; j < cs - 1; j++) {
				GVector n1, n2;
				n1 = (GVector) cont.get(j);
				n2 = (GVector) cont.get((j + 1));
				g.drawLine((int) tran.getWindowX((float) n1.x), (int) tran
						.getWindowY((float) n1.y), (int) tran
						.getWindowX((float) n2.x), (int) tran
						.getWindowY((float) n2.y));
			}
		}
		g.setColor(new Color(0, 0, 0));
	}

	public void reset() {
		tran = new MapWindowTranslator(mesh.getEnvelope(), getWidth(),
				getHeight());
		this.update(getGraphics());
	}

	public void setMesh(Mesh ms) {
		mesh = ms;
		createContour();
	}
}