package com.byq.triangle.gui;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.swing.JPanel;

import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.creutil.Tracer;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.render.AreasetRenderer;
import com.byq.triangle.render.ContourRenderer;
import com.byq.triangle.render.DelaunnyRenderType;
import com.byq.triangle.render.DelaunnyRenderer;
import com.byq.triangle.render.ImageRenderer;
import com.byq.triangle.render.RenderWindow;
import com.byq.triangle.render.Renderer;
import com.byq.triangle.render.TranslatorSource;
import com.byq.triangle.session.MapEnvelope;
import com.byq.triangle.session.NetSession;

public class RenderPanel extends JPanel implements MouseListener,MouseMotionListener, KeyListener,TranslatorSource, RenderWindow{
	NetSession session;
	int imode = 0;
	MapWindowTranslator tran;
	Tracer tracer;
	Renderer[] renderers;
	int renderMode = 0;
	public RenderPanel(NetSession s){
		this.setBackground(Color.black);
		this.session = s;
		renderers = new Renderer[8];
		renderers[0] = new DelaunnyRenderer(this, session, DelaunnyRenderType.TRIANGLE);
		renderers[1] = new DelaunnyRenderer(this, session, DelaunnyRenderType.POLYGON);
		renderers[2] = new DelaunnyRenderer(this, session, DelaunnyRenderType.POLYGON_TRIANGLE);
		renderers[3] = new ContourRenderer(this, session, false);
		renderers[4] = new ContourRenderer(this, session, true);
		renderers[5] = new ImageRenderer(this, session, false, this);
		renderers[6] = new ImageRenderer(this, session, true, this);
		renderers[7] = new AreasetRenderer(this, session);
		this.setFocusable(true);
		this.addMouseListener(this);
		this.addMouseMotionListener(this);
		this.addKeyListener(this);
		/**/
	}
	public void renderMode(int k){
		renderMode = k;
		update(getGraphics());
	}
	public int nextRenderMode(){
		renderMode = (renderMode + 1) % 8;
		update(getGraphics());
		return renderMode;
	}
	public int nextInputMode(){
		imode = (imode + 1) % 4;
		return imode;
	}
	public void mouseClicked(MouseEvent arg0) {
		
	}

	public void mouseDragged(MouseEvent e) {
		if (imode == 2)
			tracer.trace(e);
	}
	public void mousePressed(MouseEvent e) {

		int px = e.getX();
		int py = e.getY();
		double x = (double) tran.getMapX(px), y = (double) tran.getMapY(py);
		GVector v = new GVector();
		v.x = x;
		v.y = y;

		if (imode == 0) {
			session.insert(v);
		}
		if (imode == 1) {
			session.addBoundPoint(v);

		}
		if (imode == 2) {
			if (tracer != null)
				tracer.draw();
			tracer = new Tracer(this.getGraphics());
			tracer.start(e);
			return;
		}
		if (imode == 3) {
			session.choose(x, y);
		}



		Graphics g = this.getGraphics();
		this.update(g);
	}
	public void mouseReleased(MouseEvent e) {
		if (imode == 2){
			tracer.stop(e);
			java.awt.Polygon poly = tracer.getResult();
			Rectangle rec = poly.getBounds();
			MapEnvelope env = new MapEnvelope(
					(double) tran.getMapX((int) rec.getMinX()), 
					(double) tran.getMapY((int) rec.getMaxY()), 
					(double) tran.getMapX((int) rec.getMaxX()),
					(double) tran.getMapY((int) rec.getMinY())
					);
			session.setEnvelope(env);
		}
	}
	public void mouseEntered(MouseEvent arg0) {
		
	}
	public void mouseExited(MouseEvent arg0) {
		
	}
	public void keyPressed(KeyEvent e) {
		int code = e.getKeyCode();
		char k = e.getKeyChar();
		Graphics g = this.getGraphics();
		switch (k) {
		case 'x':
			for (int i = 0; i < 100; i++) {
				double x = Math.random() * 1500;
				double y = Math.random() * 1500;
				double z = Math.random() * 20;
				GVector v = new GVector(x, y, z);
				session.insert(v);
			}
			break;
		case 'r': {

			session.getNetFactory().getAnalize().recreateSurface();
			break;
		}
		case '-': {

			tran.zoomCenter(1.2f);
			break;
		}
		case 'd': {

	//		drawCon(g, smoset);
			return;
		}
		case 'c': {

	//		reset();
			break;
		}
		case '=': {
			tran.zoomCenter(.8f);
			break;
		}
		case 'a': {
	//		this.setCursor(WAIT_CURSOR);
	//		createNet();
	//		this.setCursor(DEFAULT_CURSOR);

			break;
		}
		case 'b': {
	//		this.setCursor(WAIT_CURSOR);
	//		createClip();
	//		this.setCursor(DEFAULT_CURSOR);

			break;
		}
	

		case 'z':
			break;
		case '1': {
			renderMode = 1;
			break;
		}
		case '2': {
			renderMode = 2;
			break;
		}
		case '3': {
			renderMode = 3;
			break;
		}
		case '4': {
			renderMode = 4;
			break;
		}
		case '5': {
			renderMode = 5;
			break;
		}
		case '6': {
			renderMode = 6;
			break;
		}
		case '7': {
			renderMode = 7;
			break;
		}
		}
		switch (code) {
		case 39: {
			tran.move(+30, 0);
			break;
		}
		case 40: {
			tran.move(0, 30);
			break;
		}
		case 37: {
			tran.move(-30, 0);
			break;
		}
		case 38: {
			tran.move(0, -30);
			break;
		}
		}
		this.update(g);
	}

	public void paint(Graphics g) {
		super.paint(g);
		Renderer r = renderers[renderMode];
		if(r != null)
		r.render(g);
	}

	public void keyTyped(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void keyReleased(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public MapWindowTranslator getTranslator() {
		// TODO Auto-generated method stub
		return tran;
	}
	public void reset(){
	tran = new MapWindowTranslator(getWidth(), getHeight());
	session.reset();
	}
	public void mouseMoved(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	public double getMaxZ() {
		// TODO Auto-generated method stub
		return session.getLmaxz();
	}
	public double getMinZ() {
		// TODO Auto-generated method stub
		return session.getLminz();
	}
}
