package com.tc.browser.node;

import java.io.File;
import java.io.IOException;

import javax.swing.tree.DefaultMutableTreeNode;

import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpURL;
import org.apache.webdav.lib.WebdavResource;


public class DavFileNode extends FileNode{
	WebdavResource wdr;

	public DavFileNode(WebdavResource client) {
		this.wdr = client;
		setUserObject(wdr);
	}

	public boolean getAllowsChildren() {
		return isDirectory();
	}

	public boolean isLeaf() {
		return !isDirectory();
	}

	public String toString() {
		WebdavResource file = (WebdavResource) getUserObject();
		String filename = file.getName();
		int index = filename.lastIndexOf(File.separator);

		String s = (index != -1 && index != filename.length() - 1) ? filename
				.substring(index + 1) : filename;
		return s;
	}

	public void explore() throws HttpException, IOException {
		if (!isDirectory()) {
			return;
		}

		if (!isExplored()) {
			WebdavResource[] children = wdr.listWebdavResources();

			for (int i = 0; i < children.length; ++i) {
				if (children[i].isCollection()) {
					add(new DavFileNode(children[i]));
				}
			}
			for (int i = 0; i < children.length; ++i) {
				if (!children[i].isCollection()) {
					add(new DavFileNode(children[i]));
				}
			}
			explored = true;
		}
	}

	@Override
	public boolean isDirectory() {
		
		return wdr.isCollection();
	}



}