package com.tc.assistance.client;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import com.tc.assistance.AccountManager;
import com.tc.assistance.entity.device.DetailedRelation;
import com.tc.assistance.entity.device.GeneralDevice;
import com.tc.assistance.parser.ProtocolParser;

public class HttpServer {
	static HttpServer instance;
	static {
		instance = new HttpServer();
	}

	static public HttpServer getInstance() {
		return instance;
	}
	ServerConfig serverConfig = new ServerConfig();

	String service = "http://192.168.1.103:8080/AssistanceWeb/DeviceService";

	public Map<String, String> readStatus(String deviceId) {
		List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(4);
		nameValuePairs.add(new BasicNameValuePair("command", "status_read"));
		nameValuePairs.add(new BasicNameValuePair("deviceId", deviceId));
		nameValuePairs.add(new BasicNameValuePair("username", serverConfig.user));
		nameValuePairs.add(new BasicNameValuePair("password", serverConfig.password));
		return ProtocolParser
		.parseDeviceParams(read(service, nameValuePairs));
	}
	public DetailedRelation readRelation(String deviceId) {
		List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(5);
		nameValuePairs
				.add(new BasicNameValuePair("command", "relation_read"));
		nameValuePairs.add(new BasicNameValuePair("deviceId", deviceId));
		nameValuePairs.add(new BasicNameValuePair("detailed", "true"));
		nameValuePairs.add(new BasicNameValuePair("username", serverConfig.user));
		nameValuePairs.add(new BasicNameValuePair("password", serverConfig.password));
		return ProtocolParser
				.parseDeviceRelation(read(service, nameValuePairs));

	}
	public GeneralDevice readDefination(String deviceId) {
		List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(4);
		nameValuePairs
				.add(new BasicNameValuePair("command", "defination_read"));
		nameValuePairs.add(new BasicNameValuePair("deviceId", deviceId));
		nameValuePairs.add(new BasicNameValuePair("username", serverConfig.user));
		nameValuePairs.add(new BasicNameValuePair("password", serverConfig.password));
		return ProtocolParser
				.parseDeviceDefination(read(service, nameValuePairs));

	}

	InputStream read(String service, List<NameValuePair> pas) {
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(service);

		try {

			UrlEncodedFormEntity he = new UrlEncodedFormEntity(pas);
			httppost.setEntity(he);

			// Execute HTTP Post Request
			HttpResponse response = httpclient.execute(httppost);
			HttpEntity e = response.getEntity();

			return e.getContent();

		} catch (ClientProtocolException e) {
			// TODO Auto-generated catch block
		} catch (IOException e) {
			e.printStackTrace();
			// TODO Auto-generated catch block
		}
		return null;

	}
	public void writeRelation(String xml){
		write(service, xml, "relation_write");
	}
	public void writeDevice(String xml){
		write(service, xml, "defination_write");
	}
	void write(String service, String xml, String command) {
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(service + "?deviceId=" + AccountManager.getInstance().getMe().getId() + "&command=" + command + "&username=" +  serverConfig.user + "&password=" +  serverConfig.password);

		try {

			 HttpEntity he = new StringEntity(xml, "UTF-8");
		     httppost.setEntity(he);  
			httppost.setEntity(he);

			// Execute HTTP Post Request
			 httpclient.execute(httppost);
			

		} catch (ClientProtocolException e) {
			// TODO Auto-generated catch block
		} catch (IOException e) {
			e.printStackTrace();
			// TODO Auto-generated catch block
		}


	}
	public ServerConfig getServerConfig() {
		return serverConfig;
	}
	
}
