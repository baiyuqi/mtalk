package com.tc.assistance.client;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import com.tc.assistance.LoginActivity;

public class ServerConfig {
	public String domain;
	public String port;
	public String user;
	public String password;

	public void save(Context ctx) {
		SharedPreferences mPerferences = PreferenceManager
				.getDefaultSharedPreferences(ctx);
		SharedPreferences.Editor mEditor = mPerferences.edit();
		mEditor.putString("domain", domain);
		mEditor.putString("port", port);
		mEditor.putString("user", user);
		mEditor.putString("password", password);
	}

	public void load(Context ctx) {
		SharedPreferences mPerferences = PreferenceManager
				.getDefaultSharedPreferences(ctx);

		user = mPerferences.getString("user", "");
		password = mPerferences.getString("password", "");
		domain = mPerferences.getString("domain", "");
		port = mPerferences.getString("port", "");
	}
	public boolean valid(){
		return !empty(domain) && !empty(port) && !empty(user) && !empty(password);
	}
	boolean empty(String s){
		return s == null | s.trim().equals("");
	}
	
}
