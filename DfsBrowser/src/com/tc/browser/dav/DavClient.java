package com.tc.browser.dav;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpURL;
import org.apache.webdav.lib.WebdavResource;

public class DavClient {
	WebdavResource wdr;
	public DavClient(String path, String user, String pwd) throws Exception{
		HttpURL hrl = new HttpURL(path);
		hrl.setUserinfo(user, pwd);
		wdr = new WebdavResource(hrl);

	}
	String[] list(){
		return wdr.list();
	}
	public void to(String d){
		d = wdr.getPath()  + d + "/";
		
		try {
			wdr.setPath(d) ;
		} catch (HttpException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public void makdir(String d){
		try {
			d = wdr.getPath()  + d + "/";
			
			wdr.mkcolMethod(d) ;
		} catch (HttpException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	public boolean upload(String localPath, String localFileName) {
		boolean bool = false;
		try {

			System.out.println("测试Web路径:" + wdr.getPath());

			File file = new File(localPath + "/" + localFileName); // 指定上传本地某个目录下的文件
			String path = wdr.getPath();
			if (!path.endsWith("/"))
				path += "/";
			path += localFileName; // 上传到WebDAV后可按另外一个文件名保存
			System.out.println("\nFilePath is:" + path + "\n");

			try // 尝试锁定
			{
				wdr.setPath(path);

				if (wdr.isLocked()) {

					// bool = false;
					// System.out.println("文件已被锁定，上传失败!");
					// return bool;
					wdr.unlockMethod();
					bool = wdr.putMethod(path, file);

				} else {
					// wdr.lockMethod(uid,10000000);
					bool = wdr.putMethod(path, file);
					// wdr.unlockMethod();
				}

			} catch (Exception ex) {
				bool = wdr.putMethod(path, file);
				// wdr.unlockMethod();
				System.out.println("\n文件不存在，不需锁定...\n");
			} finally {

			}
		} catch (MalformedURLException mue) {

			System.out.println("MalformedURLException:" + mue.getMessage());
		} catch (HttpException he) {

			System.out.println("HttpException:" + he.getMessage());

		} catch (IOException ioe) {

			System.out.println("IOException:" + ioe.getMessage());

		} catch (Exception ex) {

			System.out.println("ThrowException:" + ex.getMessage());
		} finally {
			// wdr.close();
		}

		return bool;

	}

	public static void main(String args[]) {

		try {
			DavClient c = new DavClient(
					"http://192.168.1.101:8080/logicaldoc/webdav/store/",
					"admin", "admin");
			c.to("me");
//			String s[] = c.list();
			c.upload("c:\\", "localfile.txt");
//			c.makdir("me");
		} catch (Exception ex) {
		}
	}

}
