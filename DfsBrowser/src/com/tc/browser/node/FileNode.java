package com.tc.browser.node;

import java.io.File;
import java.io.IOException;

import javax.swing.tree.DefaultMutableTreeNode;

import org.apache.commons.httpclient.HttpException;

public class FileNode extends DefaultMutableTreeNode {

	protected boolean explored = false;
	String name;
	public FileNode(String s){
		super();
		name = s;
	}
	@Override
	public String toString() {
		
		return name;
	}
	public FileNode(){
		super();
	}
	public boolean getAllowsChildren() {
		return isDirectory();
	}

	public boolean isLeaf() {
		return !isDirectory();
	}

	public boolean isExplored() {
		return explored;
	}

	public boolean isDirectory(){
		return true;
	}

	
	public void explore() throws HttpException, IOException{
		
	}

	public void reexplore() throws HttpException, IOException {
		this.removeAllChildren();
		explored = false;
		explore();
	}
	public String getFilePath() {
		return null;
	}
}