<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
</head>
<body>
<s:form action="DeviceAction_save">
<s:textfield name="toId" label="我的设备" readonly="true"></s:textfield>
<s:textfield name="fromId" label="控制设备"></s:textfield>
<s:textfield name="reverseName" label="命名"></s:textfield>
<s:submit value="确定"></s:submit>
</s:form>
</body>
</html>