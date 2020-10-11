package com.tc.assistance;

import java.io.IOException;
import java.io.Serializable;
import java.util.Properties;

import org.apache.http.client.methods.HttpPost;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.client.ServerConfig;
import com.tc.assistance.entity.device.Relationship;
import com.tc.assistance.protocol.ProtocolListener;
import com.tc.assistance.protocol.ProtocolProvider;
import com.tc.assistance.protocol.entity.Alarm;
import com.tc.assistance.protocol.entity.ProtocolMessageWrapper;
import com.tc.assistance.protocol.entity.Request;
import com.tc.assistance.protocol.entity.PictureReport;
import com.tc.assistance.protocol.entity.Report;
import com.tc.assistance.protocol.sms.SmsProtocolProvider;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.IBinder;
import android.telephony.SmsMessage;

public class AssistanceService extends Service implements ProtocolListener{
	AccountManager accountManager;
	 private NotificationManager mNM;
	ProtocolProvider provider;
	LocationManager locationManager;
	Location location;
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		provider.destroy();

	}


	@Override
	public void onCreate() {
	    mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
		super.onCreate();
		 android.os.Debug.waitForDebugger();

		this.accountManager = AccountManager.getInstance(this);
		// 注册短信变化监听
		// this.getContentResolver().registerContentObserver(Uri.parse("content://sms/"),
		// true, content);
		showNotification();
		provider = new SmsProtocolProvider();
		provider.init(this);
		provider.setProtocolListener(this);
		
		locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE); 
		
		LocationListener locationListener = new LocationListener() {   
		    public void onLocationChanged(Location location) { //当坐标改变时触发此函数，如果Provider传进相同的坐标，它就不会被触发   
		    	AssistanceService.this.location = location;
		    }   
		  
		    public void onProviderDisabled(String provider) {   
		    // Provider被disable时触发此函数，比如GPS被关闭   
		    }   
		  
		    public void onProviderEnabled(String provider) {   
		    //  Provider被enable时触发此函数，比如GPS被打开   
		    }   
		  
		    public void onStatusChanged(String provider, int status, Bundle extras) {   
		    // Provider的转态在可用、暂时不可用和无服务三个状态直接切换时触发此函数   
		    }   
		};  

		
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,   
				 1000, 0, locationListener); 
		
	}



	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	 private void showNotification() {


	        // Set the icon, scrolling text and timestamp
	        Notification notification = new Notification(R.drawable.icon, "互助精灵",
	                System.currentTimeMillis());

	        // The PendingIntent to launch our activity if the user selects this notification
	        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
	                new Intent(this, MainActivity.class), 0);

	        // Set the info for the views that show in the notification panel.
	        notification.setLatestEventInfo(this, "互助精灵",
	        		"互助精灵", contentIntent);

	        // Send the notification.
	        // We use a layout id because it is a unique number.  We use it later to cancel.
	        mNM.notify(R.string.servicestarted, notification);
	    }

	@Override
	public void onCommand(final ProtocolMessageWrapper<Request> command) {
		
		if(command.getProtocolMessage().action.equals(Request.COMMAND_PIC)){
			PictureCallback mPictureCallback = new PictureCallback() {   
				  
		        public void onPictureTaken(byte[] data, Camera camera) {   
		            // String s = command.getCommand();
					if (data != null) {
						Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
						PictureReport report = new PictureReport(new Report());
						HttpPost post = new HttpPost("http://192.168.1.101/AssistanceWeb/pic/");
						
						report.getProtocolMessage().from = command.getProtocolMessage().from;
						provider.getProtocolSender().report(report);
					}
				}

		  
		};  
		Camera cam = Camera.open();
		cam.takePicture(null, null, mPictureCallback);   
		
		}
		if(command.getProtocolMessage().action.equals(Request.COMMAND_LOCATION)){
			Location location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);   
			double latitude = location.getLatitude();     //经度   
			double longitude = location.getLongitude(); //纬度   
			double altitude =  location.getAltitude();     //海拔  

		}
		
	}

	@Override
	public void onReceive(ProtocolMessageWrapper entity) {

		
	}

	@Override
	public void onReport(ProtocolMessageWrapper<Report> report) {
		
		
	}


	@Override
	public void onAlarm(Alarm alarm) {
		String addr =alarm.getProtocolMessage().from;
		Relationship ca = this.accountManager.getAccounts().getCared(addr);
		if(ca == null)return;
		Intent intent = new Intent();
        intent.putExtra("entity", ca);
        intent.setClass(this, CaredActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        Notification notification = new Notification(R.drawable.alarm, ca.getName() + "有情况",
                System.currentTimeMillis());
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                intent, 0);
        notification.setLatestEventInfo(this, "报警",
        		ca.getName() + "有情况", contentIntent);
        mNM.notify(ca.hashCode(), notification);
		
		
		
		
	}
}
