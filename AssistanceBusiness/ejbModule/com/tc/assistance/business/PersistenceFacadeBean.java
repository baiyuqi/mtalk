package com.tc.assistance.business;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.ejb.Stateless;
import javax.ejb.TransactionAttributeType;
import javax.persistence.EntityManager;
import javax.persistence.LockModeType;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;




public @Stateless
class PersistenceFacadeBean implements PersistenceFacade{
	@PersistenceContext
    public	EntityManager em;

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public List query(String hql, int from, int size) {
		Query q = em.createQuery(hql);
		q.setFirstResult(from);
		q.setMaxResults(size);
		return q.getResultList();
	}

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public List query(String hql) {
		Query q = em.createQuery(hql);
		try {
			return q.getResultList();
		} catch (Throwable e) {
			e.printStackTrace();
			return null;
		}
	}

	public void lock(Object o, LockModeType lockModeType) {
		if (o instanceof Collection) {
			Collection<?> c = (Collection<?>) o;
			for (Object object : c) {
				try {
					em.lock(object, lockModeType);
				} catch (Exception e) {
				
					e.printStackTrace();
				}

			}
		} else {
			em.lock(o, lockModeType);
		}
	}

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public long count(String hql) {
		Long count = (Long) em.createQuery("select count(*) " + hql)
				.getSingleResult();
		return count;
	}

	public void removeAll(List es) {
		for (Object ob : es) {
			Object id = null;
			try {
				id = ob.getClass().getMethod("getId", new Class[0]).invoke(ob,
						new Object[0]);
			} catch (Exception e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Object rb = em.find(ob.getClass(), id);

			em.remove(rb);
			
		}
	}

	public Object persist(Object o) {

			em.persist(o);
			
		return o;
	}
	public void persist(Collection os) {
		for(Object o : os)
			em.persist(o);
		
	
}

	public Object merge(Object o) {

		Object rst= null;
		try {
			rst = em.merge(o);
		} catch (RuntimeException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return rst;

	}

	public void refresh(Object o) {
		em.refresh(o);

	}

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public <T> T find(Class<T> type, Serializable id) {
		return em.find(type, id);
	}

	public void remove(Object o) {
		em.remove(o);
		

	}

	public void remove(Class type, Serializable id) {
		Object o = em.find(type, id);
		if (o != null) {
			em.remove(o);
			
		}

	}

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public <T> List<T> get(Class<T> type, String[] ids) {

		List rst = new ArrayList();
		return em.createQuery(
				"from " + type.getSimpleName() + " where id in (:idsList)")
				.setParameter("idsList", Arrays.asList(ids)).getResultList();

		// for (String id : ids) {
		// rst.add(em.find(type, id));
		// }
		// return rst;
	}

	@javax.ejb.TransactionAttribute(TransactionAttributeType.SUPPORTS)
	public Object unique(String hql) {
		Query q = em.createQuery(hql);
        try{
		return q.getSingleResult();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        	return null;
        }
	}

	public List nativeQuery(String hql) {

		return em.createNativeQuery(hql).getResultList();
	}

	public int nativeUpdate(String hql) {

		return em.createNativeQuery(hql).executeUpdate();
	}
	public int nativeUpdate(String hql,Map<String,Object> params) {
		Query q = em.createNativeQuery(hql);
		for (String key:params.keySet()) {
			q.setParameter(key, params.get(key));
			
		}
		 
		return q.executeUpdate();
	}
	
	public List<?> nativeQuery(String query, Object[] params) {
		Query q = em.createNamedQuery(query);
		// q.setParameter(0, null);
		if (params != null) {
			for (int i = 0; i < params.length; i++) {
				q.setParameter(i + 1, params[i]);
			}
		}
		return q.getResultList();
	}


	@Deprecated
	public List query(String hql, Map<String, Object> conditions) {
		// TODO Auto-generated method stub
		Query query = em.createQuery(hql);
		for (String key : conditions.keySet()) {
			query.setParameter(key, conditions.get(key));
		}
		return query.getResultList();
	}

	public List  queryLock(String hql,
			LockModeType lockModetype) {
		List queryList = query(hql);
	
		if (queryList instanceof Collection) {
			Collection<?> c = (Collection<?>) queryList;
			for (Object object : c) {
				try {
					em.lock(object, lockModetype);
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		} else {
			em.lock(queryList, lockModetype);
		}
		return queryList;
	}

	public long count(String sqlquery, Object[] params) {
		Query q = em.createQuery("select count(*) " + sqlquery);
		for( int i = 0; i < params.length; i++)
			q.setParameter(i+1, params[i]);
		Long count = (Long) q.getSingleResult();
		return count;
	}

	public List query(String sqlquery, Object[] params) {
		Query q = em.createQuery(sqlquery);
		for( int i = 0; i < params.length; i++)
			q.setParameter(i+1, params[i]);
		
		return q.getResultList();
	}

	@Override
	public boolean exist(Class type, Serializable id) {
		Query q = em.createQuery("select count(id) from " + type.getSimpleName() + " where id = '" + id + "'");
		long s = (Long)q.getSingleResult();
		return s == 1;
	}
	
//	public void batchedSave(List<OwnershipEnabled> tobesavedList){
//		PersistenceFacade facade = (PersistenceFacade)Contexts.getApplicationContext().get(PersistenceFacadeBean.class);
//		EntityTransaction transaction = em.getTransaction();
//		transaction.begin();
//		for (int i = 0; i < tobesavedList.size(); i++) {
//			em.persist(tobesavedList.get(i));
//			if ( i % 50 == 0 ) {
//				em.clear();
//				em.flush();
//			}
//		}
//		transaction.commit();
//		
//	}

}
