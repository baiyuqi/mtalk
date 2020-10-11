package com.tc.assistance.protocol;

import android.content.Context;

public interface ProtocolProvider {
	void setProtocolListener(ProtocolListener listener);
	ProtocolSender getProtocolSender();
	void init(Context context);
	void destroy();
}
