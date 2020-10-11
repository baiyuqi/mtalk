package com.tc.browser.facade;

import java.util.List;
import java.util.Map;

public interface DocumentService {
	void save(String user, Document doc, boolean index);
	List<Document> search(String user, Map<String, String> condition);
}
