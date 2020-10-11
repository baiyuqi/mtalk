package com.tc.assistance;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.tc.assistance.client.HttpServer;
import com.tc.assistance.client.ServerConfig;

public class LoginActivity extends Activity {
    /** Called when the activity is first created. */
	EditText domain;
	EditText port;
        EditText user;
        EditText password;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
       // requestWindowFeature(Window.FEATURE_LEFT_ICON);
        setContentView(R.layout.login);
      //  getWindow().setFeatureDrawableResource(Window.FEATURE_LEFT_ICON, 
      //          android.R.drawable.ic_dialog_alert);
        domain=(EditText)LoginActivity.this.findViewById(R.id.domain); 
    	port=(EditText)LoginActivity.this.findViewById(R.id.port); 
        user=(EditText)LoginActivity.this.findViewById(R.id.name); 
        password=(EditText)LoginActivity.this.findViewById(R.id.password); 
        Button btn=(Button)findViewById(R.id.login); 
        
        ServerConfig sc = HttpServer.getInstance().getServerConfig();
		domain.setText(sc.domain);
		port.setText(sc.port);
		user.setText(sc.user);
		password.setText(sc.password);
		btn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {

				ServerConfig sc = HttpServer.getInstance().getServerConfig();
				sc.domain = domain.getText().toString();
				sc.port = port.getText().toString();
				sc.user = user.getText().toString();
				sc.password = password.getText().toString();
				sc.save(LoginActivity.this);
				LoginActivity.this.finish();

			}
		});

        Button et=(Button)findViewById(R.id.exit); 
        et.setOnClickListener(new View.OnClickListener() 
        { 
                public void onClick(View v) 
                { 
                	LoginActivity.this.finish();
                } 
        }); 
    }
}