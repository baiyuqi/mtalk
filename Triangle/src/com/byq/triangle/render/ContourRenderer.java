package com.byq.triangle.render;

import java.awt.Color;
import java.awt.Graphics;
import java.util.Vector;

import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.gui.DelaunnyFrame;

public class ContourRenderer extends AbstractRenderer{
	boolean smooth;
	public ContourRenderer(TranslatorSource trans, NetSource ns, boolean smooth) {
		super(trans, ns);
		this.smooth = smooth;
		// TODO Auto-generated constructor stub
	}

	public void render(Graphics g) {
		drawCon(g);

	}
	private void drawCon(Graphics g) {

		
		Vector<Vector<GVector>> smoset;
		if(!smooth)
			smoset = this.getNetSource().getContours();
		else smoset = this.getNetSource().getSmoothContours();
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
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();
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

	private void drawCon(Graphics g, Vector v) {
		if (v == null)
			return;
		int n = v.size();
		for (int i = 0; i < n; i++) {
			g.setColor(new Color(1.0f - i * 1.0f / n, 1.0f - i * 1.0f / n, i
					* 1.0f / n));
			drawcset(g, (Vector) v.get(i));
		}
	}
}
