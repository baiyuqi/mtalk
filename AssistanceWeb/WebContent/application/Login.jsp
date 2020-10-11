<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<title>Sign On</title>
</head>

<body>

<s:form action="Login">
	<table align="center">
		<caption>
		<h3>登录</h3>
		</caption>
		<tr>
			<td><s:textfield name="username" label="用户名" /></td>
		</tr>
		<tr>

			<td><s:password name="password" label="密码 " /></td>
		</tr>
		<tr align="center">
			<td><s:submit value="登录"/></td>
		</tr>
		<li><a href="<s:url action="Register_input"/>">Register</a></li>
	</table>
</s:form>

</body>
</html>
