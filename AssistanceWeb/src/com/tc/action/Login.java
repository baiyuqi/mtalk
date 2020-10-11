/*
 * $Id: Login.java,v 1.8 2010/05/04 02:07:25 byq Exp $
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package com.tc.action;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.struts2.interceptor.SessionAware;

import com.tc.assistance.entity.business.Person;
import com.tc.assistance.entity.business.DeviceUser;


public class Login extends EnvironmentSupport implements SessionAware{
	
	
    public String execute() throws Exception {

        if (isInvalid(getUsername())) return INPUT;

        if (isInvalid(getPassword())) return INPUT;
        
        DeviceUser user = (DeviceUser)this.env.getPersistenceFacade().unique("from DeviceUser where name = '" + username + "' and password='" + password + "'");
        if(user != null){
        	session.put("user", user);
           	
        	
        return SUCCESS;
        }
        return INPUT;
    }
	
	private boolean isInvalid(String value) {
        return (value == null || value.length() == 0);
    }

    private String username;

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    private String password;

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
    Map<String, Object> session;
	@Override
	public void setSession(Map<String, Object> s) {
		session = s;
		
	}

	
}