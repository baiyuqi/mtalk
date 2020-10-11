package com.tc.assistance.entity.device;

import java.io.Serializable;
import java.lang.String;
import java.util.Map;

import javax.persistence.*;

/**
 * Entity implementation class for Entity: DeviceProfile
 *
 */

public class DeviceComponentType implements Serializable {

	

	public String getTypeId() {
		return typeId;
	}
	public void setTypeId(String typeId) {
		this.typeId = typeId;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getDescription() {
		return description;
	}
	public void setDescription(String description) {
		this.description = description;
	}
	private String typeId;
	private String name;
	private String description;
   
}
