

<script type='text/javascript'>
function gobutton() {
    var URL = "/api/plugin-apis/sendconfig/true";
    $.get(URL);
}

</script>
<style>
</style>


<div id="global" class="settings">
<fieldset>
<legend>FPP data2mqtt Plugin</legend>

<h3>Loaded MQTT Items</h3>
<div>
 <?php
 $fp = fopen("/home/fpp/media/config/fpp-data2mqtt-plugin", "r") or die("error");
 $ = array();
 while (!feof($fp)){
  $line = fgets($fp);
  if (strlen($line) > 1) { 
  list($ip, $start, $size) = explode(",", $line);
  $tasmotaBulbs[] = array($ip, $start);
  }
 }
 fclose($fp);
 print "<table border='1' cellspacing='0' cellpadding='3'>";
 print "<tr><td>IP</td><td>Start Channel</td></tr>";
 foreach($tasmotaBulbs as $bulb)
 {
	  print "<tr><td>{$bulb[0]}</td><td>{$bulb[1]}</td></tr>";
 }
  print "</table>";
 
 ?>
</div>

<h4>About</h4>
<p>
The FPP data2mqtt plugin will send channel data to a mqtt topics.
<p>

</fieldset>
</div>
