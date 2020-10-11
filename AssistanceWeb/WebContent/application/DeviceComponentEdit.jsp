<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
</head>
<body>
<s:form action="DeviceAction_updateComponent">
<s:textfield name="id" label="我的设备" readonly="true"></s:textfield>
<s:textfield name="localId" label="组件编号"  readonly="true"></s:textfield>
<s:textfield name="componentType" label="组件类型"  readonly="true"></s:textfield>
<s:textfield name="componentName" label="组件名称"></s:textfield>

<s:submit value="确定"></s:submit>
</s:form>
</body>
</html>