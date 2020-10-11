package com.tc.sip;

import gov.nist.javax.sip.header.extensions.ReplacesHeader;

import java.net.InetAddress;
import java.nio.charset.Charset;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;

import javax.sip.Dialog;
import javax.sip.PeerUnavailableException;
import javax.sip.SipFactory;
import javax.sip.SipProvider;
import javax.sip.address.Address;
import javax.sip.address.AddressFactory;
import javax.sip.address.SipURI;
import javax.sip.header.CSeqHeader;
import javax.sip.header.CallIdHeader;
import javax.sip.header.ContactHeader;
import javax.sip.header.ContentLengthHeader;
import javax.sip.header.ContentTypeHeader;
import javax.sip.header.ExpiresHeader;
import javax.sip.header.FromHeader;
import javax.sip.header.Header;
import javax.sip.header.HeaderFactory;
import javax.sip.header.MaxForwardsHeader;
import javax.sip.header.ToHeader;
import javax.sip.header.ViaHeader;
import javax.sip.message.MessageFactory;
import javax.sip.message.Request;
import javax.sip.message.Response;

public class MessageComposer {
	public MessageComposer(SipFactory fac, SipProvider p)
			throws PeerUnavailableException {
		this.addressFactory = fac.createAddressFactory();
		this.headerFactory = fac.createHeaderFactory();
		this.messageFactory = fac.createMessageFactory();
		this.provider = p;

	}

	Random localTagGenerator = new Random();

	AddressFactory addressFactory;

	HeaderFactory headerFactory;

	MessageFactory messageFactory;

	SipProvider provider;

	int[] sequence = new int[] { 1, 1, 1 };

	// SessionDescriptionCreator sdpc;
	private Header stripReplacesHeader(Address address) throws Exception {
		javax.sip.address.URI uri = address.getURI();
		Header replacesHeader = null;

		if (uri.isSipURI()) {
			SipURI sipURI = (SipURI) uri;
			String replacesHeaderValue = sipURI.getHeader(ReplacesHeader.NAME);

			if (replacesHeaderValue != null) {
				for (Iterator<?> headerNameIter = sipURI.getHeaderNames(); headerNameIter
						.hasNext();) {
					if (ReplacesHeader.NAME.equals(headerNameIter.next())) {
						headerNameIter.remove();
						break;
					}
				}

				replacesHeader = headerFactory.createHeader(
						ReplacesHeader.NAME, replacesHeaderValue);

			}
		}
		return replacesHeader;
	}

	Address getOurSipAddress() throws Exception {

		InetAddress localHost = InetAddress.getLocalHost();

		SipURI ourSipURI = addressFactory.createSipURI(
				CenterConfiguration.name, localHost.getHostAddress());

		ourSipURI.setTransportParam("udp");
		ourSipURI.setPort(CenterConfiguration.agentPort);

		Address ourSipAddress = addressFactory.createAddress(
				CenterConfiguration.name, ourSipURI);

		ourSipAddress.setDisplayName(CenterConfiguration.name);

		return ourSipAddress;

	}

	ArrayList<ViaHeader> getLocalViaHeaders() throws Exception {
		ArrayList<ViaHeader> viaHeaders = new ArrayList<ViaHeader>();
		ViaHeader viaHeader = headerFactory.createViaHeader(InetAddress
				.getLocalHost().getHostAddress(),
				CenterConfiguration.agentPort, "udp", null);
		viaHeader.setParameter("rport", "");
		viaHeaders.add(viaHeader);

		return viaHeaders;

	}

	ContactHeader getContactHeader(InetAddress address, int port)
			throws Exception {
		ContactHeader registrationContactHeader = null;

		SipURI contactURI = addressFactory.createSipURI(
				CenterConfiguration.name, address.getHostAddress());

		String transport = "udp";
		contactURI.setTransportParam(transport);

		contactURI.setPort(port);

		Address contactAddress = addressFactory.createAddress(contactURI);

		contactAddress.setDisplayName(CenterConfiguration.name);

		registrationContactHeader = headerFactory
				.createContactHeader(contactAddress);

		return registrationContactHeader;
	}

