package com.tc.assistance.entity.device;

import java.util.List;


public class DetailedRelation {
	public DetailedRelation(List<Relationship> i, List<Relationship> o) {
		in = i;
		out = o;
	}
	public List<Relationship> in;
	public List<Relationship> out;
}
