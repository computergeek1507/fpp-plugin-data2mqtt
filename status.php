
<style>
</style>


<div id="global" class="settings">
<fieldset>
<legend>FPP data2mqtt Plugin</legend>

<h3>Loaded MQTT Items</h3>
<div>
<?php
 $fp = fopen("/home/fpp/media/config/fpp-plugin-data2mqtt", "r") or die("error");
 $mqttItems = array();
 while (!feof($fp)){
  $line = fgets($fp);
  if (strlen($line) > 1) { 
  list($topic, $payload, $start, $size) = explode(";", $line);
  $mqttItems[] = array($topic, $payload, $start);
  }
 }
 fclose($fp);
 print "<table border='1' cellspacing='0' cellpadding='3'>";
 print "<tr><td>Topic</td><td>Payload</td><td>Start Channel</td></tr>";
 foreach($mqttItems as $mqtt)
 {
	  print "<tr><td>{$mqtt[0]}</td><td>{$mqtt[1]}</td><td>{$mqtt[2]}</td></tr>";
 }
  print "</table>";
 
 ?>
</div>

<h4>About</h4>
<p>
The FPP data2mqtt plugin will send channel data to mqtt topics.
<p>

</fieldset>
</div>
