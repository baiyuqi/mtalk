package com.tc.assistance.business;
import java.util.Map;

import javax.ejb.Local;

import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.GeneralDevice;


@Local
public interface DeviceFacadeLocal {
	Map<String, GeneralDevice> all();
	DeviceRelation relation(String deviceId);
	DetailedRelation detailedRelation(String deviceId);
}
