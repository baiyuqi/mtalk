package com.tc.action;

import java.util.Map;
import java.util.Set;

import org.apache.struts2.interceptor.SessionAware;

import com.tc.assistance.entity.business.DeviceUser;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.protocol.entity.Request;
import com.tc.device.DeviceGateway;
import com.tc.device.DeviceGatewayManager;

public class DeviceControlAction extends  EnvironmentSupport  implements  SessionAware{         
	DeviceUser user;
	String requestId;
	Set<String> channels;
	public String getRequestId() {
		return requestId;
	}
	public void setRequestId(String requestId) {
		this.requestId = requestId;
	}
	public String getId() {
		return id;
	}
	public void setId(String id) {
		this.id = id;
	}
	
	String id;
	public String getLocalId() {
		return localId;
	}
	public void setLocalId(String localId) {
		this.localId = localId;
	}

	String localId;
	String deviceType;
	DeviceComponent component;
	private String action;

	@Override
	public void setSession(Map<String, Object> s) {
		user = (DeviceUser)s.get("user");
		
	}
	public String control(){
		if(requestId.indexOf("_")==-1){
			id = requestId;
			deviceType = (String)this.env.getPersistenceFacade().unique("select type from GeneralDevice where id = '" + id + "'");
			
			return "device";
		}
		String[] ids = requestId.split("_");
		id = ids[0];
		localId = ids[1];

		component = this.env.getPersistenceFacade().find(DeviceComponent.class, requestId);
		
		return "component";
	}
	public DeviceComponent getComponent() {
		return component;
	}
	
	public String getControlPage(){
		if(component == null){
			if(deviceType == null ||deviceType.trim().equals("") )
				return "Device_general";
			return "Device_" + deviceType;
		}
		
		return "Component_" + component.getComponentType();
	}
	
	public String request(){
		channels = this.env.getPersistenceFacade().find(GeneralDevice.class, id).getSupportedChannel();
		if(channels == null || channels.isEmpty())
			return "NOCHANNEL";
		Request r = new Request();
		r.to = id;
		r.source = user.getName();
		r.component = localId;
		r.action = action;
		DeviceGateway dg = env.getDeviceGatewayManager().get(channels.toArray()[0].toString());
		dg.send(r);
		return control();
		
	}
	public void setAction(String action) {
		this.action = action;
	}

}
