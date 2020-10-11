package com.byq.triangle.render;

import java.awt.Color;
import java.awt.Graphics;
import java.util.Vector;

import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.geomath.ConstVector;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.triangles.Delaunny;
import com.byq.triangle.triangles.Trigon;
import com.byq.triangle.triangles.Vertex;
import com.byq.triangle.triangles.VoronoiGraph;

public class DelaunnyRenderer extends AbstractRenderer{
	int renderType;
	public DelaunnyRenderer(TranslatorSource trans, NetSource ns, int type) {
		super(trans, ns);
		this.renderType = type;
	}

	public void render(Graphics g) {
		if(this.renderType == DelaunnyRenderType.POLYGON)
			this.drawVor(g);
		else if(this.renderType == DelaunnyRenderType.TRIANGLE)
			this.drawDelaunny(g);
		else {
			this.drawVor(g);
			this.drawDelaunny(g);
		}
	}

	private void DrawTrigon(Graphics g, Trigon tri) {
		if (tri == null)
			return;
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();

		for (int i = 0; i < 3; i++) {
			Vertex v1, v2;
			v1 = tri.get(i);
			v2 = tri.get((i + 1) % 3);
			g.drawLine(tran.getWindowX((float) v1.getX()), tran
					.getWindowY((float) v1.getY()), tran.getWindowX((float) v2
					.getX()), tran.getWindowY((float) v2.getY()));
		}
	
	}
	private void drawPoly(Graphics g, Vector<GVector> poly) {
		if (poly == null)
			return;
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();
		int n = poly.size();
		for (int k = 0; k < n; k++) {
			ConstVector v1 = (ConstVector) poly.get(k);
			ConstVector v2 = (ConstVector) poly.get((k + 1) % n);
			g.drawLine(tran.getWindowX((float) v1.getX()), tran
					.getWindowY((float) v1.getY()), tran.getWindowX((float) v2
					.getX()), tran.getWindowY((float) v2.getY()));
		}

	}
	
	private void drawVor(Graphics g) {
		
		VoronoiGraph vg = this.getNetSource().getNetFactory().getVoronoiGraph();
		int n = vg.GetNumOfVertex();
		Vector buf = this.getNetSource().getNetFactory().getBuffer();

		for (int i = 0; i < buf.size(); i++)
			drawVertex(g, (Vertex) buf.elementAt(i));
		drawPoly(g,this.getNetSource().getBoundPoly() );
		Color old = g.getColor();

		g.setColor(Color.MAGENTA);
		for (int i = 0; i < n; i++) {
			drawPoly(g, vg.GetIndexedPoly(i));
			drawVertex(g, vg.GetIndexedVertex(i));
		}
		g.setColor(old);
	}

	private void drawVertex(Graphics g, Vertex v) {
		if (v == null)
			return;
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();
		g.drawOval(tran.getWindowX((float) v.getX()) - 2, tran
				.getWindowY((float) v.getY()) - 2, 4, 4);
	}

	private void drawDelaunny(Graphics g) {
		Delaunny d = this.getNetSource().getNetFactory().getDelaunny();
		Trigon t = d.GetFirstTrigon();
		DrawTrigon(g, t);
		Vector buf = this.getNetSource().getNetFactory().getBuffer();
		for (int i = 0; i < buf.size(); i++)
			drawVertex(g, (Vertex) buf.elementAt(i));


		while ((t = d.GetNext()) != null)
			DrawTrigon(g, t);

	}

}
