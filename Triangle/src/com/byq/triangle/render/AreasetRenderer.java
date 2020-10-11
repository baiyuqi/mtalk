package com.byq.triangle.render;

import java.awt.Graphics;

import com.byq.triangle.clip.Area;
import com.byq.triangle.clip.AreaSet;
import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.gui.DelaunnyFrame;

public class AreasetRenderer extends AbstractRenderer {

	public AreasetRenderer(TranslatorSource trans, NetSource ns) {
		super(trans, ns);
		// TODO Auto-generated constructor stub
	}

	public void render(Graphics g) {
		AreaSet aset = this.getNetSource().getAreaSet();
		Area h1 = this.getNetSource().getCurrentArea();
		MapWindowTranslator tran = this.getTranslatorSource().getTranslator();
		if (aset != null)
			aset.draw(g, tran);
		if (h1 != null)
			h1.drawW(g, tran);
	}

}
