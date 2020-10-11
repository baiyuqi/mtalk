package com.tc.sip;

import java.io.InputStream;
import java.net.InetAddress;
import java.util.Properties;
import java.util.Timer;
import java.util.TimerTask;

import javax.sip.ClientTransaction;
import javax.sip.ListeningPoint;
import javax.sip.ObjectInUseException;
import javax.sip.ResponseEvent;
import javax.sip.SipFactory;
import javax.sip.SipProvider;
import javax.sip.SipStack;
import javax.sip.header.CSeqHeader;
import javax.sip.header.ContactHeader;
import javax.sip.message.Request;
import javax.sip.message.Response;


public class SipService {
	SipStack stack;
	SipProvider provider;
	MessageComposer mp;
	ListeningPoint lp;
	SipProcessor sipProcessor;
	public SipProcessor getSipProcessor() {
		return sipProcessor;
	}
	Timer timer = new Timer();
	public void init() throws Exception{
		Properties p = new Properties();
		InputStream in = SipService.class.getResourceAsStream("/service.properties");
		p.load(in);
		in.close();
		SipFactory fac = SipFactory.getInstance();
		fac.setPathName("gov.nist");
		stack = fac.createSipStack(p);
		lp = stack.createListeningPoint(InetAddress.getLocalHost().getHostAddress(), CenterConfiguration.agentPort, "udp");
		provider = stack.createSipProvider(lp);


		mp = new MessageComposer(fac, provider);
		sipProcessor = new SipProcessor(this, mp);
		provider.addSipListener(sipProcessor);
		stack.start();
		
//		TimerTask tt = new TimerTask(){
//
//			@Override
//			public void run() {
//				register();
//
//				
//			}
//			
//		};
//		timer.schedule(tt,1000, 10000);
//		
	}

	public void message(String to, String content){
		Request r;
		try {

			r = mp.message(to, content);

			provider.sendRequest(r);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	Request previous;
	public void register(){

		try {
			Request r = mp.register();
			previous = r;
			ClientTransaction regTrans = provider.getNewClientTransaction(r);
			regTrans.sendRequest();
		
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}
	public void reflect(ResponseEvent e){
		Response resp = e.getResponse();
		
		if(previous == null)
			return;
		CSeqHeader ch = (CSeqHeader)resp.getHeader(CSeqHeader.NAME);
		long cs = ch.getSeqNumber();
		ch = (CSeqHeader)previous.getHeader(CSeqHeader.NAME);
		long ns = ch.getSeqNumber();
		if(ns != cs)return;
		ContactHeader hh = mp.reflect(resp);
		if(hh == null)return;
		try {
			update(hh);
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
	}
	void update(ContactHeader hh) throws  Exception{
		ContactHeader och = (ContactHeader)previous.getHeader(ContactHeader.NAME);
		och.setWildCard();
		
		//och.setExpires(0);
		Request r = mp.register();
		
		r.removeHeader(ContactHeader.NAME);
		r.addHeader(och);
		r.getExpires().setExpires(0);
		 ClientTransaction t = provider.getNewClientTransaction(r);
		t.sendRequest();
		
		
		r = mp.register();
		r.removeHeader(ContactHeader.NAME);
		r.addHeader(hh);
		
		provider.sendRequest(previous);
		t = provider.getNewClientTransaction(r);
		t.sendRequest();
	}
	public void destroy() {
		stack.stop();
		try {
			stack.deleteSipProvider(provider);
			
		} catch (ObjectInUseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
	}
	
}
