package com.byq.triangle.triangles;

import com.byq.triangle.geomath.*;

class Polygon {
	TrigonNet pNet;

	int vnumber;

	PolygonNode header;

	public Polygon() {
		vnumber = 0;
		header = null;
		pNet = null;
	}

	public Polygon(TrigonNet net) {
		vnumber = 0;
		header = null;
		pNet = net;
	}

	public void insertNode(Vertex vertex) {
		PolygonNode node = new PolygonNode();
		node.vertex = vertex;
		if (header == null) {
			node.next = node.prev = node;
			header = node;
		} else {
			node.next = header;
			node.prev = header.prev;
			header.prev.next = node;
			header.prev = node;
		}
		node.vertex.onbound = true;
		vnumber++;
	}

	public void triangulize() {
		if (vnumber == 0)
			return;
		if (vnumber == 2)
			return;
		if (cutable()) {
			appendTrigon();
			triangulize();
		} else {
			header = header.next;
			triangulize();
		}
	}

	private boolean cutable() {
		if (vnumber == 3)
			return true;
		Vertex v1 = header.vertex;
		Vertex v2 = header.next.next.vertex;

		Vertex temp = new Vertex();
		temp.coord.x = (v1.coord.x + v2.coord.x) / 2.0;
		temp.coord.y = (v1.coord.y + v2.coord.y) / 2.0;
		if (!insidePolygon(temp))
			return false;
		if (vnumber == 4)
			return true;
		PolygonNode p1, p2;
		p1 = header.next.next.next;
		p2 = p1.next;
		while (p2 != header) {
			if (GeoMath.InterSect(v1.coord, v2.coord, p1.vertex.coord,
					p2.vertex.coord))
				return false;
			p1 = p2;
			p2 = p2.next;
		}
		return true;

	}

	boolean insidePolygon(Vertex vertex) {
		int c = 0;
		Vertex faraway = new Vertex();
		faraway.coord.x = 10000000;
		PolygonNode p1, p2;
		p1 = header;
		p2 = p1.next;
		while (p1.next != header) {
			boolean b1, b2, b3, b4, b5, b6, b7, b8, b;
			b1 = GeoMath.InterSect1(vertex.coord, faraway.coord,
					p1.vertex.coord, p2.vertex.coord);
			b2 = GeoMath.OnLine(p2.vertex.coord, vertex.coord, faraway.coord);
			b3 = GeoMath.OnLine(p2.next.vertex.coord, vertex.coord,
					faraway.coord);
			b4 = GeoMath.SameSide1(p1.vertex.coord, p2.next.vertex.coord,
					vertex.coord, faraway.coord);
			b5 = GeoMath.OnLine(p2.next.vertex.coord, vertex.coord,
					faraway.coord);
			b6 = GeoMath.SameSide1(p1.vertex.coord, p2.next.next.vertex.coord,
					vertex.coord, faraway.coord);
			b7 = !b3 && !b4;
			b8 = b5 && !b6;
			b = b1 || (b2 && (b7 || b8));
			if (b)
				c++;
			p1 = p2;
			p2 = p2.next;
		}

		{
			boolean b1, b2, b3, b4, b5, b6, b7, b8, b;
			b1 = GeoMath.InterSect1(vertex.coord, faraway.coord,
					p1.vertex.coord, p2.vertex.coord);
			b2 = GeoMath.OnLine(p2.vertex.coord, vertex.coord, faraway.coord);
			b3 = GeoMath.OnLine(p2.next.vertex.coord, vertex.coord,
					faraway.coord);
			b4 = GeoMath.SameSide1(p1.vertex.coord, p2.next.vertex.coord,
					vertex.coord, faraway.coord);
			b5 = GeoMath.OnLine(p2.next.vertex.coord, vertex.coord,
					faraway.coord);
			b6 = GeoMath.SameSide1(p1.vertex.coord, p2.next.next.vertex.coord,
					vertex.coord, faraway.coord);
			b7 = !b3 && !b4;
			b8 = b5 && !b6;
			b = b1 || (b2 && (b7 || b8));
			if (b)
				c++;
		}

		return (c % 2) != 0;
	}

	public void appendTrigon() {
		if (vnumber == 3) {
			Trigon pt = new Trigon();
			if (pt == null)
				return;
			pt.vex[0] = header.vertex;
			pt.vex[1] = header.next.vertex;
			pt.vex[2] = header.next.next.vertex;

			pt.nbtri[0] = header.next.trigon;
			pt.nbtri[1] = header.prev.trigon;
			pt.nbtri[2] = header.trigon;

			pt.nbvex[0] = header.next.vid;
			pt.nbvex[1] = header.prev.vid;
			pt.nbvex[2] = header.vid;

			pNet.bop.InsertTrigon(pt);
			pNet.bop.CheckTrigonEdge(pt);
			header = null;
			vnumber = 0;
			return;
		}

		Trigon pt = new Trigon();
		if (pt == null)
			return;
		pt.vex[0] = header.vertex;
		pt.vex[1] = header.next.vertex;
		pt.vex[2] = header.next.next.vertex;
		pt.nbtri[0] = header.next.trigon;
		pt.nbtri[1] = null;
		pt.nbtri[2] = header.trigon;
		pt.nbvex[0] = header.next.vid;
		pt.nbvex[1] = -1;
		pt.nbvex[2] = header.vid;

		pNet.bop.InsertTrigon(pt);
		pNet.bop.CheckTrigonEdge(pt);
		header.next = header.next.next;
		header.next.prev = header;
		header.trigon = pt;
		header.vid = 1;
		vnumber--;
	}

}