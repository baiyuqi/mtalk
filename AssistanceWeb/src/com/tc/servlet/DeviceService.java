package com.tc.servlet;

import java.io.IOException;


import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;



import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.parser.ProtocolParser;
import com.tc.assistance.parser.ProtocolStreamer;

import com.tc.clientmessage.DeviceStatusUpdate;
import com.tc.env.EnvironmentServlet;

public class DeviceService extends EnvironmentServlet {
	private static final long serialVersionUID = 1L;


	protected void doGet(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		String command = request.getParameter("command");
		if(command.equals("status_read"))
			readStatus(request, response);
		if(command.equals("status_write"))
			writeStatus(request, response);
		if(command.equals("defination_read"))
			readDefination(request, response);
		if(command.equals("defination_write"))
			writeDefination(request, response);
		if(command.equals("relation_read"))
			readRelation(request, response);
		if(command.equals("relation_write"))
			writeRelation(request, response);
	}
	
	private void writeDefination(HttpServletRequest request,
			HttpServletResponse response) {
		try {
			GeneralDevice d = ProtocolParser.parseDeviceDefination(request.getInputStream());
			d.componentId();
			boolean exist = env.getPersistenceFacade().exist(GeneralDevice.class, d.getId());
			if(exist)
				return;
			env.getPersistenceFacade().persist(d);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
	}
	boolean validate(String user, String pwd){
		return true;
	}
	private void readRelation(HttpServletRequest request,
			HttpServletResponse response) {
		String deviceId = request.getParameter("deviceId");
		
		boolean detailed = Boolean.parseBoolean(request.getParameter("detailed"));
		try {
			if(detailed){
				DetailedRelation dr = env.getDeviceFacade().detailedRelation(deviceId);
				com.tc.assistance.parser.ProtocolStreamer.streamDetailedRelation(response.getOutputStream(),dr);
			}
			else{
				DeviceRelation re = env.getDeviceFacade().relation(deviceId);
				ProtocolStreamer.streamSimpleRelation(response.getOutputStream(), re);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	private void writeRelation(HttpServletRequest request,
			HttpServletResponse response) {
		try {
			DetailedRelation d = ProtocolParser.parseDeviceRelation(request.getInputStream());
			//env.getPersistenceFacade().persist(d);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
	}



	private void readDefination(HttpServletRequest request,
			HttpServletResponse response) {
		String id = request.getParameter("deviceId");
		GeneralDevice d = env.getDeviceManager().getDevice(id);
		if(d != null){
			try {
				ProtocolStreamer.streamDevice(response.getOutputStream(), d);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}



	private void writeStatus(HttpServletRequest request, HttpServletResponse response) {
		String id = request.getParameter("deviceId");
		GeneralDevice d = env.getDeviceManager().getDevice(id);
		for(DeviceComponent c : d.getComponents().values()){
			
			String localId = c.getLocalId();
			String value = request.getParameter(localId);
			c.setCurrentStatus(value);
		}
		
		
		DeviceStatusUpdate msg = new DeviceStatusUpdate();
		msg.setDevice(d);
		this.env.getAlarmRegister().message(msg );
	}

	private void readStatus(HttpServletRequest request, HttpServletResponse response) {
		String id = request.getParameter("deviceId");
		GeneralDevice d = env.getDeviceManager().getDevice(id);
		try {
			com.tc.assistance.parser.ProtocolStreamer.streamDeviceStatus(response.getOutputStream(), d);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		doGet(request,response);
	}
	
}
