<%@ page contentType="text/html; charset=UTF-8"%>
<%@ taglib prefix="s" uri="/struts-tags"%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>

 <script src="http://www.google.com/jsapi?key=ABQIAAAAqPfnRvzjCHUxbXrov2skQRQQv-XifgqJyry7v_GLd4f8fOymoBTkk8tY3PDT5aU2_5HF8x6RRakviQ"
    type="text/javascript">
    </script>
   <script type="text/javascript"> 
      google.load("maps", "2"); 
      google.load("search", "1"); 
</script>
  <script type="text/javascript">
   var map;
  function init(){
   map = new GMap2(document.getElementById("map"));
   var pt = new GLatLng(30.609682, -96.340264);
   map.setCenter(pt,16);
   var marker = new GMarker(pt);
   var html = "<br>nihao</br>";
   GEvent.addListener(marker, "click", function(){
   	marker.openInfoWindowTabsHtml(html);
   });
   //map.addOverlay(marker);
   map.addControl(new GSmallMapControl());
   map.addControl(new GOverviewMapControl());
   map.addControl(new GMapTypeControl());
      map.addControl(new GScaleControl());
      }
       google.setOnLoadCallback(init); 
   //map.setMapType(G_HYBRID_MAP);
  </script>
  <script type="text/javascript">


 function draw(){

 var request =   GXmlHttp.create();
 request.open("GET","http://192.168.1.100:8080/DataAnalyzer/DataService",true);
	 request.send();


	request.onreadystatechange = new function() {
	//alert(request.getAllResponseHeaders());

			
			var doc = request.responseXML;
			
			var root = doc.documentElement;
			
			var ms = root.getElementsByTagName("marker");
			for(var i = 0; i < ms.length; i++){
			var p = ms[i];
			var pt = new GLatLng(parseFloat(p.getAttribute("lat")),parseFloat(p.getAttribute("lng")));
			alert(pt);
			var marker = new GMarker(pt);
			map.addOverlay(marker);
			}
	
		}


	}
 </script>
 
</head>
<body>
 <div id="map" style="width:400px;height:300px">
    <span style="color:#676767;font-size:11px;margin:10px;padding:4px;">Loading...</span>
  </div>
<input type="button" value="draw" onclick = "draw();" />
 
  <style type="text/css">
    @import url("http://www.google.com/uds/css/gsearch.css");
  </style>


<s:form>
<s:hidden name="id"></s:hidden>
<s:hidden name="localId"></s:hidden>
<s:hidden name="requestId"></s:hidden>
<s:hidden name="action" value="location"></s:hidden>
<s:submit value="定位" action="DeviceControlAction_request"/>

</s:form>
<s:form>
<s:hidden name="id"></s:hidden>
<s:hidden name="localId"></s:hidden>
<s:hidden name="requestId"></s:hidden>
<s:hidden name="action" value="picture"></s:hidden>
<s:submit value="取图" action="DeviceControlAction_request"/>

</s:form>
</body>
</html>