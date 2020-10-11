package com.tc.dwr;

import org.directwebremoting.ScriptBuffer;
import org.directwebremoting.ScriptSession;

public class AlarmEventProcessor { 
    private ScriptSession scriptSession; 

    public AlarmEventProcessor(ScriptSession scriptSession) { 
        this.scriptSession=scriptSession; 
    } 



    public void processEventMsg(String from, Object message) { 
        ScriptBuffer scriptBuffer = new ScriptBuffer(); 

        scriptBuffer.appendScript("showAlarm(") 
                .appendData("123") 
                .appendScript(");"); 
        scriptSession.addScript(scriptBuffer); 
    
    } 
  } 