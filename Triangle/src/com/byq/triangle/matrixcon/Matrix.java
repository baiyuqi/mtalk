package com.byq.triangle.matrixcon;

import java.util.*;
import java.awt.*;

import com.byq.triangle.creutil.*;
import com.byq.triangle.geomath.*;
import com.byq.triangle.triangles.Analize;

class Matrix implements ContourCreator, Mesh, Envelope {
	int state;

	int col, row;

	double[][] data;

	byte[] flag;

	Vector con;

	double minx, maxx, miny, maxy, minz, maxz;

	double curh;

	int cur, prev, start;

	Vector CurCon;

	Vector conset;

	boolean rev;

	public Matrix() {
		state = 0;
	}

	public Matrix(double x0, double y0, double x1, double y1, int r, int c) {
		Initiate(x0, y0, x1, y1, r, c);
	}

	void reset() {
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++) {
				flag[(i * col + j) * 3] = flag[(i * col + j) * 3 + 1] = flag[(i
						* col + j) * 3 + 2] = 0;
				if (j % col == col - 1) {
					flag[(i * col + j) * 3 + 1] = -1;
					flag[(i * col + j) * 3 + 2] = -1;
				}
				if (i == row - 1) {
					flag[(i * col + j) * 3] = -1;
					flag[(i * col + j) * 3 + 1] = -1;
				}
			}
		rev = false;
		prev = -1;
	}

	public void setElement(int i, int j, double z) {
		data[i][j] = z;
	}

	double getStart(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		return data[r][c];
	}

	double getX(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		return minx + c * (maxx - minx) / col;
	}

	double getY(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		return miny + r * (maxy - miny) / row;
	}

	double getEnd(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		switch (index % 3) {
		case 0:
			r++;
			break;
		case 1:
			r++;
			c++;
			break;
		case 2:
			c++;
			break;
		}
		return data[r][c];
	}

	double getEY(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		switch (index % 3) {
		case 0:
			r++;
			break;
		case 1:
			r++;
			c++;
			break;
		case 2:
			c++;
			break;
		}
		return miny + r * (maxy - miny) / row;
	}

	double getEX(int index) {
		int c, r;
		int n = index / 3;
		r = n / col;
		c = n % col;
		switch (index % 3) {
		case 0:
			r++;
			break;
		case 1:
			r++;
			c++;
			break;
		case 2:
			c++;
			break;
		}
		return minx + c * (maxx - minx) / col;
	}

	int Pre(int index, int order) {

		switch (index % 3) {
		case 0:
			if (order == 0)
				return index - 1;
			else
				return index - 2;

		case 1:
			if (order == 0)
				return index - 1;
			else
				return index + 3 * col + 1;
		case 2:
			if (order == 0)
				return index - col * 3 - 1;
			else
				return index - col * 3 - 2;
		}
		return -2;
	}

	int Next(int index, int order) {
		switch (index % 3) {
		case 0:
			if (order == 0)
				return index + col * 3 + 2;
			else
				return index + 1;
		case 1:
			if (order == 0)
				return index + 2;
			else
				return index + 1;
		case 2:
			if (order == 0)
				return index - 1;
			else
				return index + 1;
		}
		return -2;
	}

	int Candidate(int order) {
		int c = (prev == Pre(cur, 0) || prev == Pre(cur, 1) || prev == -1) ? Next(
				cur, order)
				: Pre(cur, order);
		if (c < 0 || flag[c] == -1)
			return -1;
		return c;
	}

	boolean next() {
		int c1, c2;
		c1 = Candidate(0);
		c2 = Candidate(1);
		GVector nex1 = intersect(c1);
		GVector nex2 = intersect(c2);
		if (nex1 == null && nex2 == null) {
			if (rev == false) {
				int t = start;
				start = cur;
				cur = t;
				rev = true;
				prev = -2;
			} else
				rev = false;
			return false;
		}
		// cnode nex;
		// nex=intersect(c1);
		if (nex1 != null) {
			flag[c1] = 1;
			prev = cur;
			cur = c1;
			addnode(nex1);
		} else {
			// nex=intersect(c2);
			flag[c2] = 1;
			prev = cur;
			cur = c2;
			addnode(nex2);
		}
		if (cur == start)
			return false;
		return true;
	}

	void GetOneSeg() {
		while (next())
			;
		if (rev)
			while (next())
				;
	}

	void addnode(GVector node) {
		if (rev)
			CurCon.insertElementAt(node, 0);
		else
			CurCon.addElement(node);
	}

	GVector intersect(int index) {
		if (index < 0)
			return null;
		double s, e;
		s = getStart(index);
		e = getEnd(index);
		double z1, z2;
		z1 = s > e ? e : s;
		z2 = s > e ? s : e;
		if (z1 == z2 && curh == z1)
			return new GVector((getX(index) + getEX(index)) / 2.,
					(getY(index) + getEY(index)) / 2., curh);

		if (curh >= z1 && curh <= z2) {
			double x, y;
			x = getX(index) + (getEX(index) - getX(index)) * (curh - s)
					/ (e - s);
			y = getY(index) + (getEY(index) - getY(index)) * (curh - s)
					/ (e - s);
			return new GVector(x, y, curh);
		} else
			return null;
	}

	private Vector GetContourSet() {
		conset = new Vector();
		for (int i = 0; i < 3 * col * row; i++) {
			if (flag[i] == 0) {
				GVector sect = intersect(i);
				if (sect != null) {
					flag[i] = 1;
					CurCon = new Vector();
					cur = start = i;
					prev = -1;
					CurCon.add(sect);
					GetOneSeg();
					conset.add(CurCon);
					CurCon = null;
				}
			}
		}
		reset();
		return conset;
	}

	/*
	 * public void paint(Graphics g) {
	 * 
	 * for(int i=0;i<col;i++) for(int j=0;j<row;j++) { double x,y;
	 * x=data[i][j].x; y=data[i][j].y;
	 * g.drawOval((int)(x*100)+100,(int)(y*100)+100,2,2); }
	 * if(conset==null)return;
	 *  }
	 */
	boolean Initiate(double x0, double y0, double x1, double y1, int row,
			int col) {
		if (state != 0)
			return false;
		this.row = row;
		this.col = col;
		minx = x0;
		miny = y0;
		maxx = x1;
		maxy = y1;
		data = new double[row][col];
		flag = new byte[col * row * 3];
		reset();
		return true;
	}

	public Vector getContourSet(double z) {
		curh = z;
		GetContourSet();
		Vector result = conset;
		conset = null;
		return result;
	}

	public double getZ(int i, int j) {
		return data[i][j];
	}

	public ContourCreator getContourCreator() {
		return this;
	}

	public int getHeight() {
		return row;
	}

	public int getWidth() {
		return col;
	}

	public void createMesh(Analize alz, double x0, double y0, double x1,
			double y1, int r, int c) {
		GVector nor = new GVector();
		Initiate(x0, y0, x1, y1, r, c);
		minz = 200000;
		maxz = -200000;
		double z;
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++) {
				double x, y;
				x = getX(i, j);
				y = getY(i, j);
				z = data[i][j] = alz.Evalue(x, y, nor);
				// if( z == -200000000)continue;
				// minz = minz>z?z:minz;
				// maxz = maxz<z?z:maxz;
			}
		// blendMulti(4);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++) {
				z = data[i][j];
				if (z == -200000000)
					continue;
				minz = minz > z ? z : minz;
				maxz = maxz < z ? z : maxz;
			}
		state = 2;
	}

	private double getX(int i, int j) {
		return minx + j * (maxx - minx) / col;
	}

	private double getY(int i, int j) {
		return miny + i * (maxy - miny) / row;
	}

	public double getMinZ() {
		return minz;
	}

	public double getMaxZ() {
		return maxz;
	}

	public double getMinX() {
		return minx;
	}

	public double getMaxX() {
		return maxx;
	}

	public double getMinY() {
		return miny;
	}

	public double getMaxY() {
		return maxy;
	}

	public Envelope getEnvelope() {
		return this;
	}

	double getAt(int i, int j, int order) {
		switch (order) {
		case 0:
			j++;
			break;
		case 1:
			i--;
			break;
		case 2:
			j--;
			break;
		case 3:
			i++;
			break;

		}
		if (i >= row || j >= col || i < 0 || j < 0)
			return -2000000;
		return data[i][j];
	}

	void blend(int i, int j) {
		int n = 0;
		double sz = getZ(i, j);
		for (int k = 0; k < 4; k++) {
			double nz = getAt(i, j, k);
			if (nz != -2000000) {
				n++;
				sz += nz;
			}
		}
		data[i][j] = sz / n;

	}

	void blendOnce() {
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				blend(i, j);
	}

	void blendMulti(int n) {
		for (int i = 0; i < n; i++)
			blendOnce();
	}
}
