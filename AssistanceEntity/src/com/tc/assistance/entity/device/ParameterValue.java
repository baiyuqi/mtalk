package com.tc.assistance.entity.device;

import java.io.Serializable;
import javax.persistence.*;

/**
 * Entity implementation class for Entity: ParameterValue
 *
 */
@Entity

public class ParameterValue implements Serializable {

	
	private static final long serialVersionUID = 1L;
	String id;
	String deviceId;
	String parameterName;
	double value;
	public String getDeviceId() {
		return deviceId;
	}

	public void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}

	public String getParameterName() {
		return parameterName;
	}

	public void setParameterName(String parameterName) {
		this.parameterName = parameterName;
	}

	public double getValue() {
		return value;
	}

	public void setValue(double value) {
		this.value = value;
	}

	@Id
	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}
	public ParameterValue() {
		super();
	}
   
}
