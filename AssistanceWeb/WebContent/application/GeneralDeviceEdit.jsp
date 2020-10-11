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
<s:form action="DeviceAction_update">
  <s:textfield name="id" label="设备号" readonly="true"/> 
  <s:textfield name="type" label="定型设备" readonly="true"/> 
  <s:textfield name="name" label="设备名"/>
  <s:checkboxlist  name="supportedChannel" list="{'sip','sms'}" label="支持的通道"/> 
  <s:submit  value="修改"/>
</s:form> 
<s:textarea name="deviceDifination" label="设备描述" id="devicedefination"></s:textarea>
<s:form action="DeviceAction_remove">
<s:hidden name="id" />
<s:submit value="删除设备" onclick="return confirm('真要删除？')"></s:submit>
</s:form>

<s:if test="type==null">
<s:form action="DeviceComponentAdd">
<s:hidden name="id" />
<s:submit value="增加设备组件"></s:submit>
</s:form>
</s:if>
设备组件:
	<table border="1" width="707" >
	<tr>
		<td  width="320">编号</td>
		<td  width="320">名称</td>
		<td  width="198">类型</td>
		<td  width="198">操作</td>
	</tr>
	<s:iterator value="components" >
	<tr>
		<td  width="320"><s:property value="value.localId"/>　</td>
		<td  width="320"><s:property value="value.name"/>　</td>
		<td  width="198"><s:property value="value.componentType"/>　</td>
		<td  width="198"><s:form>
		<s:hidden name="id"></s:hidden>
		<s:hidden name="localId" value="%{value.localId}"></s:hidden>
		<s:submit  action="DeviceAction_editComponent" value="修改"></s:submit>
		<s:submit  action="DeviceAction_removeComponent" value="删除"  onclick="return confirm('真要删除？')"></s:submit>
		</s:form>
		
		</td>
	</tr>
	</s:iterator>
</table>


<s:form action="DeviceRelationAction_add">
<s:hidden name="toId" value="%{id}"/>
<s:submit value="产生设备关联"></s:submit>
</s:form>
	控制我的设备:
	<table border="1" width="707" >
	<tr>
		<td  width="320">控制方</td>
		<td  width="198">控制方名称</td>
		<td  width="198">被控方</td>
		
		<td  width="167">被控设备是否注册</td>
	</tr>
	<s:iterator value="inRelation">
	<tr>
		<td  width="320"><s:property value="fromId"/>　</td>
		<td  width="320"><s:property value="reverseName"/>　</td>
		<td  width="198"><s:property value="toId"/>　</td>
		<td  width="167">
		<s:form >
		<s:hidden name="id" ></s:hidden>
		<s:submit action="DeviceRelationAction_edit" value="修改"></s:submit>
		<s:submit action="DeviceRelationAction_remove" value="删除"  onclick="return confirm('真要删除？')"></s:submit>
		</s:form>
	
		
		　</td>
	</tr>
	</s:iterator>
</table>
我控制的设备:
	<table border="1" width="707" >
	<tr>
		<td  width="320">控制方</td>
		<td  width="198">被控方</td>
		<td  width="167">被控设备是否注册</td>
	</tr>
	<s:iterator value="outRelation">
	<tr>
		<td  width="320"><s:property value="fromId"/>　</td>
		<td  width="198"><s:property value="toId"/>　</td>
		<td  width="167">　</td>
	</tr>
	</s:iterator>
</table>
</body>
</html>