package com.tc.assistance.protocol.entity;

import java.util.Map;

public class ProtocolMessage {
	
	static public final String COMMAND_PIC = "picture";
	static public final String COMMAND_LOCATION = "location";
	static public final String COMMAND_CALLBACK = "callback";
	static public final String COMMAND_PROFILE = "profile";

	
	public String from;
	public String to;
	public String source;
	public String component;
	public String action;
	public Map<String, String> asstributes;
	public Object data;
}
