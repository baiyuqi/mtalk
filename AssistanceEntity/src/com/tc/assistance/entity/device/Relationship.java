package com.tc.assistance.entity.device;

import java.io.Serializable;
import javax.persistence.*;

/**
 * Entity implementation class for Entity: Relationship
 *
 */
@Entity

public class Relationship implements Serializable {


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
	String toId;
	String id;
	String name;
	String reverseName;

	@Id
	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}
	public Relationship() {
		super();
	}
   
}
