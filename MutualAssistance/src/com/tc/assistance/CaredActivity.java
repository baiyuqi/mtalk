package com.tc.assistance;

import java.util.Map;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.entity.device.Relationship;


public class CaredActivity extends Activity {
	Relationship account;
	Map<String, String> deviceParameters;
	
	 @Override
	    public void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);
	        setContentView(R.layout.cared);
	        account = (Relationship)this.getIntent().getExtras().get("entity");
	        this.setTitle(account.getName());
	        deviceParameters = HttpServer.getInstance().readStatus(account.getToId());
	        render();
	        Button btn=(Button)findViewById(R.id.locating); 
	        btn.setOnClickListener(new View.OnClickListener() 
	        { 
	                public void onClick(View v) 
	                { 
	                	Intent intent = new Intent();
	                	intent.setClass(CaredActivity.this, LocationActivity.class);
	                	CaredActivity.this.startActivity(intent);
	                      
	                } 
	        }); 
	        Button me=(Button)findViewById(R.id.measuring); 
	        me.setOnClickListener(new View.OnClickListener() 
	        { 
	                public void onClick(View v) 
	                { 
	                	
	                	CaredActivity.this.deviceParameters = HttpServer.getInstance().readStatus(account.getToId());
	                	CaredActivity.this.render();
	                } 
	        }); 
	     
	    }
	 void render(){
		 String dtemp = this.deviceParameters.get("temperature");
		 String dpre = this.deviceParameters.get("pressure");
		 String dpul = this.deviceParameters.get("pulse");
		 TextView temp=(TextView)findViewById(R.id.temperature); 
		 temp.setText(dtemp == null?"":dtemp);
		 TextView pres=(TextView)findViewById(R.id.pressure); 
		 pres.setText(dpre == null? "":dpre);
		 TextView pul=(TextView)findViewById(R.id.pulse); 
		 pul.setText(dpul == null?"":dpul);
	 }
	 
}
