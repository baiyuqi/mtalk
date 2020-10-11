package com.tc.action;

import java.io.ByteArrayOutputStream;
import java.util.List;
import java.util.Map;

import org.apache.struts2.interceptor.SessionAware;

import com.opensymphony.xwork2.ModelDriven;
import com.opensymphony.xwork2.Preparable;
import com.tc.assistance.entity.business.DeviceUser;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.parser.ProtocolStreamer;
import com.thoughtworks.xstream.XStream;

public class DeviceAction extends BaseAction implements ModelDriven<GeneralDevice>,Preparable,  SessionAware{         
	DeviceUser user;
	String prototype;
	public String getPrototype() {
		return prototype;
	}
	public void setPrototype(String prototype) {
		this.prototype = prototype;
	}

	private GeneralDevice model;
	  public GeneralDevice getModel() {
	    return model;
	  }
	  public void prepare() throws Exception {
	    if (getId()==null) {
	      return;
	    } else {
	      model = this.env.getPersistenceFacade().find(GeneralDevice.class, getId());
	      if(model == null){
	    	  model = this.env.getDeviceManager().getPrototypes().get(this.prototype);
	    	  XStream s = new XStream();
	    	  model = (GeneralDevice)s.fromXML(s.toXML(model));
	    	  model.setId(getId());
	    	  model.componentId();
	      	  model.setOwnerId(user.getName());
	      }
	    }
	  }
	@Override
	protected List getModelList() {
		 return this.env.getPersistenceFacade().query("from GeneralDevice where ownerId = '" + user.getName() + "'");

	}
	@Override
	public void setSession(Map<String, Object> s) {
		user = (DeviceUser)s.get("user");
		
	}
	@Override
	public Class getModelType() {
		// TODO Auto-generated method stub
		return GeneralDevice.class;
	}

	
	 public List<Relationship> getInRelation(){
		  return this.env.getPersistenceFacade().query("from Relationship where toId = '" + getId() + "'");
	  }
	 public List<Relationship> getOutRelation(){
		  return this.env.getPersistenceFacade().query("from Relationship where fromId = '" + getId() + "'");
	  }
	 
	String localId;
	String componentType;

	public String getLocalId() {
		return localId;
	}
	public void setLocalId(String localId) {
		this.localId = localId;
	}
	public String getComponentType() {
		return componentType;
	}
	public void setComponentType(String componentType) {
		this.componentType = componentType;
	}
	
	String componentName;
	public String getComponentName() {
		return componentName;
	}
	public void setComponentName(String componentName) {
		this.componentName = componentName;
	}
	public String editComponent(){
		 DeviceComponent rs = model.getComponents().get(localId);
		
		 componentName= rs.getName();
		 componentType = rs.getComponentType();
		 return "editComponent";
	 }
	public String updateComponent(){
		DeviceComponent rs = model.getComponents().get(localId);
		rs.setName(componentName);
		this.env.getPersistenceFacade().merge( this.getModel());
		return "edit";
	}
	public String removeComponent(){
		model.getComponents().remove(localId);
		
		this.env.getPersistenceFacade().merge( this.getModel());
		return "edit";
	}
	public String createComponent(){
		 DeviceComponent rs = new DeviceComponent();
		 rs.setLocalId(localId);
		 rs.setName(componentName);
		 rs.setComponentType(componentType);
		 this.getModel().getComponents().put(localId, rs);
		 this.getModel().componentId();
		 this.env.getPersistenceFacade().merge( this.getModel());
		return "edit";
	 }
	public String getDeviceDifination(){
		if(getModel() != null){
			ByteArrayOutputStream out = new ByteArrayOutputStream();
		
				ProtocolStreamer.streamDevice(out, model);
		
			 return new String(out.toByteArray());
		}
		return null;
	}
}
