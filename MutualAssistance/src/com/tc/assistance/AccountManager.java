package com.tc.assistance;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.StringWriter;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.xmlpull.v1.XmlSerializer;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.client.ServerConfig;
import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.DeviceComponent;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.parser.ProtocolParser;
import com.tc.assistance.parser.ProtocolStreamer;

import android.content.Context;
import android.content.ContextWrapper;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.PreferenceManager;
import android.telephony.TelephonyManager;
import android.util.Xml;

public class AccountManager {
	public GeneralDevice getMe() {
		return me;
	}
	static AccountManager instance;
	static AccountManager getInstance(ContextWrapper ctx){
		instance = new AccountManager(ctx);
		return instance;
	}
	static public AccountManager getInstance(){
		return instance;
	}
	ContextWrapper context;
	DetailedRelation detailedRelation;
	public DetailedRelation getDetailedRelation() {
		return detailedRelation;
	}
	String[] groups = new String[]{"关心我的人","我照料的人"};
	Map<String, GeneralDevice> devices;
	GeneralDevice me;

	
	AccountManager(ContextWrapper ctx) {
		this.context = ctx;
		HttpServer.getInstance().getServerConfig().load(ctx);
		me = createMe();
		TelephonyManager  ts = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
		String number = ts.getLine1Number();
		devices = loadDevice(ctx);
		detailedRelation = loadAccount(ctx);
		if(detailedRelation == null)
			return;
		detailedRelation.computeCared(me.getId());
		
		for(Relationship r : detailedRelation.out){
			r.setDevice(devices.get(r.getToId()));
		}
	}
	
	GeneralDevice createMe(){
		GeneralDevice d = new GeneralDevice();
		d.setName("Android手机");
		d.setType("phone");
		d.setId("1111");
		d.setOwnerId(HttpServer.getInstance().getServerConfig().user);
		Set<String> sc = new HashSet<String>();
		sc.add("sms");
		d.setSupportedChannel(sc);
		Map<String, DeviceComponent> dcs = new HashMap<String, DeviceComponent>();
		DeviceComponent dc = new DeviceComponent();
		dc.setComponentType("phone");
		dc.setLocalId("phone");
		dc.setName("phone");
		dcs.put(dc.getLocalId(), dc);
		d.setComponents(dcs);
		return d;
	}
	private Map<String, GeneralDevice> loadDevice(Context ctx) {
		Map<String, GeneralDevice> rst = new HashMap<String, GeneralDevice>();
//		File f = new File("devices.xml");
//		if(!f.exists())
//			return rst;
//		InputStream in = null;
//		try {
//			in = new FileInputStream("devices.xml");
//		} catch (FileNotFoundException e1) {
//			// TODO Auto-generated catch block
//			e1.printStackTrace();
//		}
		
		SharedPreferences pref = PreferenceManager
		.getDefaultSharedPreferences(ctx);
		String dvs = pref.getString("devices", null);
		if(dvs == null)
			return rst;
		InputStream in = new ByteArrayInputStream(dvs.getBytes());
		DocumentBuilderFactory docBuilderFactory = null;
		DocumentBuilder docBuilder = null;
		Document doc = null;
		try {
			docBuilderFactory = DocumentBuilderFactory.newInstance();
			docBuilder = docBuilderFactory.newDocumentBuilder();
			// xml file 放到 assets目录中的
			
			doc = docBuilder.parse(in);
			Element root = doc.getDocumentElement();
			NodeList nodeList = root.getElementsByTagName("generalDevice");

			for (int i = 0; i < nodeList.getLength(); i++) {
				
				Node nd = nodeList.item(i);
				GeneralDevice ca = ProtocolParser.parseDeviceDefination((Element)nd);
				rst.put(ca.getId(), ca);
			}
		} catch (IOException e) {
		} catch (SAXException e) {
		} catch (ParserConfigurationException e) {
		} finally {
			doc = null;
			docBuilder = null;
			docBuilderFactory = null;
		}
		return rst;
		
	}

