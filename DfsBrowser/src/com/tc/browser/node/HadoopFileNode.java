package com.tc.browser.node;

import java.io.IOException;

import org.apache.commons.httpclient.HttpException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;


public class HadoopFileNode extends FileNode {
	FileSystem fs;
	Path path;
	public HadoopFileNode(FileSystem f, String p){
		this.fs = f;
		path = new Path(p);
	}
	@Override
	public void explore() throws HttpException, IOException {
		
		
		if (!isDirectory()) {
			return;
		}

		if (!isExplored()) {
			
			FileStatus[] children = fs.listStatus(path);
			for (int i = 0; i < children.length; ++i) {
				
				if (children[i].isDir()) {
					add(new HadoopFileNode(fs, children[i].getPath().toString()));
				}
			}
			for (int i = 0; i < children.length; ++i) {
				if (!children[i].isDir()) {
					add(new HadoopFileNode(fs, children[i].getPath().toString()));
				}
			}
			explored = true;
		}



		

	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		String name = path.getName();
		if(name != null && !name.equals(""))
			return name;
		return path.toString();
	}
	@Override
	public boolean isDirectory() {
		// TODO Auto-generated method stub
		try {
			return fs.getFileStatus(path).isDir();
		} catch (IOException e) {
			e.printStackTrace();
			return false;

		}
	}
	public String getFilePath() {
		return path.toString();
	}
}
