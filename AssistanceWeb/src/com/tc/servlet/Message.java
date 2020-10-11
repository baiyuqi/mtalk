package com.tc.servlet;

import java.io.IOException;

import javax.ejb.EJB;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.tc.assistance.business.DeviceFacadeLocal;
import com.tc.env.EnvironmentServlet;
import com.tc.sip.CenterConfiguration;
import com.tc.sip.SipService;
import com.tc.sip.message.RtspCommand;
import com.tc.sip.message.SipMessage;

public class Message extends EnvironmentServlet {
	private static final long serialVersionUID = 1L;
	

    public Message() {
        super();

    }

	@Override
	public void init() throws ServletException {
		// TODO Auto-generated method stub
		super.init();


	}

	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		SipMessage m = extract(request);
		String to = "SIP:" + m.clientId + "@" + CenterConfiguration.domain;
		env.getSipService().message(to, m.content());
	}
	SipMessage extract(HttpServletRequest request){
		RtspCommand m = new RtspCommand();
		m.clientId = request.getParameter("clientId");
		m.command = request.getParameter("command");
		m.ip = request.getParameter("ip");
		m.port = Integer.parseInt(request.getParameter("port"));
		return m;
		
	}
	

}
