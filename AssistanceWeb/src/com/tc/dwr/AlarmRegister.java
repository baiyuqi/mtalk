package com.tc.dwr;

import java.util.HashMap;
import java.util.Map;

import org.directwebremoting.ScriptBuffer;
import org.directwebremoting.ScriptSession;

import com.tc.assistance.entity.business.DeviceUser;
import com.tc.clientmessage.*;

import uk.ltd.getahead.dwr.WebContext;
import uk.ltd.getahead.dwr.WebContextFactory;

public class AlarmRegister { 
    public AlarmRegister() { 
    } 
    Map<String, ScriptSession> sessions = new HashMap<String, ScriptSession>();

    public void register(){ 
        WebContext wctx = WebContextFactory.get(); 
        DeviceUser user = (DeviceUser)wctx.getSession().getAttribute("user");
        if(user == null)
        	return;
        final ScriptSession scriptSession = wctx.getScriptSession(); 
        sessions.put(user.getName(),scriptSession );
    } 
    public void unregister(){ 
        WebContext wctx = WebContextFactory.get(); 
        DeviceUser user = (DeviceUser)wctx.getSession().getAttribute("user");
        if(user == null)
        	return;
        ScriptSession scriptSession = sessions.remove(user.getName());
        ScriptBuffer scriptBuffer = new ScriptBuffer(); 
        scriptBuffer.appendScript("showAlarm(") 
        .appendData("³É¹¦ÍË³ö") 
        .appendScript(");"); 

        scriptSession .addScript(scriptBuffer); 
    } 
    public void message(ClientMassage msg){
    	if(msg instanceof DeviceStatusUpdate){
    		DeviceStatusUpdate dm = (DeviceStatusUpdate)msg;
    		ScriptSession scriptSession = sessions.get(dm.getDevice().getOwnerId());
    		if(scriptSession== null)return;
    		ScriptBuffer scriptBuffer = new ScriptBuffer(); 

            scriptBuffer.appendScript("showAlarm(") 
                    .appendData(dm.getDevice().toString()) 
                    .appendScript(");"); 
            
			scriptSession .addScript(scriptBuffer); 
			
    	}
    	
    }
} 