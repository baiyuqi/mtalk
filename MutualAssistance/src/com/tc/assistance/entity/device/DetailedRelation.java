package com.tc.assistance.entity.device;

import java.util.List;


public class DetailedRelation {
	public DetailedRelation(List<Relationship> i, List<Relationship> o) {
		in = i;
		out = o;
	}
	public List<Relationship> in;
	public List<Relationship> out;
	
	public Relationship getCared(String id){
		for(Relationship c : out){
			if(c.getToId().equals(id))
				return c;
			
		}
		return null;
	}
	public void computeCared(String me){
		for(Relationship c : out){
			c.cared = true;
		}
		for(Relationship c : in){
			c.cared = false;
		}
	}
}
