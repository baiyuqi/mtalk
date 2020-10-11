package com.tc.mobile;

import java.util.ArrayList;
import java.util.EventObject;
import java.util.List;

import com.thoughtworks.xstream.XStream;

public class Account {
	String name;
	String phone;
	List<EventObject> events;
	@Override
	public String toString() {
		return name;
	}

	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getPhone() {
		return phone;
	}
	public void setPhone(String phone) {
		this.phone = phone;
	}
	static public void main(String[] as){
		List l = new ArrayList();
		Account a = new Account();
		a.name = "me";
		l.add(a);
		XStream s = new XStream();
		System.out.println(s.toXML(l));
	}
	
}
