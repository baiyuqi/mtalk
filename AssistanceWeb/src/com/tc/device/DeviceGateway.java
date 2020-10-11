package com.tc.device;

import com.tc.assistance.protocol.entity.ProtocolMessage;

public interface DeviceGateway {
	void send(ProtocolMessage msg);
	void addListener(MessageListener l);
}
