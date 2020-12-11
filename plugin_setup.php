
<?
function returnIfExists($json, $setting) {
    if ($json == null) {
        return "";
    }
    if (array_key_exists($setting, $json)) {
        return $json[$setting];
    }
    return "";
}

function convertAndGetSettings() {
    global $settings;
        
    $cfgFile = $settings['configDirectory'] . "/plugin.data2mqtt.json";
    if (file_exists($cfgFile)) {
        $j = file_get_contents($cfgFile);
        $json = json_decode($j, true);
        return $json;
    }
    $j = "[]";
    return json_decode($j, true);
}

$pluginJson = convertAndGetSettings();
?>


<div id="global" class="settings">
<fieldset>
<legend>FPP Data2MQTT Config</legend>

<script>

var data2mqttConfig = <? echo json_encode($pluginJson, JSON_PRETTY_PRINT); ?>;


var uniqueId = 1;
var modelOptions = "";
function AddTopic() {
    var id = $("#data2mqttTableBody > tr").length + 1;
    var html = "<tr class='fppTableRow";
    if (id % 2 != 0) {
        html += " oddRow'";
    }
    html += "'><td class='colNumber rowNumber'>" + id + ".</td><td><span style='display: none;' class='uniqueId'>" + uniqueId + "</span></td>";
    html += "<td><input type='text' minlength='1' maxlength='50' size='15' class='topic' /></td>";
    html += "<td><input type='text' minlength='1' maxlength='50' size='15' class='payload' /></td>";
    html += "<td><input type='number' value='1' min='1' max='10000000' class='startchan' />"
    html += "</tr>";
    
    $("#data2mqttTableBody").append(html);

    newRow = $('#data2mqttTableBody > tr').last();
    $('#data2mqttTableBody > tr').removeClass('selectedEntry');
    DisableButtonClass('deleteEventButton');
    uniqueId++;

    return newRow;
}

function SaveMQTTItem(row) {
    var topic = $(row).find('.topic').val();
    var payload = $(row).find('.payload').val();
    var startchan = parseInt($(row).find('.startchan').val(),10);

    var json = {
        "topic": topic,
        "payload": payload,
        "startchannel": startchan
    };
    return json;
}

function SaveMQTTItems() {
    var data2mqttConfig = [];
    var i = 0;
    $("#data2mqttTableBody > tr").each(function() {
        data2mqttConfig[i++] = SaveMQTTItem(this);
    });
    
    var data = JSON.stringify(data2mqttConfig);
    $.ajax({
        type: "POST",
        url: 'fppjson.php?command=setPluginJSON&plugin=data2mqtt',
        dataType: 'json',
        async: false,
        data: data,
        processData: false,
        contentType: 'application/json',
        success: function (data) {
           SetRestartFlag(2);
        }
    });
}


function RenumberRows() {
    var id = 1;
    $('#data2mqttTableBody > tr').each(function() {
        $(this).find('.rowNumber').html('' + id++ + '.');
        $(this).removeClass('oddRow');

        if (id % 2 != 0) {
            $(this).addClass('oddRow');
        }
    });
}
function RemoveTopic() {
    if ($('#data2mqttTableBody').find('.selectedEntry').length) {
        $('#data2mqttTableBody').find('.selectedEntry').remove();
        RenumberRows();
    }
    DisableButtonClass('deleteEventButton');
}


$(document).ready(function() {
                  
    $('#data2mqttTableBody').sortable({
        update: function(event, ui) {
            RenumberRows();
        },
        item: '> tr',
        scroll: true
    }).disableSelection();

    $('#data2mqttTableBody').on('mousedown', 'tr', function(event,ui){
        $('#data2mqttTableBody tr').removeClass('selectedEntry');
        $(this).addClass('selectedEntry');
        EnableButtonClass('deleteEventButton');
    });
});

</script>
<div>
<table border=0>
<tr><td colspan='2'>
        <input type="button" value="Save" class="buttons genericButton" onclick="SaveMQTTItems();">
        <input type="button" value="Add" class="buttons genericButton" onclick="AddTopic();">
        <input id="delButton" type="button" value="Delete" class="deleteEventButton disableButtons genericButton" onclick="RemoveTopic();">
    </td>
</tr>
</table>

<div class='fppTableWrapper fppTableWrapperAsTable'>
<div class='fppTableContents'>
<table class="fppTable" id="data2mqttTable"  width='100%'>
<thead><tr class="fppTableHeader"><th>#</th><th></th><th>Topic</th><th>Payload</th><th>Start Chan</th></tr></thead>
<tbody id='data2mqttTableBody'>
</tbody>
</table>
</div>
</div>
<div>
<table border=1>
<tr><td colspan='2'>Replace Values In Payload i.e. "%R%,%G%,%B%"</td>
</tr><tr><td>Parmeter</td><td>Key</td></tr>
</tr><tr><td>1st Channel(0-255)</td><td>%R%</td></tr>
</tr><tr><td>2st Channel(0-255)</td><td>%G%</td></tr>
</tr><tr><td>3st Channel(0-255)</td><td>%B%</td></tr>
</tr><tr><td>1st Channel(0-100)</td><td>%RS%</td></tr>
</tr><tr><td>2st Channel(0-100)</td><td>%GS%</td></tr>
</tr><tr><td>3st Channel(0-100)</td><td>%BS%</td></tr>
</tr><tr><td>Hue Value(0-360)</td><td>%H%</td></tr>
</tr><tr><td>Sat Value(0-100)</td><td>%S%</td></tr>
</tr><tr><td>Int Value(0-100)</td><td>%I%</td></tr>
</table>
</div>
</div>
<script>

$.each(data2mqttConfig, function( key, val ) {
    var row = AddTopic();
    $(row).find('.topic').val(val["topic"]);
    $(row).find('.payload').val(val["payload"]);
    $(row).find('.startchannel').val(val["startchannel"]);

});
</script>

</fieldset>
</div>
