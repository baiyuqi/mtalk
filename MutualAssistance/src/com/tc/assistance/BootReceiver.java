package com.tc.assistance;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;
public class BootReceiver extends BroadcastReceiver
{
    /*Ҫ���յ�intentԴ*/
    static final String ACTION = "android.intent.action.BOOT_COMPLETED";
        
    public void onReceive(Context context, Intent intent) 
    {
        if (intent.getAction().equals(ACTION)) 
        {
        	Intent mi = new Intent(context, 
          		  AssistanceService.class);
        //intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                  context.startService(mi);//��������ʱ����
  //           //Toast.makeText(context, "OlympicsReminder service has started!", Toast.LENGTH_LONG).show();
            //��߿�����ӿ����Զ�������Ӧ�ó������
        }
    }
}
//04-05 03:17:14.811: ERROR/AndroidRuntime(235): Caused by: android.util.AndroidRuntimeException: Calling startActivity() from outside of an Activity  context requires the FLAG_ACTIVITY_NEW_TASK flag. Is this really what you want?

