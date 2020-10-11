package test;

import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import com.tc.servlet.DeviceService;

public class DeviceServiceTest {
	static public void main(String[] s) throws Exception{
		String service = "http://192.168.1.100:8080/AssistanceWeb/DeviceService?command=defination_write";
	
		HttpClient httpclient = new DefaultHttpClient();  
	    HttpPost httppost = new HttpPost(service);  
	  
 
	        // Add your data  
	       // List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(2);  
	       // nameValuePairs.add(new BasicNameValuePair("command", "defination_write"));  
	     byte[] b = new byte[2048];
	     int l = DeviceService.class.getResourceAsStream("devices.xml").read(b);
	     String ss = new String(b, 0, l);
	     
	       HttpEntity he = new StringEntity(ss, "UTF-8");
	        httppost.setEntity(he);  
	  
	        // Execute HTTP Post Request  
	        HttpResponse response = httpclient.execute(httppost);  
	        
	      
	}
}
