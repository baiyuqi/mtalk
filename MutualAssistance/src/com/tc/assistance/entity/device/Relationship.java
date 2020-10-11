package com.tc.assistance.entity.device;

import java.io.Serializable;



public class Relationship implements Serializable {
	public boolean cared;
	GeneralDevice device;
	public GeneralDevice getDevice() {
		return device;
	}
	public void setDevice(GeneralDevice device) {
		this.device = device;
	}
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getReverseName() {
		return reverseName;
	}

	public void setReverseName(String reverseName) {
		this.reverseName = reverseName;
	}

	public String getFromId() {
		return fromId;
	}

	public void setFromId(String fromId) {
		this.fromId = fromId;
	}

	public String getToId() {
		return toId;
	}

	public void setToId(String toId) {
		this.toId = toId;
	}
	private static final long serialVersionUID = 1L;
	String fromId;
	String toId;
	String id;
	String name;
	String reverseName;

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}
	public Relationship() {
		super();
	}
	@Override
	public String toString() {
		String n = null;
		if(cared)
			return (name == null || name.trim().equals("")?"Î´ÃüÃû":name) + "[" + toId + "]";

		 return (reverseName == null || reverseName.trim().equals("")?"Î´ÃüÃû":reverseName) + "[" + fromId + "]";
	}
	
   
}
