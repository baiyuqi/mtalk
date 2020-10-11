package com.tc.browser.facade.impl;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.tc.browser.facade.Document;
import com.tc.browser.facade.DocumentService;
import com.tc.browser.facade.SecurityService;

public class SecurityServiceImpl implements SecurityService {

	@Override
	public int access(String user, String path) {
		return 0;
	}

	@Override
	public List<String> homes(String user) {
		List<String> rst = new LinkedList<String>();
		rst.add("c:\\windows");
		rst.add("d:\\soft");
		return rst;
	}

	@Override
	public boolean login(String user, String pwd) {
		if(user != null && user.equals("admin")&& pwd != null && pwd.equals("admin"))
			return true;
		return false;
	}


}
