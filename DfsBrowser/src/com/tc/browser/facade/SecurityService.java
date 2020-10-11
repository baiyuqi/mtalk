package com.tc.browser.facade;

import java.util.List;

public interface SecurityService {
	int access(String user, String path);
	boolean login(String user, String pwd);
	List<String> homes(String user);
}
