package com.tc.assistance.protocol.entity;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Command extends ProtocolMessageWrapper<Request>{
	public Command(Request message) {
		super(message);
		
	}
	
	static public final String COMMAND_PIC = "picture";
	static public final String COMMAND_LOCATION = "location";
	static public final String COMMAND_CALLBACK = "callback";
	static public final String COMMAND_PROFILE = "profile";
	
	public String getCommand() {
		return this.protocolMessage.action;
	}
	public void setCommand(String command) {
		 this.protocolMessage.action = command;
	}
	
	
}