	public DetailedRelation getAccounts(){
		return detailedRelation;
	}
	public void saveAccounts(Context ctx){
		SharedPreferences pref = PreferenceManager
		.getDefaultSharedPreferences(ctx);
		Editor e = pref.edit();
		String xml = xmlRelation();
		e.putString("accounts", xml);
		//write("accounts.xml", xml);
		String dvs = xmlDevice();
		e.putString("devices", dvs);
		//write("devices.xml", xml);
	}
	public String[] getGroup(){
		return this.groups;
	}
	DetailedRelation loadAccount(Context ctx) {
//		File f = new File("accounts.xml");
//		if(!f.exists())
//			return null;
//		InputStream in = null;
//		try {
//			in = new FileInputStream(f);
//		} catch (FileNotFoundException e1) {
//			// TODO Auto-generated catch block
//			e1.printStackTrace();
//			return null;
//		}
		SharedPreferences pref = PreferenceManager
		.getDefaultSharedPreferences(ctx);
		String dvs = pref.getString("accounts", null);
		if(dvs == null)
			return null;
		InputStream in = new ByteArrayInputStream(dvs.getBytes());
			return ProtocolParser.parseDeviceRelation(in);
		
		
	}
	
	private String xmlRelation() {

		XmlSerializer serializer = Xml.newSerializer();
		StringWriter writer = new StringWriter();
		try {
			serializer.setOutput(writer);

			// <?xml version=”1.0″ encoding=”UTF-8″ standalone=”yes”?>
			serializer.startDocument("UTF-8", true);
			serializer.startTag("", "detailedRelation");
			serializer.startTag("", "in");
			for(Relationship o: this.detailedRelation.in){
				
				serializer.startTag("", "relationship");
				serializer.attribute("", "fromId", o.getFromId());
				serializer.attribute("", "toId", o.getToId());
				serializer.attribute("", "name", o.getName());
				serializer.attribute("", "reverseName", o.getReverseName());
				serializer.endTag("", "relationship");
			} 
			serializer.endTag("", "in");
			serializer.startTag("", "out");
			for(Relationship o: this.detailedRelation.in){
				
				serializer.startTag("", "relationship");
				serializer.attribute("", "fromId", o.getFromId());
				serializer.attribute("", "toId", o.getToId());
				serializer.attribute("", "name", o.getName());
				serializer.attribute("", "reverseName", o.getReverseName());
				serializer.endTag("", "relationship");
			} 
			serializer.endTag("", "out");
			serializer.endTag("", "detailedRelation");
			serializer.endDocument();
			return writer.toString();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	private String xmlDevice() {

		XmlSerializer serializer = Xml.newSerializer();
		StringWriter writer = new StringWriter();
		try {
			serializer.setOutput(writer);

			// <?xml version=”1.0″ encoding=”UTF-8″ standalone=”yes”?>
			serializer.startDocument("UTF-8", true);
			serializer.startTag("", "devices");
			
			for(GeneralDevice o: devices.values()){
				ProtocolStreamer.streamDevice(serializer, o);
			} 
			
			serializer.endTag("", "devices");
			serializer.endDocument();
			return writer.toString();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	private String mydefination() {

		XmlSerializer serializer = Xml.newSerializer();
		StringWriter writer = new StringWriter();
		try {
			serializer.setOutput(writer);
			serializer.startDocument("UTF-8", true);
			ProtocolStreamer.streamDevice(serializer, me);
			serializer.endDocument();
			return writer.toString();
		} catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
	boolean write(String path, String txt) {
		try {
			OutputStream os = new FileOutputStream(path);
			OutputStreamWriter osw = new OutputStreamWriter(os);
			osw.write(txt);
			osw.close();
			os.close();
		} catch (FileNotFoundException e) {
			return false;
		} catch (IOException e) {
			return false;
		}
		return true;
	}
	public void downloadRelation(Context ctx){
		detailedRelation = HttpServer.getInstance().readRelation(me.getId());
		detailedRelation.computeCared(me.getId());
		devices.clear();
		for(Relationship r : this.detailedRelation.out){
			String deviceId = r.getToId();
			GeneralDevice device = HttpServer.getInstance().readDefination(deviceId);
			devices.put(deviceId, device);
			r.setDevice(device);
			
		}
		this.saveAccounts(ctx);
	}
	public void uploadRelation(){
		String xml = xmlRelation();
		HttpServer.getInstance().writeRelation(xml);
	}
	public void uploadDefination(){
		String xml = mydefination();
		HttpServer.getInstance().writeDevice( xml);
	}
}
