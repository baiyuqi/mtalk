package com.tc.assistance.entity.device;

import java.util.List;


public class DeviceRelation {
	public DeviceRelation(List<String> i, List<String> o) {
		in = i;
		out = o;
	}
	public List<String> in;
	public List<String> out;
}
