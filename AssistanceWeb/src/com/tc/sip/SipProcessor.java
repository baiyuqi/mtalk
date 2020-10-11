package com.tc.sip;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;

import javax.sip.Dialog;
import javax.sip.DialogTerminatedEvent;
import javax.sip.IOExceptionEvent;
import javax.sip.RequestEvent;
import javax.sip.ResponseEvent;
import javax.sip.ServerTransaction;
import javax.sip.SipException;
import javax.sip.SipListener;
import javax.sip.TimeoutEvent;
import javax.sip.TransactionTerminatedEvent;
import javax.sip.header.CSeqHeader;
import javax.sip.message.Request;
import javax.sip.message.Response;


public class SipProcessor implements SipListener{
	List<SipMessageListener> listeners = new ArrayList<SipMessageListener>();
	SipService service;
	MessageComposer mp;
	public void addMessageListener(SipMessageListener l){
		listeners.add(l);
	}
	public SipProcessor( SipService ser,MessageComposer mp){

		this.service = ser;
		this.mp = mp;
	}
	public void processDialogTerminated(DialogTerminatedEvent e) {
		System.out.println(e);

		
	}

	public void processIOException(IOExceptionEvent arg0) {
		System.out.println(arg0);
		
	}

	public void processRequest(RequestEvent r) {
		System.out.println(r.getRequest());
	
		
	}

	public void processResponse(ResponseEvent e) {

			System.out.println(e.getResponse());

		

	}

	public void processTimeout(TimeoutEvent arg0) {
		System.out.println(arg0);
		
	}

	public void processTransactionTerminated(TransactionTerminatedEvent arg0) {
//		System.out.println(arg0);
		
	}

	 


}
