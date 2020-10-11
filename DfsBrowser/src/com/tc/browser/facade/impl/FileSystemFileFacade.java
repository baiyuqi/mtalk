package com.tc.browser.facade.impl;

import java.io.File;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import com.tc.browser.facade.FileFacade;

public class FileSystemFileFacade implements FileFacade {
	FileSystem fs;
	public FileSystemFileFacade(FileSystem fs){
		this.fs = fs;
	}
	@Override
	public void copy(String local, String path) {
	/*	try {
			fs.copyFromLocalFile(new Path(local), new Path(path));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
*/
	}

}
