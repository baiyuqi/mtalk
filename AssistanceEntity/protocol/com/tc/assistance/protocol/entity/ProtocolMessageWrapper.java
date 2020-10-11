package com.tc.assistance.protocol.entity;

public class ProtocolMessageWrapper<T extends ProtocolMessage> {
	public ProtocolMessageWrapper(T message) {
		super();
		this.protocolMessage = message;
	}

	protected T protocolMessage;

	public T getProtocolMessage() {
		return protocolMessage;
	}
}
