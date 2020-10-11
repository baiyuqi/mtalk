package com.tc.browser.facade;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;

import com.tc.browser.facade.impl.FileSystemFileFacade;
import com.tc.browser.facade.impl.SecurityServiceImpl;

public class Client {
	String user;
	SecurityService security = new SecurityServiceImpl();
	List<String> homes;
	FileSystem fs = null;
	FileFacade fileFacade;
	public FileFacade getFileFacade() {
		return fileFacade;
	}
	public Client(){
		Configuration conf = new Configuration();
		
		try {
			fs = FileSystem.getLocal(conf);
		} catch (IOException e) {
			e.printStackTrace();
		}
		fileFacade = new FileSystemFileFacade(fs);
	}
	public boolean login(String u, String pwd){
		if(security.login(u, pwd)){
			user = u;
			return true;
		}
		return false;
	}
	public List<String> homes(){
		
		//if(user == null)
		//	throw new RuntimeException("»¹Ã»ÓÐµÇÂ¼£¡");
		if(homes == null){
			homes = security.homes(user);
		}
		return homes;
	}
	static Client instance;
	static{
		instance = new Client();
	}
	static public Client getInstance(){
		return instance;
		
	}
	public boolean isloggedin() {
		
		return user != null;
	}
	
	
}
