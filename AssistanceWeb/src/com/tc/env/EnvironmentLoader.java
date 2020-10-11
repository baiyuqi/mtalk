package com.tc.env;

import java.util.Enumeration;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;

import com.tc.dwr.AlarmRegister;

/**
 * Servlet implementation class EnvironmentLoader
 */
public class EnvironmentLoader extends HttpServlet {
	private static final long serialVersionUID = 1L;

	/**
	 * @see Servlet#init(ServletConfig)
	 */
	public void init(ServletConfig config) throws ServletException {
		ServletContext ctx = config.getServletContext(); 
		AlarmRegister r = (AlarmRegister)ctx.getAttribute("AlarmRegister");
		ctx.setAttribute("environment", new Environment(r));
	}

	@Override
	public void destroy() {
		// TODO Auto-generated method stub
		super.destroy();
		Environment env = (Environment)this.getServletContext().getAttribute("environment");
		env.destroy();
	}

}
