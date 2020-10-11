<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
</head>
<body>
<s:form action="DeviceRelationAction_update">
<s:hidden name="id"></s:hidden>
<s:textfield name="toId" label="我的设备" readonly="true"></s:textfield>
<s:textfield name="fromId" label="控制设备" readonly="true"></s:textfield>
<s:textfield name="reverseName" label="关系命名"></s:textfield>
<s:submit value="确定"></s:submit>
</s:form>
</body>
</html>