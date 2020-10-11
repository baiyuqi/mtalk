package com.tc.assistance.protocol.sms;

import java.io.DataOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;

import org.apache.http.client.methods.HttpPost;

public class HttpUploader {
	  public static String post(String actionUrl,  byte[] file, String form, String ct, String fn) {

	      try {          
	    	  HttpPost p;
	    

	          String BOUNDARY = "---------7d4a6d158c9"; //���ݷָ���

	          String MULTIPART_FORM_DATA = "multipart/form-data";

	         

	          URL url = new URL(actionUrl);

	          HttpURLConnection conn = (HttpURLConnection) url.openConnection();

	          conn.setConnectTimeout(6* 1000);

	          conn.setDoInput(true);//��������

	          conn.setDoOutput(true);//�������

	          conn.setUseCaches(false);//��ʹ��Cache

	          conn.setRequestMethod("POST");          

	          conn.setRequestProperty("Connection", "Keep-Alive");

	          conn.setRequestProperty("Charset", "UTF-8");

	          conn.setRequestProperty("Content-Type", MULTIPART_FORM_DATA + "; boundary=" + BOUNDARY);

	 

	        

	          DataOutputStream outStream = new DataOutputStream(conn.getOutputStream());

	          StringBuilder split = new StringBuilder();

	              split.append("--");

	              split.append(BOUNDARY);

	              split.append("\r\n");

	              split.append("Content-Disposition: form-data;name=\""+ form+"\";filename=\""+ fn + "\"\r\n");

	              split.append("Content-Type: "+ct+"\r\n\r\n");

	              outStream.write(split.toString().getBytes());

	              outStream.write(file, 0, file.length);

	              outStream.write("\r\n".getBytes());



	          byte[] end_data = ("--" + BOUNDARY + "--\r\n").getBytes();//���ݽ�����־       

	          outStream.write(end_data);

	          outStream.flush();

	          int cah = conn.getResponseCode();

	          if (cah != 200) throw new RuntimeException("����urlʧ��");

	          InputStream is = conn.getInputStream();

	          int ch;

	          StringBuilder b = new StringBuilder();

	          while( (ch = is.read()) != -1 ){

	          b.append((char)ch);

	          }

	        

	          outStream.close();

	          conn.disconnect();

	          return b.toString();

	      } catch (Exception e) {

	       throw new RuntimeException(e);

	      }

	  }
}

	 
