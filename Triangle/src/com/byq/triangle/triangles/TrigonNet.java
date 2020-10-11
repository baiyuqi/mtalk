package com.byq.triangle.triangles;

import com.byq.triangle.creutil.*;
import com.byq.triangle.geomath.*;
import com.byq.triangle.matrixcon.*;

import java.util.*;

class TrigonNet implements ContourSource, Delaunny, VoronoiGraph, Interactive,
		Analize, Envelope, ContourCreator {
	byte show;

	ListParam listp;

	NetParam np;

	RawDataInfo DataInfo;

	boolean created;

	QueryList qlist;

	VertexSet vlist;

	Queue queue;

	Polygon startpolygon;

	BasicOperation bop;

	InsertAlgorithm IA;

	GetComVexAlgorithm CA;

	VoronoiAlgorithm VA;

	SurfaceAlgorithm SA;

	public TrigonNet() {
		np = new NetParam();
		created = false;
		show = 0;
		DataInfo = null;
		startpolygon = new Polygon(this);
		queue = new Queue();
		qlist = new QueryList(this);
		vlist = new VertexSet(this);
	}

	public boolean IsCreated() {
		return created;
	}

	/*
	 * public void drawNet(Graphics g,DrawParam p) { if(DataInfo==null)return;
	 * g.setColor(Color.pink); qlist.draw(g,p); } public void drawVor(Graphics
	 * g,DrawParam p) { if(DataInfo==null)return; g.setColor(Color.gray);
	 * qlist.drawv(g,p); } public void drawContour(Graphics g,DrawParam p) {
	 * if(DataInfo==null)return; }
	 * 
	 * public void draw(Graphics g,DrawParam p) { if(DataInfo==null)return;
	 * switch(show) { case 0: { g.setColor(Color.red); vlist.draw(g,p); break; }
	 * case 1: { drawNet(g,p); break; } case 2: { drawVor(g,p); break; } case 3: { //
	 * if(contourset!=null) // contourset.draw(g,p); break; } }
	 *  // convex.draw(g,p);
	 * 
	 *  }
	 */
	public void Init(RawDataInfo rdf) {
		DataInfo = rdf;
		listp = new ListParam(DataInfo);
		qlist.Init(listp);
		vlist.Init(listp);
		bop = new BasicOperation(vlist, qlist, np);
		IA = new InsertAlgorithm(vlist, qlist, np, bop);
		CA = new GetComVexAlgorithm(vlist, qlist, np, bop);
		SA = new SurfaceAlgorithm(vlist, qlist, np);
		VA = new VoronoiAlgorithm(CA);
		// processor=new TrigonProcessor(BasicOperation bp);
		Queue.tp = IA;
	}

	public void ReceiveVertex(Vertex vertex) {
		IA.ReceiveVertex(vertex);
	}

	public void PrepareVertex(Vertex vertex) {
		IA.PrepareVertex(vertex);
	}

	public void GetBoundary() {
		Convex convex = new Convex(this);
		convex.reset();
		convex.SetData();
		convex.GetConvex();
		convex.ExportData(startpolygon);
	}

	public void InitTriang() {
		startpolygon.triangulize();
		Queue.Process();
	}

	public void CreateNet() {
		vlist.CreateNet();
		created = true;
		SA.setSurface();
	}

	public void resetvisit() {
		qlist.reset();
	}

	public Trigon GetCrossTrigon(double z) {
		return qlist.GetCrossTrigon(z);
	}

	public void next() {
		show = (byte) ((show + 1) % 4);
	}

	public double getHeight(double x, double y) {
		return bop.GetHeight(x, y);
	}

	public Trigon GetFirstTrigon() {
		return qlist.GetFirstTrigon();
	}

	public Trigon GetNext() {
		return qlist.GetNext();
	}

	public Vector GetComVerTri(int id) {
		return CA.GetCommonVertexTrigon(vlist.GetVertex(id));
	}

	public int GetNumOfTrigon() {
		return np.not;
	}

	public int GetNumOfVertex() {
		return np.nov;
	}

	public Vector GetPolygon(int index) {
		Vertex v = vlist.GetVertex(index);
		return GetVorPoly(v);
	}

	public Vertex PickVertex(double x, double y) {
		return vlist.PickVertex(x, y);
	}

	public Vector GetVorPoly(Vertex vertex) {

		return VA.GetVorPoly(vertex);
	}

	public Vertex GetVertex(int id) {
		return vlist.GetVertex(id);
	}

	public Vertex GetIndexedVertex(int ind) {
		return vlist.GetIndexedVertex(ind);
	}

	public Vector GetIndexedPoly(int ind) {
		return VA.GetVorPoly(vlist.GetIndexedVertex(ind));
	}

	public void recreateSurface() {
		SA.setSurface();
	}

	public double Evalue(double x, double y, GVector normal) {

		return SA.Evalue(x, y, normal);
	}

	public double getMinZ() {
		return DataInfo.minz;
	}

	public double getMaxZ() {
		return DataInfo.maxz;
	}

	public double getMinY() {
		return DataInfo.miny;
	}

	public double getMaxY() {
		return DataInfo.maxy;
	}

	public double getMinX() {
		return DataInfo.minx;
	}

	public double getMaxX() {
		return DataInfo.maxx;
	}

	public Envelope getEnvelope() {
		return this;
	}

	public Vector getContourSet(double z) {
		ContourSet cs = new ContourSet(z, this);
		Vector result = new Vector();
		Vector conset = cs.list;
		int n = conset.size();
		for (int i = 0; i < n; i++) {
			Contour con = (Contour) conset.elementAt(i);
			result.addElement(con.list);
		}
		return result;
	}
}
