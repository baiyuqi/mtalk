package com.tc.assistance.parser;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.FactoryConfigurationError;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.xmlpull.v1.XmlSerializer;

import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.DeviceRelation;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.protocol.entity.DefinationReport;
import com.tc.assistance.protocol.entity.LocationReport;
import com.tc.assistance.protocol.entity.PictureReport;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.assistance.protocol.entity.ProtocolMessageWrapper;
import com.tc.assistance.protocol.entity.Report;
import com.tc.assistance.protocol.entity.Request;


public class ProtocolParser {
	static Set<String> fields = new HashSet<String>();
	static{
		fields.add("action");fields.add("component");
		fields.add("from");fields.add("to");
		fields.add("source");
	}
	static ProtocolMessageWrapper wrap(ProtocolMessage report){
		if(report.action.equals(ProtocolMessage.COMMAND_PIC))
				return new PictureReport((Report)report);
		if(report.action.equals(ProtocolMessage.COMMAND_LOCATION))
			return new LocationReport((Report)report);
		if(report.action.equals(ProtocolMessage.COMMAND_PROFILE))
			return new DefinationReport((Report)report);
		return new ProtocolMessageWrapper(report);
	}
	static public ProtocolMessageWrapper parseAndWrap(String xml){
		ProtocolMessage m = parseProtocolMessage(xml);
		
		return wrap((Report) m);
		
	}
	
	static public ProtocolMessage parseProtocolMessage(String xml){
		
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;
			try {
				docBuilderFactory = DocumentBuilderFactory.newInstance();
				docBuilder = docBuilderFactory.newDocumentBuilder();
				InputStream in = new ByteArrayInputStream(xml.getBytes());
				doc = docBuilder.parse(in);
				
				Element re = doc.getDocumentElement();
				ProtocolMessage r;
				if(re.getNodeName().equals("request"))
					r = new Request();
				else r = new Report();
				NamedNodeMap map = re.getAttributes();
				for(int i = 0; i < map.getLength(); i++){
					Node n = map.item(i);
					String name = n.getNodeName();
					String value = n.getNodeValue();
					if(fields.contains(name)){
						r.getClass().getField(name).set(r, value);
					}else{
						r.asstributes.put(name, value);
					}
				}
				return r;
			}catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
	}
	
	static public GeneralDevice parseDeviceDefination(InputStream stream){
		
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;
			try {
				docBuilderFactory = DocumentBuilderFactory.newInstance();
				docBuilder = docBuilderFactory.newDocumentBuilder();
				doc = docBuilder.parse(stream);
				Element root = doc.getDocumentElement();
				return parseDeviceDefination(root);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		return null;
	}
	static public GeneralDevice parseDeviceDefination(Element nd){
		GeneralDevice ca = new GeneralDevice();

		String mid = nd.getAttribute("id");
		ca.setId(mid);
		ca.setName(nd.getAttribute("name"));
		
		ca.setOwnerId(nd.getAttribute("ownerId"));
		ca.setType(nd.getAttribute("type"));

		String sc = nd.getAttribute("supportedChannel");
		sc = sc.substring(1, sc.length() - 1);
		String[] ss = sc.split(",");
		if(ss.length > 0){
			Set<String> s = new HashSet<String>();
			for(String se : ss)
				s.add(se);
			ca.setSupportedChannel(s);
		}
		NodeList dcs = nd.getElementsByTagName("deviceComponents");
		if(dcs != null && dcs.getLength() != 0){
			Element dd = (Element)dcs.item(0);
			
			Map<String, DeviceComponent> coms = components(dd);
			ca.setComponents(coms);
		}
		return ca;
	}
	
	static Map<String, DeviceComponent> components(Element node){
		Map<String, DeviceComponent> coms = new HashMap<String, DeviceComponent>();
		NodeList nl = node.getElementsByTagName("deviceComponent");
		for (int j = 0; j < nl.getLength(); j++) {
			Element com = (Element)nl.item(j);
		
				DeviceComponent dc = new DeviceComponent();
				dc.setComponentType(com.getAttribute("componentType"));
				dc.setLocalId(com.getAttribute("localId"));
				dc.setName(com.getAttribute("name"));
				coms.put(dc.getLocalId(), dc);

		}
		return coms;
	}
	static public Map<String, String> parseDeviceParams(InputStream stream){
		Map<String, String> rst = new HashMap<String, String>();
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;
		try {
			docBuilderFactory = DocumentBuilderFactory.newInstance();
			docBuilder = docBuilderFactory.newDocumentBuilder();
			// xml file 放到 assets目录中的
			doc = docBuilder.parse(stream );
			
			// root element
			Element root = doc.getDocumentElement();

				NodeList vs = root.getElementsByTagName("statusItem");
				for(int i = 0; i < vs.getLength(); i++){
					Element v = (Element)vs.item(i);
					rst.put(v.getAttribute("localId"),v.getAttribute("status"));
				}

			
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			doc = null;
			docBuilder = null;
			docBuilderFactory = null;
		}
		return rst;
	}
public static DetailedRelation parseDeviceRelation(InputStream stream) {
	DocumentBuilderFactory docBuilderFactory = null;
	DocumentBuilder docBuilder = null;
	Document doc = null;

		docBuilderFactory = DocumentBuilderFactory.newInstance();
		try {
			docBuilder = docBuilderFactory.newDocumentBuilder();
			doc = docBuilder.parse(stream);
		} catch (Exception e) {
		}
		Element root = doc.getDocumentElement();
		return  parseDeviceRelation(root);

	}
	public static DetailedRelation parseDeviceRelation(Element relationNode) {
	
		
		Element in = (Element)relationNode.getElementsByTagName("in").item(0);
		Element out = (Element)relationNode.getElementsByTagName("out").item(0);
		
		return new DetailedRelation(relations(in), relations(out));
	}
	static List<Relationship> relations(Element root){
		List<Relationship> rst = new ArrayList<Relationship>();
		NodeList nrs = root.getElementsByTagName("relationship");
		for(int i = 0; i < nrs.getLength(); i++)
		{
			Element er = (Element)nrs.item(i);
			Relationship r = new Relationship();
			r.setFromId(er.getAttribute("fromId"));
			r.setToId(er.getAttribute("toId"));
			r.setName(er.getAttribute("name"));
			r.setReverseName(er.getAttribute("reverseName"));
			rst.add(r);
		}
		return rst;
	}
	public static DeviceRelation  parseSimpleRelation(InputStream stream) {
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;

			docBuilderFactory = DocumentBuilderFactory.newInstance();
			try {
				docBuilder = docBuilderFactory.newDocumentBuilder();
				doc = docBuilder.parse(stream);
			} catch (Exception e) {
			}
			Element root = doc.getDocumentElement();
			String ins = root.getAttribute("in");
			ins = ins.substring(1, ins.length() - 1);
			String[] ss = ins.split(",");
			List<String> in = new ArrayList<String>();
			if(ss.length > 0){
				
				for(String se : ss)
					in.add(se);
			}
			
			String outs = root.getAttribute("out");
			outs = outs.substring(1, ins.length() - 1);
			ss = outs.split(",");
			List<String> out = new ArrayList<String>();
			if(ss.length > 0){
				
				for(String se : ss)
					out.add(se);
			}
			DeviceRelation r = new DeviceRelation(in, out);
			return  r;

	
}
}
