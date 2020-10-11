/*
 * $Id: EnvironmentSupport.java,v 1.2 2010/05/04 02:07:25 byq Exp $
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

import java.util.Map;

import org.apache.struts2.interceptor.ApplicationAware;

import com.opensymphony.xwork2.ActionSupport;
import com.tc.env.Environment;

/**
 * Base Action class for the Tutorial package.
 */
public class EnvironmentSupport extends ActionSupport implements ApplicationAware{
	protected Environment env;

	@Override
	public void setApplication(Map<String, Object> arg0) {
		env = (Environment)arg0.get("environment");
		
	}

}
