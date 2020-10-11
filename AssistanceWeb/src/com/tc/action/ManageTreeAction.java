package com.tc.action;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.struts2.interceptor.SessionAware;

import com.tc.assistance.entity.business.DeviceUser;
import com.tc.assistance.entity.device.GeneralDevice;


public class ManageTreeAction extends EnvironmentSupport implements SessionAware{
	  DeviceUser user;
	  
		@Override
	public String execute() throws Exception {
		// TODO Auto-generated method stub
		return SUCCESS;
	}
		@Override
		public void setSession(Map<String, Object> s) {
			user = (DeviceUser)s.get("user");
					}
		  public Category getTreeRootNode() {
			  List<GeneralDevice> devices = this.env.getPersistenceFacade().query("from GeneralDevice where ownerId = '" + user.getName() + "'");
			  Category cat = new Category("1", user.getName());
			  for(GeneralDevice d : devices){
				  Category cl = new Category(d.getId(), d.getName());
				  cat.getChildren().add(cl);
			  }
			  return cat;
		
			  
		  
		  }
}
