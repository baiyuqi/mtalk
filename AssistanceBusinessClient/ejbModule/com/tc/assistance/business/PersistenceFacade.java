package com.tc.assistance.business;

import java.io.Serializable;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.ejb.Local;
import javax.persistence.LockModeType;
@Local
public interface PersistenceFacade {
	List query(String hql, int from, int size);
	List query(String hql,Map<String,Object> conditions);
	List query(String sqlquery,Object[] params);
	List query(String hql);


	Object unique(String hql);
	void removeAll(List es);
	void remove(Object o);
	void remove(Class type, Serializable id);
	Object persist(Object o);
	Object merge(Object o);
	void persist(Collection os);
	boolean exist(Class type, Serializable id);
//	void refresh(Object o);

	<T> T find(Class<T> type, Serializable id);

	
	<T> List<T> get(Class<T> type, String[] ids);
	long count(String sqlquery);
	long count(String sqlquery,Object[] params);
	int nativeUpdate(String sqlquery);

	List<?> nativeQuery(String query, Object[] params);
	void lock(Object o, LockModeType lockModeType);
	List queryLock(String hql,LockModeType lockModeType);
	int nativeUpdate(String hql, Map<String, Object> params);

}
