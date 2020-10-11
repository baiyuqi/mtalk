<%@ page contentType="text/html; charset=UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<html>
<head>
<title>Sign On</title>
</head>

<body>

<s:form action="Register">
	<table align="center">
		<caption>
		<h3>注册</h3>
		</caption>
		<tr>
			<td><s:textfield name="username" label="用户名" /></td>
		</tr>
	
		<tr>

			<td><s:password name="password" label="密码 " /></td>
		</tr>
		<tr>

			<td><s:password name="passwordconfirm" label="确认密码 " /></td>
		</tr>
		<tr align="center">
			<td><s:submit value="注册"/></td>
		</tr>
	</table>
</s:form>
</body>
</html>
