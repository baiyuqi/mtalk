package com.tc.assistance.parser;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

public class ParserFactory {
	static public XmlSerializer getSerializer(){
		XmlSerializer serializer = null;
		try {
			serializer = XmlPullParserFactory.newInstance().newSerializer();
		} catch (XmlPullParserException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		return serializer;
	}
}
