package com.tc.sip;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;

public class AddressTest {
 static public void main(String[] a){
	String r =  "REGISTER sip:nist.gov SIP/2.0\r\n"
	 + "Call-ID: 303aa22a27dfbd34d5dfcbf631f8fb77@192.168.200.4\r\n"
	 + "CSeq: 0 REGISTER\r\n"
	 + "From: <sip:byq@nist.gov>;tag=87d9a47d\r\n"
	 + "To: <sip:byq@nist.gov>\r\n"
	 + "Via: SIP/2.0/UDP 192.168.200.4:5060;rport=\r\n"
	 + "Max-Forwards: 70\r\n"
	 + "Expires: 1500\r\n"
	 + "Contact: \"byq\" <sip:byq@192.168.200.4:5060;transport=udp>;rinstance=;expires=3600\r\n"
	 + "Content-Length: 0\r\n\r\n";
	
	r = "INVITE sip:xyz@nist.gov SIP/2.0\r\n" + 
	"Call-ID: 3fdf7b7f17c8c22faefdd02318888d8c@192.168.200.3\r\n" + 
	"CSeq: 11 INVITE\r\n" + 
	"From: \"byq\" <sip:byq@192.168.200.3:5060;transport=udp>;tag=242eea9d\r\n" + 
	"To: <sip:xyz@nist.gov>\r\n" + 
	"Via: SIP/2.0/UDP 192.168.200.3:5060;rport=\r\n" + 
	"Max-Forwards: 70\r\n" + 
	"Contact: \"byq\" <sip:byq@192.168.200.3:5060;transport=udp>\r\n" + 
	"Content-Type: application/sdp\r\n" + 
	"Content-Length: 257\r\n" + 
	"\r\n" +
	"v=0\r\n" + 
	"o=center 0 0 IN IP4 192.168.200.3\r\n" + 
	"s=-\r\n" + 
	"c=IN IP4 192.168.200.3\r\n" + 
	"t=0 0\r\n" + 
	"m=audio 10000 RTP/AVP 0 8 97 3 5 4\r\n" + 
	"a=rtpmap:4 G723/8000\r\n" + 
	"a=fmtp:4 annexa=no;bitrate=6.3\r\n" + 
	"m=video 10008 RTP/AVP 99 34 26 31\r\n" + 
	"a=rtpmap:99 H264/90000\r\n" + 
	"a=fmtp:99 packetization-mode=1\r\n";
	
	r = "MESSAGE sip:xyz@nist.gov SIP/2.0\r\n" + 
	"Call-ID: bcc8f87ac587bcdf28b9035c94f2b3ec@192.168.200.3\r\n" + 
	"CSeq: 2140 MESSAGE\r\n" + 
	"From: \"byq0\" <sip:byq0@192.168.200.3:20008;transport=udp>;tag=7d023559\r\n" + 
	"To: <sip:xyz@nist.gov>\r\n" + 
	"Via: SIP/2.0/UDP 192.168.200.3:20008;rport=\r\n" + 
	"Max-Forwards: 70\r\n" + 
	"Content-Type: application/ddd\r\n" + 
	"Content-Length:5\r\n" + 
	"\r\n" + 
	"sgvsG\r\n";

	try {
		DatagramSocket s = new DatagramSocket(new InetSocketAddress(InetAddress.getLocalHost().getHostAddress(), 30000));
		//s.bind();
		s.connect(new InetSocketAddress(InetAddress.getByName("192.168.100.25"), 5060));
		byte[] buf = r.getBytes();
		
		DatagramPacket p = new DatagramPacket(buf, buf.length);
		s.send(p);
		s.receive(p);
		String ss = new String(p.getData(),p.getOffset(), p.getLength());
		System.out.println(ss);
	} catch (Exception e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}

 }
}
