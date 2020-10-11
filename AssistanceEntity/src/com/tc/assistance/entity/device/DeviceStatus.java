package com.tc.assistance.entity.device;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DeviceStatus {
	String deviceId;
	List<StatusItem> values = new ArrayList<StatusItem>();
	public String getDeviceId() {
		return deviceId;
	}
	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}
	public List<StatusItem> getValues() {
		return values;
	}
	public void setValues(List<StatusItem> values) {
		this.values = values;
	}
}
