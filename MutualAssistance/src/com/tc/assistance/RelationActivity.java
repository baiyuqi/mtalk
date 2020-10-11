package com.tc.assistance;

import java.util.Properties;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.entity.device.Relationship;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;

public class RelationActivity extends Activity {
	EditText deviceId;
	EditText relationName;
	RadioGroup type;
	boolean cared;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        cared =  (Boolean)this.getIntent().getExtras().get("cared");
        setContentView(R.layout.rr);

        deviceId=(EditText)findViewById(R.id.deviceId); 
    	type=(RadioGroup)findViewById(R.id.rg);
    	if(!cared){
    		type.setEnabled(false);
    	}
        relationName=(EditText)findViewById(R.id.relationName); 
        Button confirm=(Button)findViewById(R.id.relationConfirm); 
        Button cancel=(Button)findViewById(R.id.relationCancel); 
        confirm.setOnClickListener(new View.OnClickListener() 
        { 
                public void onClick(View v) 
                {  
                	if(deviceId.getText() == null ||deviceId.getText().toString().trim().equals("") )
                		return;
                	if(relationName.getText() == null ||relationName.getText().toString().trim().equals("") )
                		return;
                	Relationship r = new Relationship();
                	r.cared = cared;
                	if(cared){
                		r.setFromId(AccountManager.getInstance().getMe().getId());
                		r.setToId(deviceId.getText().toString());
                		r.setName(relationName.getText().toString());
                		r.setId(r.getFromId() + "-" + r.getToId());
                		AccountManager.getInstance().getDetailedRelation().out.add(r);
                	}else{
                		r.setToId(AccountManager.getInstance().getMe().getId());
                		r.setFromId(deviceId.getText().toString());
                		r.setReverseName(relationName.getText().toString());
                		r.setId(r.getFromId() + "-" + r.getToId());
                		AccountManager.getInstance().getDetailedRelation().in.add(r);
                	}
                	
                	RelationActivity.this.finish();
                      
                } 
        }); 
        cancel.setOnClickListener(new View.OnClickListener() 
        { 
                public void onClick(View v) 
                { 
                	RelationActivity.this.finish();
                } 
        }); 
    }
}
