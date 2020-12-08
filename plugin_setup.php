
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
        
    $cfgFile = $settings['configDirectory'] . "/plugin.fpp-tasmota.json";
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
<legend>FPP Tasmota Bulb Config</legend>

<script>

var tasmotaConfig = <? echo json_encode($pluginJson, JSON_PRETTY_PRINT); ?>;


var uniqueId = 1;
var modelOptions = "";
function AddBulb() {
    var id = $("#tasmotaTableBody > tr").length + 1;
    var html = "<tr class='fppTableRow";
    if (id % 2 != 0) {
        html += " oddRow'";
    }
    html += "'><td class='colNumber rowNumber'>" + id + ".</td><td><span style='display: none;' class='uniqueId'>" + uniqueId + "</span></td>";
    html += "<td><input type='text' minlength='7' maxlength='15' size='15' pattern='^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$' class='ipaddress' /></td>";
    html += "<td><input type='number' value='1' min='1' max='10000000' class='startchan' />"
    html += "</tr>";
    
    $("#tasmotaTableBody").append(html);

    newRow = $('#tasmotaTableBody > tr').last();
    $('#tasmotaTableBody > tr').removeClass('selectedEntry');
    DisableButtonClass('deleteEventButton');
    uniqueId++;

    return newRow;
}

function SaveBulb(row) {
    var ipaddress = $(row).find('.ipaddress').val();
    var startchan = parseInt($(row).find('.startchan').val(),10);

    var json = {
        "ip": ipaddress,
        "startchannel": startchan
    };
    return json;
}

function SaveTasmota() {
    var tasmotaConfig = [];
    var i = 0;
    $("#tasmotaTableBody > tr").each(function() {
        tasmotaConfig[i++] = SaveBulb(this);
    });
    
    var data = JSON.stringify(tasmotaConfig);
    $.ajax({
        type: "POST",
        url: 'fppjson.php?command=setPluginJSON&plugin=fpp-tasmota',
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
    $('#tasmotaTableBody > tr').each(function() {
        $(this).find('.rowNumber').html('' + id++ + '.');
        $(this).removeClass('oddRow');

        if (id % 2 != 0) {
            $(this).addClass('oddRow');
        }
    });
}
function RemoveBulb() {
    if ($('#tasmotaTableBody').find('.selectedEntry').length) {
        $('#tasmotaTableBody').find('.selectedEntry').remove();
        RenumberRows();
    }
    DisableButtonClass('deleteEventButton');
}


$(document).ready(function() {
                  
    $('#tasmotaTableBody').sortable({
        update: function(event, ui) {
            RenumberRows();
        },
        item: '> tr',
        scroll: true
    }).disableSelection();

    $('#tasmotaTableBody').on('mousedown', 'tr', function(event,ui){
        $('#tasmotaTableBody tr').removeClass('selectedEntry');
        $(this).addClass('selectedEntry');
        EnableButtonClass('deleteEventButton');
    });
});

</script>
<div>
<table border=0>
<tr><td colspan='2'>
        <input type="button" value="Save" class="buttons genericButton" onclick="SaveTasmota();">
        <input type="button" value="Add" class="buttons genericButton" onclick="AddBulb();">
        <input id="delButton" type="button" value="Delete" class="deleteEventButton disableButtons genericButton" onclick="RemoveBulb();">
    </td>
</tr>
</table>

<div class='fppTableWrapper fppTableWrapperAsTable'>
<div class='fppTableContents'>
<table class="fppTable" id="tasmotaTable"  width='100%'>
<thead><tr class="fppTableHeader"><th>#</th><th></th><th>IP Address</th><th>Start Chan</th></tr></thead>
<tbody id='tasmotaTableBody'>
</tbody>
</table>
</div>
</div>

</div>
<script>

$.each(tasmotaConfig, function( key, val ) {
    var row = AddBulb();
    $(row).find('.ipaddress').val(val["ip"]);
    $(row).find('.startchannel').val(val["startchannel"]);

});
</script>

</fieldset>
</div>
