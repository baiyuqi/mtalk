package com.tc.assistance.protocol.entity;

import java.util.Map;

public class DeviceReport extends ProtocolMessageWrapper<Report>{
	public DeviceReport(Report report) {
		super(report);
	}

	Map<String, Double> parameters;

	public Map<String, Double> getParameters() {
		return (Map<String, Double>) protocolMessage.data;
	}

}
