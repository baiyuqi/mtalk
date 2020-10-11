package com.tc.assistance.entity.device;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.persistence.*;

import org.hibernate.annotations.CollectionOfElements;
@Entity
public class GeneralDevice {

	private static final long serialVersionUID = 1L;
	String id;
	String name;
	String type;
	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	String ownerId;
	Set<String> supportedChannel;
	@CollectionOfElements(fetch=FetchType.EAGER)
	public Set<String> getSupportedChannel() {
		return supportedChannel;
	}

	public void setSupportedChannel(Set<String> supportedChannel) {
		this.supportedChannel = supportedChannel;
	}

	public String getOwnerId() {
		return ownerId;
	}

	public void setOwnerId(String ownerId) {
		this.ownerId = ownerId;
	}

	@Id
	public String getId() {
		return id;
	}

	

	public void setId(String id) {
		this.id = id;
	}


   
	Map<String, DeviceComponent> components = new HashMap<String, DeviceComponent>();


	@OneToMany(cascade=CascadeType.ALL,fetch=FetchType.EAGER)
	public Map<String, DeviceComponent> getComponents() {
		return components;
	}
	public void setComponents(Map<String, DeviceComponent> components) {
		this.components = components;
	}

	
	public GeneralDevice() {
		super();
		
	}
	@Override
	public String toString() {
		// TODO Auto-generated method stub
		String r = "id: " + this.id;
		if(components != null){
		for(String k : components.keySet()){
			r += " " + components.get(k).componentType + " " + k + " : " + components.get(k).currentStatus;
		}
		}
		return r;
	}
	@Transient
	public DeviceStatus getStatus(){
		DeviceStatus stat = new DeviceStatus();
		stat.setDeviceId(id);
		for(DeviceComponent dc : this.components.values()){
			String v =  dc.currentStatus;
			
			stat.getValues().add(new StatusItem(dc.getLocalId(),v));
		}
		return stat;
		
	}

	DeviceComponent[] deviceComponents;
	String channel;
	
	public void componentId(){
		for(DeviceComponent d: components.values())
			d.setId(id + "_" + d.getLocalId());
	}
	
   
}
