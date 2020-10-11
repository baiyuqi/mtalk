package com.tc.env;

import javax.naming.InitialContext;
import javax.naming.NamingException;

import com.tc.assistance.business.DeviceFacade;
import com.tc.assistance.business.DeviceFacadeLocal;
import com.tc.assistance.business.PersistenceFacade;
import com.tc.device.DeviceGatewayManager;
import com.tc.device.DeviceManager;
import com.tc.dwr.AlarmRegister;
import com.tc.sip.SipService;

public class Environment {
	AlarmRegister alarmRegister;
	SipService service;
	DeviceGatewayManager deviceGatewayManager;
	public DeviceGatewayManager getDeviceGatewayManager() {
		return deviceGatewayManager;
	}
	public SipService getSipService() {
		return service;
	}
	public AlarmRegister getAlarmRegister() {
		return alarmRegister;
	}
	public Environment(AlarmRegister r) {
		super();
		this.alarmRegister = r;
		try {
			init();
		} catch (NamingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	PersistenceFacade persistenceFacade;
	DeviceManager deviceManager;
	DeviceFacadeLocal deviceFacade;
	public DeviceFacadeLocal getDeviceFacade() {
		return deviceFacade;
	}
	public DeviceManager getDeviceManager() {
		return deviceManager;
	}
	void init() throws NamingException{
		InitialContext ctx = new InitialContext(); 
		persistenceFacade = (PersistenceFacade) ctx
		.lookup("AssistanceStudio/PersistenceFacadeBean/local");
		deviceFacade = (DeviceFacadeLocal) ctx
		.lookup("AssistanceStudio/DeviceFacade/local");
		deviceManager = new DeviceManager(persistenceFacade);
        service = new SipService();
    	try {
			service.init();
		} catch (Exception e) {

			e.printStackTrace();
		}
		this.deviceGatewayManager = new DeviceGatewayManager(service);
		
	}
	public PersistenceFacade getPersistenceFacade() {
		return persistenceFacade;
	}
	public void destroy() {
		service.destroy();
		
	}
}
