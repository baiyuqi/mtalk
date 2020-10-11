package com.tc.sip;

public interface SipMessageListener {
	void onMessage(String from, String msg);
}
