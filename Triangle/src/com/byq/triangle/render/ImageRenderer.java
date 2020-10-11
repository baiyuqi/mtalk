package com.byq.triangle.render;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;

import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.gui.DelaunnyFrame;
import com.byq.triangle.triangles.Analize;

public class ImageRenderer extends AbstractRenderer {
	boolean smooth;
	public ImageRenderer(TranslatorSource trans, NetSource ns, boolean smooth, RenderWindow rw) {
		super(trans, ns);
		this.smooth = smooth;
		this.renderWindow = rw;
	}

	public void render(Graphics g) {
		DrawImage(g);
	}

	public void DrawImage(Graphics g) {
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();

		Graphics2D g2 = (Graphics2D) g;
		int h, w;
		h = this.getRenderWindow().getHeight();
		w = this.getRenderWindow().getWidth();
		Analize ai = this.getNetSource().getNetFactory().getAnalize();
		for (int i = 0; i < w; i += 2)
			for (int j = 0; j < h; j += 2) {
				double x, y;
				x = tran.getMapX(i);
				y = tran.getMapY(j);
				GVector n = new GVector();
				double z;
				if(smooth)
				z = ai.Evalue(x, y, n);
				else z = ai.getHeight(x, y);
				float blue, red, green;
				if (z == -200000000)
					continue;// blue=red=green=0.0f;
				else {
					blue = (float) ((z - this.getRenderWindow().getMinZ()) / ((this.getRenderWindow().getMaxZ() - this.getRenderWindow().getMinZ()) * 2.5));
					red = (float) ((1.0 - blue) / 2.0);// (1.0f-blue)/4.0f;
					green = red;// red;
				}

				try {
					g.setColor(new Color(red, green, (float) (0.3 + 0.7 * blue)));
				} catch (RuntimeException e) {
					// TODO Auto-generated catch block
					//e.printStackTrace();
				}
				g.fillRect(i, j, 2, 2);
			}
	}

}
