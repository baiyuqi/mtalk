<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<%@taglib prefix="sx" uri="/struts-dojo-tags"%>
<html>
<head>
<title>Missing Feature</title>
<link href="css.css" rel="stylesheet" type="text/css" />

<sx:head/>
</head>

<body >

<script language="JavaScript" type="text/javascript">
  dojo.event.topic.subscribe(
		  "treeSelected", 
		  function treeNodeSelected(node) {
			  var url;
			  if(node.node.widgetId == '1'){
				 
				  url = 'Monitor';
			  }
			  else{
				  url = 'DeviceControlAction_control.action?requestId='+node.node.widgetId;
			  }
  			parent.frames['main'].location= url;

		  	}
	);

  window.onload=function(){ 
	  var nodes =dojo.widget.manager.getWidgetsByType('struts:StrutsTreeNode'); 
	  for( var i=0; i < nodes.length; i++){ 
	      nodes[i].expand(); 
	  } 
	  }
	  	
</script>



 <div id="sidebar">

<sx:tree  id="tree"
	rootNode="%{treeRootNode}" childCollectionProperty="children"
	nodeIdProperty="id" nodeTitleProperty="name"
	treeSelectedTopic="treeSelected">
</sx:tree>
 </div>

</body>

</html>
