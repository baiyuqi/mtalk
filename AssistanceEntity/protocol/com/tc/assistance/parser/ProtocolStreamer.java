package com.tc.assistance.parser;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;


import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.DeviceStatus;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.entity.device.StatusItem;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.assistance.protocol.entity.Request;

public class ProtocolStreamer {

	static public String stream(ProtocolMessage pm){
		XmlSerializer serializer = ParserFactory.getSerializer();
		StringWriter writer = new StringWriter();
		try {
			serializer.setOutput(writer);
			if(pm instanceof Request)
				serializer.startTag("", "request");
			else serializer.startTag("", "report");
			if(pm.source != null && !pm.source.trim().equals(""))
				serializer.attribute("", "source", pm.source);
			
				serializer.attribute("", "action", pm.action);
				serializer.attribute("", "component", pm.component);
				if(pm.asstributes != null){
				for(String key : pm.asstributes.keySet())
					serializer.attribute("", key, pm.asstributes.get(key));
				}
				
				if(pm instanceof Request)
					serializer.endTag("", "request");
				else serializer.endTag("", "report");
			serializer.endDocument();
			return writer.toString();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	static public void streamDevice(OutputStream out, GeneralDevice o){
		XmlSerializer serializer = ParserFactory.getSerializer();
		try {
			serializer.setOutput(out, "UTF-8");
			streamDevice(serializer,o);
			serializer.flush();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalStateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	static public  void streamDevice(XmlSerializer serializer, GeneralDevice o) throws Exception{
		serializer.startTag("", "generalDevice");
		serializer.attribute("", "id", o.getId());
		serializer.attribute("", "name", o.getName());
		if(o.getType() != null)
			serializer.attribute("", "type", o.getType());
		serializer.attribute("", "ownerId", o.getOwnerId());
		serializer.attribute("", "supportedChannel", o.getSupportedChannel().toString());
		serializer.startTag("", "deviceComponents");
		for(DeviceComponent c : o.getComponents().values()){
			serializer.startTag("", "deviceComponent");
			serializer.attribute("", "localId", c.getLocalId());
			serializer.attribute("", "name", c.getName());
			serializer.attribute("", "componentType", c.getComponentType());
			serializer.endTag("", "deviceComponent");
		}
		serializer.endTag("", "deviceComponents");
		serializer.endTag("", "generalDevice");
	}
	public static void streamSimpleRelation(OutputStream out, DeviceRelation o) {

			try {
				XmlSerializer serializer = ParserFactory.getSerializer();
				serializer.setOutput(out, "utf-8");
				serializer.startTag("", "relation");
				serializer.attribute("", "in", new HashSet<String>(o.in).toString());
				serializer.attribute("", "out", new HashSet<String>(o.out).toString());
				serializer.endTag("", "relation");
				serializer.flush();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalStateException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
	}
	public static void streamDetailedRelation(OutputStream out, DetailedRelation relation) {
		try {
			XmlSerializer serializer = ParserFactory.getSerializer();
			serializer.setOutput(out, "UTF-8");
			serializer.startTag("", "detailedRelation");
			serializer.startTag("", "in");
			for(Relationship r : relation.in){
				serializer.startTag("", "relationship");
				serializer.attribute("", "fromId", r.getFromId());
				serializer.attribute("", "toId", r.getToId());
				serializer.attribute("", "name", r.getName()==null?"":r.getName());
				serializer.attribute("", "reverseName", r.getReverseName()==null?"":r.getReverseName());
				serializer.endTag("", "relationship");
			}
			serializer.endTag("", "in");
			serializer.startTag("", "out");
			for(Relationship r : relation.out){
				serializer.startTag("", "relationship");
				serializer.attribute("", "fromId", r.getFromId());
				serializer.attribute("", "toId", r.getToId());
				serializer.attribute("", "name",  r.getName()==null?"":r.getName());
				serializer.attribute("", "reverseName", r.getReverseName()==null?"":r.getReverseName());
				serializer.endTag("", "relationship");
			}
			serializer.endTag("", "out");
			serializer.endTag("", "detailedRelation");
			serializer.flush();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalStateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		}
	public static void streamDeviceStatus(OutputStream out, GeneralDevice device) {
	
			try {
				XmlSerializer serializer = ParserFactory.getSerializer();
				serializer.setOutput(out, "utf-8");
				serializer.startTag("", "deviceStatus");
				for(StatusItem i : device.getStatus().getValues()){
					serializer.startTag("", "statusItem");
					serializer.attribute("", "localId", i.getLocalId());
					serializer.attribute("", "status", i.getStatus()==null?"":i.getStatus());
					serializer.endTag("", "statusItem");
				}
				serializer.endTag("", "deviceStatus");
				serializer.flush();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalStateException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	
	static public void main(String[] s){
		
	}
}
