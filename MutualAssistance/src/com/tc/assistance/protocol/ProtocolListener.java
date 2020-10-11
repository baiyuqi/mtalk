package com.tc.assistance.protocol;

import com.tc.assistance.protocol.entity.Alarm;
import com.tc.assistance.protocol.entity.ProtocolMessageWrapper;
import com.tc.assistance.protocol.entity.Request;
import com.tc.assistance.protocol.entity.Report;

public interface ProtocolListener {
	void onReport(ProtocolMessageWrapper<Report> report);
	void onCommand(ProtocolMessageWrapper<Request> command);
	void onAlarm(Alarm alarm);
	void onReceive(ProtocolMessageWrapper entity);
}
