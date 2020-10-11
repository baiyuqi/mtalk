package com.tc.device;

import com.tc.assistance.protocol.entity.ProtocolMessage;

public interface MessageListener {
	void onMessage(ProtocolMessage msg);
}
