package com.tc.action;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Category {
	private static Map<String, Category> catMap = new HashMap<String, Category>();
	

	public static Category getById(String id) {
		return catMap.get(id);
	}

	private String id;
	private String name;
	private List<Category> children;
	private boolean toggle;

	public Category(String id, String name, Category... children) {
		this.id = id;
		this.name = name;
		this.children = new ArrayList<Category>();
		for (Category child : children) {
			this.children.add(child);
		}
		catMap.put(id, this);
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public List<Category> getChildren() {
		return children;
	}

	public void setChildren(List<Category> children) {
		this.children = children;
	}

	public void toggle() {
		toggle = !toggle;
	}

	public boolean isToggle() {
		return toggle;
	}
}
