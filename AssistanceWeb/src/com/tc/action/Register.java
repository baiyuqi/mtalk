package com.tc.action;

import com.tc.assistance.entity.business.DeviceUser;

public class Register extends EnvironmentSupport {
	String username;
	String passwordconfirm;
	public String getPasswordconfirm() {
		return passwordconfirm;
	}
	public void setPasswordconfirm(String passwordconfirm) {
		this.passwordconfirm = passwordconfirm;
	}
	String password;
	private String deviceId;
	
	public String getDeviceId() {
		return deviceId;
	}
	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}
	@Override
	public String execute() throws Exception {
		DeviceUser user = new DeviceUser();
		user.setName(username);
		user.setPassword(password);
	
		this.env.getPersistenceFacade().persist(user);
		return this.SUCCESS;
	}
	public String getUsername() {
		return username;
	}
	public void setUsername(String username) {
		this.username = username;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
}
