package com.tc.assistance.protocol.entity;

import java.util.Map;

import com.tc.assistance.entity.device.GeneralDevice;

public class DefinationReport  extends ProtocolMessageWrapper<Report> {

	
	public DefinationReport(Report report) {
		super(report);
		this.protocolMessage.action = "defination";
		this.protocolMessage.component = "device";
		
	}
	
	public GeneralDevice getDefination(){
		return (GeneralDevice) protocolMessage.data;
	}
	public void setDefination(GeneralDevice device){
		 protocolMessage.data = device;
	}
}
