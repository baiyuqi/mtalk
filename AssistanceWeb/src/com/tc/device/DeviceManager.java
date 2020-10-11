package com.tc.device;

import java.io.ByteArrayOutputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.naming.InitialContext;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;

import com.tc.assistance.business.DeviceFacadeLocal;
import com.tc.assistance.business.PersistenceFacade;
import com.tc.assistance.entity.business.DeviceUser;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.DeviceComponentType;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.parser.ProtocolStreamer;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.assistance.protocol.entity.Report;
import com.tc.assistance.protocol.entity.Request;
import com.tc.assistance.streamer.Configuration;
import com.tc.dwr.AlarmRegister;
import com.tc.servlet.DeviceService;
import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.json.JsonHierarchicalStreamDriver;

public class DeviceManager {
	
	public DeviceManager(PersistenceFacade f) {
		super();
		facade = f;

		init();
	}
	PersistenceFacade facade;
	Map<String, DeviceComponentType> componentTypes;
	Map<String, GeneralDevice> devices = new HashMap<String, GeneralDevice>();
	Map<String, GeneralDevice> prototypes = new HashMap<String, GeneralDevice>();
	public Map<String, GeneralDevice> getPrototypes() {
		return prototypes;
	}
	DeviceFacadeLocal deviceFacade;

	void init(){
		XStream stream = Configuration.getInstance().getXmlStream();

		stream.alias("request", Request.class);
		
		stream.useAttributeFor(ProtocolMessage.class, "action");
		stream.useAttributeFor(ProtocolMessage.class, "component" );
		try {
//			List<DeviceUser> u = (List<DeviceUser>)stream.fromXML(DeviceService.class.getResourceAsStream("/simpledeviceusers.xml"));
//			facade.persist(u);
			componentTypes = new HashMap<String, DeviceComponentType>();
			List<DeviceComponentType> ps = (List<DeviceComponentType>)stream.fromXML(DeviceService.class.getResourceAsStream("/DeviceComponentType.xml"));
			for(DeviceComponentType p : ps){
				componentTypes.put(p.getTypeId(), p);
			}
			InitialContext ctx = new InitialContext(); 
			deviceFacade = (DeviceFacadeLocal) ctx
			.lookup("AssistanceStudio/DeviceFacade/local");
			devices = deviceFacade.all();
			for(GeneralDevice d : devices.values()){
				
				devices.put(d.getId(), d);
			}
			
			prototypes = Configuration.getInstance().getPrototypes();
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	public GeneralDevice getDevice(String deviceId){
		return devices.get(deviceId);
		
	}
	public Map<String, DeviceComponentType> getProfiles() {
		return componentTypes;
	}

	static public void main(String[] s){
		Map<String, GeneralDevice> prototypes = Configuration.getInstance().getPrototypes();
		for(GeneralDevice d : prototypes.values()){
			ByteArrayOutputStream out = new ByteArrayOutputStream();
			
			ProtocolStreamer.streamDevice(out, d);
	
		System.out.println(new String(out.toByteArray()));
		}
	}
}
