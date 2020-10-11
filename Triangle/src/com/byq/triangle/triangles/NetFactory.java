package com.byq.triangle.triangles;

import java.util.*;

import com.byq.triangle.creutil.*;
import com.byq.triangle.geomath.*;
import com.byq.triangle.matrixcon.*;

public class NetFactory {
	TrigonNet net;

	Vector data = new Vector();

	public NetFactory() {
		net = new TrigonNet();
	}

	public void initiate(RawDataInfo di) {
		net.Init(di);
	}

	public void prepareVertex(GVector coord, int id) {
		Vertex vertex = new Vertex(coord, id);
		data.addElement(vertex);
	}

	public void insertVertex(GVector coord, int id) {
		Vertex vertex = new Vertex(coord, id);
		net.ReceiveVertex(vertex);
	}

	public void createNet() {
		expand();
		net.Init(this.extractInfo());

		this.setVertice();
		net.GetBoundary();
		net.InitTriang();
		net.CreateNet();

	}

	private void expand() {
		RawDataInfo di = this.extractInfo();
		double x0 = di.minx - (di.maxx - di.minx);
		double y0 = di.miny - (di.maxy - di.miny);
		double x1 = di.maxx + (di.maxx - di.minx);
		double y1 = di.maxy + (di.maxy - di.miny);

		this.prepareVertex(new GVector(x0, y0, di.meanz), -1);
		this.prepareVertex(new GVector(x0, y1, di.meanz), -1);
		this.prepareVertex(new GVector(x1, y0, di.meanz), -1);
		this.prepareVertex(new GVector(x1, y1, di.meanz), -1);

	}

	public Vector getBuffer() {
		return data;
	}

	private void setVertice() {
		for (int i = 0; i < data.size(); i++)
			net.PrepareVertex((Vertex) data.elementAt(i));
		data.removeAllElements();
	}

	private RawDataInfo extractInfo() {
		int np = 0;
		double x1 = 1000000000, x2 = -1000000000, y1 = 1000000000, y2 = -1000000000, z1 = 1000000000, z2 = -1000000000;
		np = data.size();
		double meanz = 0;
		for (int i = 0; i < np; i++) {
			Vertex v = (Vertex) data.elementAt(i);
			if (x1 > v.getX())
				x1 = v.getX();
			if (y1 > v.getY())
				y1 = v.getY();
			if (z1 > v.getZ())
				z1 = v.getZ();
			if (x2 < v.getX())
				x2 = v.getX();
			if (y2 < v.getY())
				y2 = v.getY();
			if (z2 < v.getZ())
				z2 = v.getZ();
			meanz += v.getZ();
		}
		meanz /= np;
		RawDataInfo dif = new RawDataInfo();
		dif.pnumber = np;
		dif.minx = x1;
		dif.maxx = x2;
		dif.miny = y1;
		dif.maxy = y2;
		dif.minz = z1;
		dif.maxz = z2;
		dif.meanz = meanz;
		return dif;
	}

	public Delaunny getDelaunny() {
		return this.net;
	}

	public VoronoiGraph getVoronoiGraph() {
		return this.net;
	}

	public ContourSource getContourSource() {
		return this.net;
	}

	public boolean isCreated() {
		return net.IsCreated();
	}

	public Interactive getInteractive() {
		return this.net;
	}

	public Analize getAnalize() {
		return this.net;
	}

	public ContourCreator getContourCreator() {
		return net;
	}

	public Envelope getEnvelope() {
		return net;
	}
}