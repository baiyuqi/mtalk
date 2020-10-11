<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
<script type="text/javascript">
 function refreshTree(){
	 <s:if test="modelChanged==true"> 
	 parent.frames['contents'].location.reload();
	</s:if>
 }
</script>
</head>
<body  onload="refreshTree()">
<s:form action="DeviceAction_add">
<s:submit  value="新增设备"></s:submit>
</s:form>


我的设备:
	<table border="1" width="707" >
	<tr>
		<td  width="320">设备名</td>
		<td  width="198">设备号</td>

	</tr>
	<s:iterator value="models">
	<tr>
		<td  width="320"><s:property value="name"/>　</td>
		<td  width="198"><s:property value="id"/>　</td>

	</tr>
	</s:iterator>
</table>
</body>
</html>