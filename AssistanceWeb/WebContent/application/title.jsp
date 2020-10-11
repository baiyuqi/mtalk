<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>

<html>
<head>
<script type='text/javascript' src='/AssistanceWeb/common/js/util.js'></script>
<script type="text/javascript" src="/AssistanceWeb/dwr/engine.js"></script>
<script type='text/javascript'
	src='/AssistanceWeb/dwr/interface/AlarmRegister.js'>
	
</script>
<SCRIPT>
	function showAlarm(alarmType) {
		alert(alarmType)
	}
	function manage() {
		parent.frames['contents'].location.href = "ManageTreeAction.action";
		parent.frames['main'].location.href = "DeviceAction_list.action";
	}
	function control() {
		parent.frames['contents'].location.href = "ControlTreeAction.action";
		parent.frames['main'].location.href = "Monitor.action";
	}
	//-->
</SCRIPT>
</head>

<body onbeforeunload="AlarmRegister.unregister();"
	onload="dwr.engine.setActiveReverseAjax(true);AlarmRegister.register();">

<s:property value="#session.user.name" />
<input type="button" value="设备管理" onclick="manage();">
<input type="button" value="设备监控" onclick="control();">
</body>
</html>