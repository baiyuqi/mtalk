package com.tc.assistance.protocol;

import com.tc.assistance.protocol.entity.Alarm;
import com.tc.assistance.protocol.entity.ProtocolMessageWrapper;
import com.tc.assistance.protocol.entity.Request;
import com.tc.assistance.protocol.entity.Report;

public interface ProtocolSender {
	void command(ProtocolMessageWrapper<Request> command);
	void report(ProtocolMessageWrapper<Report> report);
	void send(ProtocolMessageWrapper entity);
	void alarm(Alarm alarm);
}
