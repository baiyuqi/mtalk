<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
</head>
<body>
<s:form action="DeviceAction_createComponent">
<s:textfield name="id" value="%{#parameters['id']}" label="我的设备"></s:textfield>
<s:textfield name="localId" label="组件编号"></s:textfield>
<s:select name="componentType" list="#application['environment'].deviceManager.profiles" listValue="value.typeId" label="组件类型"></s:select>          
<s:textfield name="componentName" label="组件名称"></s:textfield>

<s:submit value="确定"></s:submit>
</s:form>
</body>
</html>