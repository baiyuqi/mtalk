

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Map;
public class HttpUploader {
	  public static String post(String actionUrl,  byte[] file, String form, String ct, String fn) {

	      try {          

	    

	          String BOUNDARY = "---------7d4a6d158c9"; //数据分隔线

	          String MULTIPART_FORM_DATA = "multipart/form-data";

	         

	          URL url = new URL(actionUrl);

	          HttpURLConnection conn = (HttpURLConnection) url.openConnection();

	          conn.setConnectTimeout(6* 1000);

	          conn.setDoInput(true);//允许输入

	          conn.setDoOutput(true);//允许输出

	          conn.setUseCaches(false);//不使用Cache

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



	          byte[] end_data = ("--" + BOUNDARY + "--\r\n").getBytes();//数据结束标志       

	          outStream.write(end_data);

	          outStream.flush();

	          int cah = conn.getResponseCode();

	          if (cah != 200) throw new RuntimeException("请求url失败");

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
	  static public void main(String[] s) throws Exception{
		  File f = new File("c:/accounts.xml");
		  FileInputStream in = new FileInputStream(f);
		  byte[] data = new byte[(int)f.length()];
		  in.read(data);
		  HttpUploader.post("http://192.168.1.101:8080/AssistanceWeb", data, "DeviceService", "text/xml", "accounts.xml");
	  }
}

	 
