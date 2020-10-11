package com.tc.action;

import java.util.List;

import org.apache.struts2.interceptor.validation.SkipValidation;

import com.opensymphony.xwork2.ModelDriven;

public abstract class BaseAction extends EnvironmentSupport {

	private String id;
	List models;
	 boolean modelChanged = false;
	  public boolean isModelChanged() {
			return modelChanged;
		}
	@SkipValidation
	public String add() {
		return "add";
	}

	public String save() { // insert
		this.env.getPersistenceFacade().persist(getModel());
		modelChanged=true;
		return list();
	}

	@SkipValidation
	public String edit() {
		return "edit";
	}

	public String update() {// update
		this.env.getPersistenceFacade().merge(getModel());
		modelChanged=true;
		return "edit";
	}
	public String remove() {// delete
		this.env.getPersistenceFacade().remove(getModelType(), id);
		modelChanged=true;
		return list();
	}

	@SkipValidation
	public String list() {
		models = this.getModelList();
		return "list";
	}

	public List getModels() {
		return models;
	}

	public String getDestination() {
		return getModelType().getSimpleName();
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public abstract Object getModel();
	protected abstract List getModelList();
	public abstract Class getModelType();
}