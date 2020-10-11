package com.tc.clientmessage;

import com.tc.assistance.entity.device.GeneralDevice;

public class DeviceStatusUpdate extends ClientMassage {
	GeneralDevice device;

	public GeneralDevice getDevice() {
		return device;
	}

	public void setDevice(GeneralDevice device) {
		this.device = device;
	}
}
