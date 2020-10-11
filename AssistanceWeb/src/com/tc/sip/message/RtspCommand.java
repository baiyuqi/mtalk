package com.tc.sip.message;

public class RtspCommand extends SipMessage {

	public String command;
	public String ip;
	public int port;
	public String content(){
		return "command:" + command + "\r\n" 
		+ "ip:"+ ip + "\r\n" 
		+ "port:" + port;
	}
}
