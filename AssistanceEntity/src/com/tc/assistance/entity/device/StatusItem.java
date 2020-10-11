package com.tc.assistance.entity.device;

public class StatusItem{
	public StatusItem(String localId, String status) {
		super();
		this.localId = localId;
		this.status = status;
	}
	String localId;
	public String getLocalId() {
		return localId;
	}
	public void setLocalId(String localId) {
		this.localId = localId;
	}
	public String getStatus() {
		return status;
	}
	public void setStatus(String status) {
		this.status = status;
	}
	String status;
}