package com.tc.device;

import java.util.ArrayList;
import java.util.List;


import com.tc.assistance.parser.ProtocolParser;
import com.tc.assistance.parser.ProtocolStreamer;
import com.tc.assistance.protocol.entity.ProtocolMessage;
import com.tc.sip.CenterConfiguration;

public class SmsDeviceGateway implements DeviceGateway {
	List<MessageListener> listeners = new ArrayList<MessageListener>();
	@Override
	public void addListener(MessageListener l) {
		listeners.add(l);
	}

	@Override
	public void send(ProtocolMessage msg) {
		String content = ProtocolStreamer.stream(msg);
		System.out.println(content);

	}

	public void onMessage(String from, String msg) {
		ProtocolMessage pm = ProtocolParser.parseProtocolMessage(msg);
		pm.from = from;
		pm.to = CenterConfiguration.name;
		for(MessageListener ml : listeners){
			ml.onMessage(pm);
		}
		
	}
}