	public Request message(String to, String message) throws Exception {
		Address toAddress = addressFactory.createAddress(to);
		CallIdHeader callIdHeader = provider.getNewCallId();
		CSeqHeader cSeqHeader = headerFactory.createCSeqHeader(sequence[1]++,
				Request.MESSAGE);
		String localTag = Integer.toHexString(localTagGenerator.nextInt());
		FromHeader fromHeader = headerFactory.createFromHeader(
				getOurSipAddress(), localTag);
		ToHeader toHeader = headerFactory.createToHeader(toAddress, null);
		ArrayList<ViaHeader> viaHeaders = getLocalViaHeaders();
		MaxForwardsHeader maxForwards = headerFactory
				.createMaxForwardsHeader(70);

		ContentTypeHeader contTypeHeader = headerFactory
				.createContentTypeHeader("text", "plain");

		String enc = Charset.defaultCharset().name();


		Request req = messageFactory.createRequest(toHeader.getAddress()
				.getURI(), Request.MESSAGE, callIdHeader, cSeqHeader,
				fromHeader, toHeader, viaHeaders, maxForwards);
		req.setContent(message.getBytes(), contTypeHeader);
		//req.addHeader(contLengthHeader);
		return req;
	}

	public Request register() throws Exception {

		String from = "sip:" + CenterConfiguration.name + "@"
				+ CenterConfiguration.domain;
		Address fromAddress = addressFactory.createAddress(from);
		// From
		FromHeader fromHeader = headerFactory.createFromHeader(fromAddress,
				Integer.toHexString(localTagGenerator.nextInt()));

		CallIdHeader callIdHeader = provider.getNewCallId();

		CSeqHeader cSeqHeader = headerFactory.createCSeqHeader(sequence[2]++,
				Request.REGISTER);

		// To Header (Equal to the from header in a registration message.)
		ToHeader toHeader = headerFactory.createToHeader(fromAddress, null);
		MaxForwardsHeader maxForwards = headerFactory
				.createMaxForwardsHeader(70);

		// create a host-only uri for the request uri header.
		String domain = ((SipURI) toHeader.getAddress().getURI()).getHost();

		SipURI requestURI = addressFactory.createSipURI(null, domain);

		ArrayList<ViaHeader> viaHeaders = getLocalViaHeaders();

		Request request = messageFactory.createRequest(requestURI,
				Request.REGISTER, callIdHeader, cSeqHeader, fromHeader,
				toHeader, viaHeaders, maxForwards);

		// // JvB: use Route header in addition to the request URI
		// // because some servers loop otherwise
		// if(isRouteHeaderEnabled())
		// {
		// SipURI regURI = (SipURI) registrarURI.clone();
		// regURI.setLrParam();
		// RouteHeader route = headerFactory
		// .createRouteHeader( addressFactory
		// .createAddress( null, regURI ));
		//
		// request.addHeader( route );
		// }

		// Expires Header - try to generate it twice in case the default
		// expiration period is null
		ExpiresHeader expHeader = headerFactory.createExpiresHeader(25 * 60);

		request.addHeader(expHeader);

		// Contact Header (should contain IP)
		ContactHeader contactHeader = getContactHeader(InetAddress
				.getLocalHost(), CenterConfiguration.agentPort);
		contactHeader.setParameter("rinstance", "");

		contactHeader.setExpires(2 * 60);

		request.addHeader(contactHeader);

		return request;

	}

	Response answer(Request invite, Dialog d, String sdp) throws Exception {
		Response response = messageFactory.createResponse(Response.OK, invite);
		attachToTag(response, d);
		ContactHeader contactHeader = getContactHeader(InetAddress
				.getLocalHost(), CenterConfiguration.agentPort);
		response.addHeader(contactHeader);
		ContentTypeHeader contentTypeHeader = headerFactory
				.createContentTypeHeader("application", "sdp");

		response.setContent(sdp, contentTypeHeader);
		return response;

	}


	public void attachToTag(Response response, Dialog containingDialog) {
		ToHeader to = (ToHeader) response.getHeader(ToHeader.NAME);

		if (containingDialog.getLocalTag() != null)

			return;

		if (to.getTag() == null || to.getTag().trim().length() == 0) {
			String toTag = Integer.toHexString(localTagGenerator.nextInt());

			try {
				to.setTag(toTag);
			} catch (ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}

	public ContactHeader reflect(Response resp) {
		ContactHeader ch = (ContactHeader) resp.getHeader(ContactHeader.NAME);
		ViaHeader vh = (ViaHeader) resp.getHeader(ViaHeader.NAME);
		String received = vh.getReceived();
		int rport = vh.getRPort();
		String rd = "sip:" + CenterConfiguration.name + "@" + received + ":"
				+ rport;
		String sc = ch.getAddress().getURI().getScheme();
		if (rd.toLowerCase().equals(sc.toLowerCase()))
			return null;
		try {
			ch = getContactHeader(InetAddress.getByName(received), rport);
		} catch (Exception e) {
			return null;
		}
		return ch;

	}
}
