package com.byq.triangle.session;

import java.awt.Rectangle;
import java.util.Vector;

import com.byq.triangle.clip.Area;
import com.byq.triangle.clip.AreaSet;
import com.byq.triangle.creutil.Envelope;
import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.creutil.Tracer;
import com.byq.triangle.geomath.GVector;
import com.byq.triangle.geomath.GeoMath;
import com.byq.triangle.matrixcon.ContourCreator;
import com.byq.triangle.matrixcon.MatrixFactory;
import com.byq.triangle.matrixcon.Mesh;
import com.byq.triangle.render.NetSource;
import com.byq.triangle.triangles.Analize;
import com.byq.triangle.triangles.DrawParam;
import com.byq.triangle.triangles.NetFactory;

public class NetSession implements NetSource {
	NetFactory netFactory;

	int id = 0;

	Vector<Vector<GVector>> contours;

	Vector<Vector<GVector>> smoothContours;

	AreaSet areaSet;

	double lmaxz = 0, lminz = 0;

	double delta;

	Vector<GVector> boundPoly = new Vector<GVector>();

	Area currentArea;

	Mesh mesh;

	MapEnvelope envelope;

	public NetSession() {

	}

	public void createNet() {
		netFactory.createNet();
		ContourCreator cc = netFactory.getContourCreator();
		Envelope env = cc.getEnvelope();
		contours = new Vector<Vector<GVector>>();
		for (int i = 0; i < 20; i++) {
			double z = env.getMinZ() + i * (env.getMaxZ() - env.getMinZ()) / 20;
			contours.addElement(cc.getContourSet(z));
		}
		createClip();
	}

	void createClip() {
		Analize al = netFactory.getAnalize();
		int n = boundPoly.size();
		for (int i = 0; i < n; i++) {
			GVector v = (GVector) boundPoly.elementAt(i);
			double z = al.Evalue(v.x, v.z, new GVector());
			v.z = z;
		}

		createSmoothContour();
	}

	public Mesh createMesh() {
		Envelope env = envelope;
		if (env == null)
			env = netFactory.getEnvelope();
		mesh = MatrixFactory.createMesh(netFactory.getAnalize(), env.getMinX(),
				env.getMinY(), env.getMaxX(), env.getMaxY(), 120, 120);
		return mesh;
	}

	void createSmoothContour() {
		createMesh();
		smoothContours = new Vector();
		ContourCreator cc;
		cc = mesh.getContourCreator();
		Envelope env = cc.getEnvelope();
		delta = (env.getMaxZ() - env.getMinZ()) / 10;
		for (int i = 0; i < 10; i++) {
			double z = env.getMinZ() + i * delta;
			smoothContours.addElement(cc.getContourSet(z));
		}
		clip();
		areaSet.setZ();
	}

	void clip() {
		areaSet = new AreaSet(delta);
		Area ar = new Area(boundPoly, null, areaSet, 1);
		areaSet.add(ar);
		int n = smoothContours.size();
		for (int i = 0; i < n; i++) {
			Vector lset = (Vector) smoothContours.elementAt(i);
			int m = lset.size();
			for (int j = 0; j < m; j++) {
				Vector line = (Vector) lset.elementAt(j);
				boolean closed = GeoMath.trim(line);
				areaSet.LineCut(line, closed);
			}
		}
	}

	public void setEnvelope(MapEnvelope envelope) {
		this.envelope = envelope;
	}

	public void reset() {
		netFactory = new NetFactory();
		contours = null;
		smoothContours = null;
		areaSet = null;
		boundPoly = new Vector();
		currentArea = null;

		netFactory = new NetFactory();

	}

	public Vector<GVector> getBoundPoly() {
		return boundPoly;
	}

	public AreaSet getAreaSet() {
		return areaSet;
	}

	public Area getCurrentArea() {
		return currentArea;
	}

	public Mesh getMesh() {
		return mesh;
	}

	public NetFactory getNetFactory() {
		return netFactory;
	}

	public Vector<Vector<GVector>> getSmoothContours() {
		// TODO Auto-generated method stub
		return smoothContours;
	}

	public Vector<Vector<GVector>> getContours() {
		// TODO Auto-generated method stub
		return contours;
	}

	public void choose(double x, double y) {
		if (areaSet == null)
			return;
		currentArea = areaSet.getArea(x, y);
	}

	public void addBoundPoint(GVector v) {
		boundPoly.addElement(v);
	}

	public void insert(GVector v) {
		v.z = (Math.random()) % 20;// ++;
		lmaxz = lmaxz < v.z ? v.z : lmaxz;// ++;
		id++;

		if (!netFactory.isCreated())
			netFactory.prepareVertex(v, id);
		else
			netFactory.insertVertex(v, id);
	}

	public double getLmaxz() {
		return lmaxz;
	}

	public double getLminz() {
		return this.lminz;
	}
}
