package com.tc.assistance.business;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.PersistenceContext;

import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;

@Stateless
public class DeviceFacade implements DeviceFacadeLocal {

	@PersistenceContext
	protected EntityManager em;

	public DeviceFacade() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public Map<String, GeneralDevice> all() {
		Map<String, GeneralDevice> devices = new HashMap<String, GeneralDevice>();
		List<GeneralDevice> db = (List<GeneralDevice>) em.createQuery("from GeneralDevice")
				.getResultList();
		for (GeneralDevice d : db)
			devices.put(d.getId(), d);
		return devices;

	}

	@Override
	public DeviceRelation relation(String deviceId) {
		List<String> in = em.createQuery("select fromId from " + Relationship.class.getSimpleName() + " where toId='" + deviceId + "'").getResultList();
		List<String> out = em.createQuery("select toId from " + Relationship.class.getSimpleName() + " where fromId='" + deviceId + "'").getResultList();
		return new DeviceRelation(in, out);
	}

	@Override
	public DetailedRelation detailedRelation(String deviceId) {
		List<Relationship> in = em.createQuery("from " + Relationship.class.getSimpleName() + " where toId='" + deviceId + "'").getResultList();
		List<Relationship> out = em.createQuery("from " + Relationship.class.getSimpleName() + " where fromId='" + deviceId + "'").getResultList();
		return new DetailedRelation(in, out);
	}

}
