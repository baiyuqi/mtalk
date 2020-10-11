package com.tc.assistance.protocol.sms;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.tc.assistance.parser.ProtocolParser;
import com.tc.assistance.protocol.ProtocolListener;
import com.tc.assistance.protocol.ProtocolProvider;
import com.tc.assistance.protocol.ProtocolSender;
import com.tc.assistance.protocol.entity.Alarm;
import com.tc.assistance.protocol.entity.ProtocolMessageWrapper;
import com.tc.assistance.protocol.entity.Request;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.assistance.protocol.entity.Report;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.telephony.SmsMessage;

public class SmsProtocolProvider implements ProtocolProvider{
	Context context;
	ProtocolListener listener;
	
	@Override
	public ProtocolSender getProtocolSender() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setProtocolListener(ProtocolListener l) {
		this.listener = l;
		
	}

	@Override
	public void destroy() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void init(Context ctx) {
		this.context = ctx;
		context.registerReceiver(mReceiver, new IntentFilter(
		"android.provider.Telephony.SMS_RECEIVED"));
		
	}
	BroadcastReceiver mReceiver = new BroadcastReceiver() {

		public void onReceive(Context context, Intent intent) {
			if (!intent.getAction().equals(
					"android.provider.Telephony.SMS_RECEIVED"))
				return;
			StringBuilder sb = new StringBuilder();
			Bundle bundle = intent.getExtras();
			if (bundle == null)
				return;
			Object[] pdus = (Object[]) bundle.get("pdus");
			SmsMessage[] smsMsg = new SmsMessage[pdus.length];
			for (int i = 0; i < pdus.length; i++) {
				smsMsg[i] = SmsMessage.createFromPdu((byte[]) pdus[i]);
				String msg = smsMsg[i].getMessageBody();
				ProtocolMessageWrapper o = ProtocolParser.parseAndWrap(msg);
				if (o != null) {
					o.getProtocolMessage().from = smsMsg[i].getOriginatingAddress();
					process(o);
					delete(smsMsg[i]);
	
				}
			}
		}

		
		

	};
	

	private void delete(SmsMessage msg) {
		//msg.

	}

	private void process(ProtocolMessageWrapper o) {
		if(o.getProtocolMessage() instanceof Report)
			listener.onReport(o);
		if(o.getProtocolMessage() instanceof Request)
			listener.onCommand(o);
		if(o instanceof Alarm)
			listener.onAlarm((Alarm) o);
	}

}
