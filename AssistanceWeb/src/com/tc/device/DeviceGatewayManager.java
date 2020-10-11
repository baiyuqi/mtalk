package com.tc.device;

import java.util.HashMap;
import java.util.Map;

import com.tc.sip.SipService;

public class DeviceGatewayManager {
	Map<String, DeviceGateway> gateways = new HashMap<String, DeviceGateway>();
	public DeviceGatewayManager(SipService service){
		gateways.put("sms", new SmsDeviceGateway());
		gateways.put("sip", new SipDeviceGateway(service));
	}
	public DeviceGateway get(String channel){
		return gateways.get(channel);
		
	}
	public void addListener(MessageListener l) {
		for(DeviceGateway dg : gateways.values()){
			dg.addListener(l);
		}
	}

}
