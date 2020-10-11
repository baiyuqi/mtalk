package com.tc.action;

import org.apache.struts2.interceptor.SessionAware;

import com.opensymphony.xwork2.ModelDriven;
import com.opensymphony.xwork2.Preparable;
import com.tc.assistance.entity.device.Relationship;

public class DeviceRelationAction extends EnvironmentSupport implements Preparable,  ModelDriven<Relationship>{
	Relationship model;
	
	public void setModel(Relationship model) {
		this.model = model;
	}

	String id;
	

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	@Override
	public Relationship getModel() {
		// TODO Auto-generated method stub
		return model;
	}

	@Override
	public void prepare() throws Exception {
		if(id == null)
			model = new Relationship();
		else 
			model = env.getPersistenceFacade().find(Relationship.class, id);
		
	}
	public String add(){
		
		return "add";
	}
public String edit(){
		
		return "edit";
	}
	public String save(){
		model.setId(model.getFromId()  + "-" + model.getToId());
		env.getPersistenceFacade().persist(model);
		return "success";
	}
	public String update(){
		env.getPersistenceFacade().merge(model);
		return "success";
	}
	public String remove(){
		env.getPersistenceFacade().remove(Relationship.class, id);
		return "success";
	}
}
