package com.tc.assistance.streamer;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import javax.servlet.ServletInputStream;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import com.tc.assistance.entity.business.DeviceUser;
import com.tc.assistance.entity.business.Person;
import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.DeviceComponentType;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.DeviceStatus;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.entity.device.StatusItem;
import com.tc.assistance.parser.ProtocolParser;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.assistance.protocol.entity.Report;
import com.tc.assistance.protocol.entity.Request;
import com.tc.servlet.DeviceService;
import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.json.JettisonMappedXmlDriver;
import com.thoughtworks.xstream.io.json.JsonHierarchicalStreamDriver;

public class Configuration {
	static Configuration instance;
	static{
		instance = new Configuration();
		
	}
	static public Configuration getInstance(){
		return instance;
	}
	XStream[] streams;
	public XStream getXmlStream(){
		return streams[0];
	}
	public Configuration(){
		streams = new XStream[2];
		streams[0] = new XStream();
		streams[1] = new XStream(new JsonHierarchicalStreamDriver());
		for(XStream stream : streams){
			stream.setMode(XStream.NO_REFERENCES);
	
		
		
		stream.alias("deviceComponentType", DeviceComponentType.class);
		stream.useAttributeFor(DeviceComponentType.class, "typeId");
		stream.useAttributeFor(DeviceComponentType.class, "name");
		stream.useAttributeFor(DeviceComponentType.class, "description");
		
		
		stream.alias("deviceUser", DeviceUser.class);
		stream.useAttributeFor(Person.class, "password");
		stream.useAttributeFor(Person.class, "name");
		}

	}
	public Map<String, GeneralDevice> getPrototypes(){
		Map<String, GeneralDevice> rst = new HashMap<String, GeneralDevice>();
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;
			try {
				docBuilderFactory = DocumentBuilderFactory.newInstance();
				docBuilder = docBuilderFactory.newDocumentBuilder();
				doc = docBuilder.parse(this.getClass().getResourceAsStream("/DeviceProtoTypes.xml"));
				Element root = doc.getDocumentElement();
				NodeList nl = root.getElementsByTagName("generalDevice");
				for(int i = 0; i < nl.getLength(); i++){
					Element de = (Element)nl.item(i);
					GeneralDevice d = ProtocolParser.parseDeviceDefination(de);
					rst.put(d.getName(), d);
				}
				
				return rst;
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
	}
	
	
}





