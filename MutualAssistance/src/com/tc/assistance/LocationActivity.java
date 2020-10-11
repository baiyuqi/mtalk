package com.tc.assistance;

import android.content.Context;
import android.location.Location;
import android.location.LocationManager;
import android.os.Bundle;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;

public class LocationActivity extends MapActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mapview);
        MapView map = (MapView) findViewById(R.id.mapview);
        MapController mc = map.getController();
        Location l = getLocation();
        if(l == null)
        	return;
        int lat = (int)(l.getLatitude() * 1e6);
        int lont = (int)(l.getLongitude() * 1e6);
        GeoPoint point = new GeoPoint(lat, lont);
		mc.setCenter(point );
		mc.setZoom(15);

    }
    Location getLocation(){
    	Location l  = null;
    	Bundle ex = this.getIntent().getExtras();
    	if(ex != null)
    		l = (Location)ex.get("location");
    	if(l == null){
    		LocationManager locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE); 
    		l = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
    	}
    	return l;
    }

    @Override
    protected boolean isRouteDisplayed() { return true; }
}
/*
C:\Documents and Settings\Administrator\.android>keytool -list -alias androiddeb
ugkey -keystore debug.keystore -storepass android -keypass android
androiddebugkey, 2010-2-12, PrivateKeyEntry,
»œ÷§÷∏Œ∆ (MD5)£∫ E6:6D:4C:C3:F6:79:1E:38:2D:02:C3:11:7B:6C:BE:A1
mapkey:0PcsH_aslv97IZXvhDiNkFUj6ucs6YgAqFkseCg
*/
