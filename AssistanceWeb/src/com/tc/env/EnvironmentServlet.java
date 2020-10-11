package com.tc.env;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;


public class EnvironmentServlet extends HttpServlet {
	protected Environment env;
	@Override
	public void init() throws ServletException {
		super.init();
		env = (Environment)this.getServletContext().getAttribute("environment");
	}

}
