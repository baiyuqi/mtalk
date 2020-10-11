package com.tc.assistance.protocol.entity;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Alarm extends ProtocolMessageWrapper<Report> {
	public Alarm(Report report) {
		super(report);
		report.action = "alarm";
		
	}

	static public final String PRESSURE_HIGH = "pressure_high";
	static public final String TEMPERATURE_HIGH = "temperature_high";

	
	public String getType() {
		return protocolMessage.asstributes.get("type");
	}
	public void setType(String type) {
		protocolMessage.asstributes.put("type",type);
	}
	
	
}
