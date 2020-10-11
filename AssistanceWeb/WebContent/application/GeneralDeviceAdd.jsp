<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>

</head>
<body>
<s:form action="DeviceAction_save">
  <s:textfield name="id" label="设备号"  required="true"/>          
  <s:textfield name="name" label="设备名"  required="true"/>          
  <s:select name="prototype" list="#application['environment'].deviceManager.prototypes" listValue="value.name" label="设备原型"/>     
  <s:checkboxlist  name="supportedChannel" list="{'sip','sms'}" label="请选择支持的通道"/>     
  <s:submit  value="确定"/>
</s:form> 
</body>
</html>