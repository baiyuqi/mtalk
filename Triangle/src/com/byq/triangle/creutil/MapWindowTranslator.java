package com.byq.triangle.creutil;

public class MapWindowTranslator {
	float wsx, wsy;

	float minx, miny, maxx, maxy;

	int wwidth, wheight;

	float ratio;

	public MapWindowTranslator(Envelope map, int w, int h) {
		minx = (float) map.getMinX();
		maxx = (float) map.getMaxX();
		miny = (float) map.getMinY();
		maxy = (float) map.getMaxY();
		wsx = minx;
		wsy = maxy;
		wwidth = w;
		wheight = h;
		ratio = (maxx - minx) / wwidth;
	}

	public MapWindowTranslator(int w, int h) {
		minx = 0;
		maxx = w;
		miny = 0;
		maxy = h;
		wsx = 0;
		wsy = maxy;
		wwidth = w;
		wheight = h;
		ratio = 1;
	}

	public void setWindow(int width, int height) {
		wwidth = width;
		wheight = height;
	}

	public int getWindowX(float mapx) {
		int x = (int) ((mapx - wsx) / ratio);
		return x;
	}

	public int getWindowY(float mapy) {
		int y = (int) ((wsy - mapy) / ratio);
		return y;
	}

	public void move(int x, int y) {
		wsx -= x * ratio;
		wsy += y * ratio;
	}

	public float getMapX(int wx) {
		return wx * ratio + wsx;
	}

	public float getMapY(int wy) {
		return wsy - wy * ratio;
	}

	public void zoomWindow(int sx, int sy, int ex, int ey) {
		if (ex - sx == 0)
			return;
		wsx = getMapX(sx);
		wsy = getMapY(sy);
		ratio /= wwidth / (ex - sx);

	}

	public void zoomCenter(float s) {
		zoom(wwidth / 2, wheight / 2, s);
	}

	public void zoom(int x, int y, float s) {
		float cx, xy;
		move(wwidth / 2 - x, wheight / 2 - y);
		wsx = getMapX((int) (-x * s + x));
		wsy = getMapY((int) (-y * s + y));
		ratio *= s;
	}

	public float getDispMinX() {
		return wsx;
	}

	public float getDispMinY() {
		return getMapY(wheight);
	}

	public float getDispMaxX() {
		return getMapX(wwidth);
	}

	public float getDispMaxY() {
		return wsy;
	}

	public void setOrigin(float x, float y) {
		wsx = x;
		wsy = y;
	}

	public void setScale(float s) {
		ratio = s;
	}

	public void reset() {
		wsx = minx;
		wsy = maxy;
		ratio = (maxx - minx) / wwidth;
	}

}