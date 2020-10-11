package com.tc.assistance.parser;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import android.util.Xml;

public class ParserFactory {
	static public XmlSerializer getSerializer(){
		XmlSerializer serializer = Xml.newSerializer();
		return serializer;
	}
}
