package com.tc.assistance.protocol.entity;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class PictureReport  extends ProtocolMessageWrapper<Report>{
	public PictureReport(Report report) {
		super(report);
		this.protocolMessage.action = "picture";
		this.protocolMessage.component = "phone";
	}

	public String getUrl() {
		return protocolMessage.asstributes.get("url");
	}

	public void setUrl(String url) {
		protocolMessage.asstributes.put("url", url);
	}


	
}
