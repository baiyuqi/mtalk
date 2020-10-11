package com.tc.assistance.protocol.entity;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class LocationReport extends ProtocolMessageWrapper<Report> {
	
	public LocationReport(Report report) {
		super(report);
		this.protocolMessage.action = "location";
		this.protocolMessage.component = "phone";
	}

	public double getLongitude() {
		return Double.parseDouble(protocolMessage.asstributes.get("longitude"));
	}
	public void setLongitude(double longitude) {
		protocolMessage.asstributes.put("longitude", "" + longitude);
	}
	public double getLatitude() {
		return Double.parseDouble(protocolMessage.asstributes.get("latitude"));
	}
	public void setLatitude(double latitude) {
		protocolMessage.asstributes.put("latitude", "" + latitude);
	}
	
}
