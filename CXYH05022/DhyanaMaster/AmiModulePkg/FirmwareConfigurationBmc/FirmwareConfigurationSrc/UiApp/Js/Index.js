
PASSWORD_MAX_SIZE = 20 ;
PASSWORD_MIN_SIZE = 3 ;

SAVE_AND_EXIT_CALLBACK  = "0x200" ;
BOOT_OPTION_PRIORITIES  = "0x300" ;

BBS_FLOPPY          = "0x01" ;
BBS_HARDDISK        = "0x02" ;
BBS_CDROM           = "0x03" ;
BBS_PCMCIA          = "0x04" ;
BBS_USB             = "0x05" ;
BBS_EMBED_NETWORK   = "0x06" ;
BBS_BEV_DEVICE      = "0x80" ;
BBS_UNKNOWN         = "0xff" ;

SYSTEM_LANGUAGE     = "0x400" ;
SYSTEM_DATETIME     = "0x500" ;
SYSTEM_DATE         = "0x501" ;
SYSTEM_TIME         = "0x502" ;
SYSTEM_PASSWORD     = "0x600" ;
NON_SYSTEM_PASSWORD = "0x601" ;

CurrentLanguage     = "en-US" ;

RemoteBiosModeOnReStart = false ;

var Timex = null ;

var timeDiff = 0 ;
var serverDateTime = new Date() ; //this would come from the server
//serverDateTime.setHours(04, 35, 1);
//serverDateTime.setFullYear(2020, 11, 14);

var localTime = Date.now();
timeDiff = serverDateTime - localTime;

Delta_Org = {
    "@odata.context": "/redfish/v1/$metadata#Systems/Members/MAC-XXYY/OEM/BIOSConfiguration/$entity",
    "@odata.id": "/redfish/v1/Systems/MAC-XXYY/OEM/BIOSConfiguration",
    "@odata.type": "#SetupConfiguration.BIOSConfigurationDelta",
    "Name": "SetupConfigurationDelta",
    "Description": "SetupConfiguration Delta for MAC-XXYY",
    "RemoteBiosModeOnReStart": false,
    "Members": []
} ;

Delta = {
    "AttributeRegistry":"BiosAttributeRegistry0ACAK.0.19.0",
    "Attributes":{
        
    }
}

Authentication = {
    "Attributes":[]
}

Resources1 = {
   
    "Members": [
        //{"type":"Config","name":"index.json"},
        //{"type":"Layout","name":"BiosAttributeRegistry.json"},
        {"type":"Config","name":"redfish/v1/Systems/Self/Bios/"}
        //{"type":"Layout","name":"http://"+IPAddress+"/redfish/v1/Registries/BiosAttributeRegistry0ACAK.0.19.0.json"}
    ]
}

Resources = {
   
    "Members": [
        {"type":"Config","name":""},
        {"type":"Layout","name":"/bios/SetupData.xml"}
    ]
}

AttributeRegistry = {} ;

var CHANGED = false ;

var IPAddress = "" ;
var isValidIp = false ;

var to = false;
var BMCADDRESS = false ;

/* The IP to test (this IP is valid) */
var str = "100.150.200.250";
/* The regular expression pattern */
var pattern = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/g;
/* use  javascript's test() function to execute the regular expression and then store the result - which is either true or false */
var bValidIP = pattern.test(str);

//Polyfill for browsers that don't support methhods (matches, closest)
if (!Element.prototype.matches){
    Element.prototype.matches = Element.prototype.msMatchesSelector || Element.prototype.webkitMatchesSelector;
}
    
if (!Element.prototype.closest){
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
   Element.prototype.closest = function(s) {
        var el = this;
        if (!document.documentElement.contains(el)) return null;
        do {
            if (el.matches(s)) return el;
            el = el.parentElement || el.parentNode;
        } while (el !== null); 
        return null;
    };
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
}

var validateIp = function (e) {
	if (!e)
      e = window.event;
	
	
		var v = document.getElementById('nav_address').value;
        
        var bValidIP = pattern.test(v);
                
        //if(!bValidIP){
        //    isValidIp = false ;
        //    alert("Please Enter a valid IP address");
        //}else{
            IPAddress = v ;
            isValidIp = true ;
    
            //alert(v)
    
            Resources = { "Members": [{"type":"Config","name":"http://172.16.97.177/redfish/v1/Systems/Self/Bios/"}] }
    
    
            //getResources() ;
        //}
		
	
}

function getRegistryEntryType(id){
            
    var entry = getRegistryEntry(id) || null ;
        
    if(entry != null){
        return entry.Type ;
    }
    
    return entry ;
}

function getRegistryEntry(id){
            
    var entry = null ;
    
    if(AttributeRegistry != null){
                
        if(typeof AttributeRegistry.RegistryEntries == 'object'){
                    
            var attributes = AttributeRegistry.RegistryEntries.Attributes ;
            
            for(var i=0; i<attributes.length; i++){
                
                if(attributes[i].AttributeName == id){
                    entry = attributes[i] ;
                    break ;
                }
            }//end-for
        }
    }
    
    return entry ;
}


/**
 * Sends http GET request for current DateTime from Server
 *
 * @param void 
 * 
 * @return void
 *
 **/
function getDateTime(){
    var url = "/SETUP/ISSPECIAL?id="+SYSTEM_DATETIME ;
    var xmlhttp = createHttpRequest();
    
    xmlhttp.onreadystatechange=function(){
        if (xmlhttp.readyState==4 && xmlhttp.status==200){
            var res = xmlhttp.responseText ;
            res = res.split("&") ; 
            var id = res[0].split("=")[1] ;
            var ack = res[1].split("=")[1] ;
            
            if(ack == "ACK"){
                
                var Year = parseInt(res[2].split("=")[1]) ;
                var Month = parseInt(res[3].split("=")[1]) ;
                var Day = parseInt(res[4].split("=")[1]) ;
                var Hour = parseInt(res[5].split("=")[1]) ;
                var Minute = parseInt(res[6].split("=")[1]) ;
                var Second = parseInt(res[7].split("=")[1]) ;
                
                serverDateTime = new Date() ; 
                serverDateTime.setHours(Hour, Minute, Second);
                serverDateTime.setFullYear(Year, Month, Day) ;

                var localTime = Date.now();
                timeDiff = serverDateTime - localTime;
                
                if(CurrentPage != null){ //Show the same page
                    reload() ;
                    var id = CurrentPage.split("&")[0] ;
                    var guid = CurrentPage.split("&")[1] ;
                    showForm(id, guid );    
                }
            }
        }
    }
    
    xmlhttp.open("GET", url, true) ;
    xmlhttp.send() ;
}


/**
 * Sends HttpRequest POST Request for Setting Server Date or Time [HTTP POST Request]
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function setDateTime(id){
    
    if(!id)return ;
    
    //Get the HTML control making the request...
    var control = document.getElementById(id) ;
    
    if(control){
        
        var url = "/SETUP/ISSPECIAL?id="+control.id  ; 
        
        if(id == SYSTEM_TIME){
            var time = control.value.split(":");
            if(time.length != 3)
                return ;
                
            url += "&Hour="+time[0]  ; 
            url += "&Minute="+time[1]  ; 
            url += "&Second="+time[2]  ;     
        }else{
            var date = control.value.split("/");
            if(date.length != 3) 
                return
                
            url += "&Month="+date[0]  ; 
            url += "&Day="+date[1]  ; 
            url += "&Year="+date[2]  ; 
        }
                
        var xmlhttp = createHttpRequest();
        
        xmlhttp.onreadystatechange=function() {
            
            if (xmlhttp.readyState==4 && xmlhttp.status==200){
                var res = xmlhttp.responseText ;
                res = res.split("&") ; 
                var id = res[0].split("=")[1] ;
                var ack = res[1].split("=")[1] ;

                if(ack=="ACK" || ack=="NACK"){
                    getDateTime();
                }//end-if ack 
            }//end-if xmlhttp.readyState
        }
                
        //Send the request to set server time...
        if(xmlhttp != null){
            xmlhttp.open("POST", url, true) ;
            xmlhttp.send() ;
        }
    }
}

/**
 * JavaScript date validation 
 *
 * @param txtDate - date strinng
 * @retval separator - delimiter
 *
 *
 * courtesy of http://www.redips.net/javascript/date-validation/
 **/
function isDate(txtDate, separator) {
    var aoDate,           // needed for creating array and object
        ms,               // date in milliseconds
        month, day, year; // (integer) month, day and year
    // if separator is not defined then set '/'
    if (separator === undefined) {
        separator = '/';
    }
    // split input date to month, day and year
    aoDate = txtDate.split(separator);
    // array length should be exactly 3 (no more no less)
    if (aoDate.length !== 3) {
        return false;
    }
    // define month, day and year from array (expected format is m/d/yyyy)
    // subtraction will cast variables to integer implicitly
    month = aoDate[0] - 1; // because months in JS start from 0
    day = aoDate[1] - 0;
    year = aoDate[2] - 0;
    // test year range
    if (year < 1000 || year > 3000) {
        return false;
    }
    // convert input date to milliseconds
    ms = (new Date(year, month, day)).getTime();
    // initialize Date() object from milliseconds (reuse aoDate variable)
    aoDate = new Date();
    aoDate.setTime(ms);
    // compare input date and parts from Date() object
    // if difference exists then input date is not valid
    if (aoDate.getFullYear() !== year ||
        aoDate.getMonth() !== month ||
        aoDate.getDate() !== day) {
        return false;
    }
    // date is OK, return true
    return true;
}


/**
 * Creates HttpRequest Object
 *
 * @param void
 * @retval object - XMLHttpRequest
 *
 **/
function createHttpRequest(){
    var xmlhttp = null ;
	
	if (window.XMLHttpRequest){// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest() ;
	}else  {// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP") ;
	}   
    
    return xmlhttp ;
}


/**
 * Special Callback handler for Boot Priorities, BBS Priorities etc. [HTTP_GET Request]
 *
 * @param void 
 * 
 * @return void
 *
 **/
function specialBootCallbackOnGet(){
    
    var xmlhttp = this ;
    
    if (xmlhttp.readyState==4 && xmlhttp.status==200){
        var res = xmlhttp.responseText ;
        res = res.split("&") ; 
        var id = res[0].split("=")[1] ;
        var ack = res[1].split("=")[1] ;
        var options = res[2].substr(res[2].indexOf("<"), res[2].lastIndexOf(">")) ;

        if(ack=="ACK"){

            switch(id) {
                case BBS_FLOPPY :  
                case BBS_HARDDISK :                             
                case BBS_CDROM :   
                case BBS_PCMCIA :
                case BBS_USB :     
                case BBS_EMBED_NETWORK :     
                case BBS_BEV_DEVICE :     
                case BBS_UNKNOWN :     
                case BOOT_OPTION_PRIORITIES :
                    break ;  

                default:
                    return    
            };
            
            //Get the HTML control who made the request...
            var control = document.getElementById(id) ;

            if(control){

                // turn off the loader image...
                if(control.activityPtr)
                    control.activityPtr.className = "loader inactive" ;

                // Create the XML Options DOMS...
                var BootPriority = loadXMLString(options) ;

                if(BootPriority){
                    // get node options...
                    var BootOptions = BootPriority.getElementsByTagName("Option") ;

                    // create and add boot options...
                    for (var i = 0; i < BootOptions.length; i++){
                        var option = document.createElement("option") ;
                        option.text = BootOptions[i].getAttribute("Option") ;
                        option.value = BootOptions[i].getAttribute("value") ;
                        option.enabled = (BootOptions[i].getAttribute("Enabled")=="TRUE")? true : false ;
                        if(option.enabled == false)
                            option.className = "InactiveBoot" ;

                        control.add(option) ;
                    }//end-for

                    if(control.className.indexOf("oneOf BootOption")>-1)
                        control.size = BootOptions.length ;

                }//end-if BootPriority
            }//end-if control

        }else{
             //WHAT???
        }//end-if ACK
    }//end-if xmlhttp.readyState
}//end specialBootCallbackOnGet

/**
 * Special Callback handler for Boot Priorities, BBS Priorities etc. [HTTP_POST Request]
 *
 * @param void 
 * 
 * @return void
 *
 **/
function specialBootCallbackOnPost(){
    
    var xmlhttp = this ;
    
    if (xmlhttp.readyState==4 && xmlhttp.status==200){
        var res = xmlhttp.responseText ;
        res = res.split("&") ; 
        var id = res[0].split("=")[1] ;
        var ack = res[1].split("=")[1] ;

        if(ack=="ACK" || ack=="NACK"){
            
            reload() ;
                            
            if(CurrentPage != null){ //Show the same page
                var id = CurrentPage.split("&")[0] ;
                var guid = CurrentPage.split("&")[1] ;
                showForm(id, guid );    
            }
        }//end-if ack 
    }//end-if xmlhttp.readyState
}

/**
 * Sends HttpRequest POST Request for Boot Priorities, BBS Priorities etc. [HTTP POST Request]
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function specialBootPostRequest(id){
    
    if(!id)return ;
    
    //Get the HTML control making the request...
    var control = document.getElementById(id) ;
    
    if(control){
        
        var url = "/SETUP/ISSPECIAL?id="+control.id+"&value="  ;
        var value = ""  ;
                
        var selectOptions = control.getElementsByTagName('option');
        for (var i = 0; i < selectOptions.length; i++) {
            var opt = selectOptions[i];
            
            value += opt.value + ":" ;
            
            if (opt.enabled == false) {
                value += "0xFFFF" ;
            }else{
                value += opt.value ;
            }
            
            if(i<selectOptions.length-1)
                value += ","
        }
        
        //*  
        // If ManagemenServer Mode (i.e., Delta Update), call setConfigData
        if(1){
            setConfigData(id, value) ;
            showActivity(id, false) ;
            return ;   
        }
        //*/
        
        
        url += value ;
        
        var xmlhttp = createHttpRequest();
                
        //Send the request to get options...
        if(xmlhttp != null){
            control.inUpdate=true ;
            xmlhttp.onreadystatechange = specialBootCallbackOnPost ;
            xmlhttp.open("POST", url, true) ;
            xmlhttp.send() ;
        }
    }
}

/**
 * Sends HttpRequest POST Request for Boot Priorities, BBS Priorities etc. [HTTP GET Request]
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function specialBootGetRequest(id){
    
    if(!id)return ;
    
    
    // If ManagemenServer Mode, call getConfigData
    if(1){
        getConfigData(id) ;
        showActivity(id, false) ;
        return ;   
    }
    
    
    //Get the HTML control making the request...
    var control = document.getElementById(id) ;
    
    if(control){

        var url = "/SETUP/ISSPECIAL?id="+control.isSpecial ;
        var xmlhttp = createHttpRequest();

        //Send the request to get options...
        if(xmlhttp != null){ 
            xmlhttp.onreadystatechange = specialBootCallbackOnGet ;
            xmlhttp.open("GET", url, true) ;
            xmlhttp.send() ;
        }
    }
}

/**
 * Special Callback handler for Password. [HTTP_GET Request]
 *
 * @param void 
 * 
 * @return void
 *
 **/
function specialPasswordGetResponse(){
    
    var xmlhttp = this ;
    
    if (xmlhttp.readyState==4 && xmlhttp.status==200){
        /*
        var res = xmlhttp.responseText ;
        res = res.split("&") ; 
        var id = res[0].split("=")[1] ;
        var ack = res[1].split("=")[1] ;
        var options = res[2].substr(res[2].indexOf("<"), res[2].lastIndexOf(">")) ;
        */
        
        //GET response format example -> "id=SETUP000&value=0" ;
		var res = xmlhttp.responseText ;
		res = res.split("&") ; 
		var id = res[0].split("=")[1] ;
		var value = res[1].split("=")[1] ;
        
        var target = document.getElementById(id) ;
        
        if(target!=null){
                //target.value = value ; // set the initial value
                
                var pageId = target.ControlPageID ;
                var guid = target.PlatformGuid ;
                var control = SetupDataGetControlById(id, guid) ;
                
                // Update control cache...
                if(control){ 
                    switch (target.tagName) {
                        
                        case "INPUT": 
                            if(target.type == "password"){
                                    
                                var hasPassword = parseInt(value) ;
                                    
                                if(hasPassword){
                                    target.parentNode.style.display = "block" ;
                                    control.setAttribute("hasPassword", hasPassword) ;
                                }else{
                                    target.parentNode.style.display = "none" ;
                                    control.setAttribute("hasPassword", 0) ;
                                }
                            }
                        default: 
                            
                    }//end-switch
                }//end-control
                
                // Remove activity indicator
                showActivity(id, false) ;
            }//end-target
    }//end-if xmlhttp.readyState
}//end specialBootCallbackOnGet

/**
 * Sends HttpRequest GET Request for Password. [HTTP GET Request]
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function specialPasswordGetRequest(id){
    
    if(!id)return ;
    
    //Get the HTML control making the request...
    var control = document.getElementById(id) ;
    
    if(control){

        var url = "/SETUP/ISSPECIAL?id="+control.isSpecial ;
        var xmlhttp = createHttpRequest();

        //Send the request to get options...
        if(xmlhttp != null){ 
            // Display activity indicator
            showActivity(id, true) ;    
            xmlhttp.onreadystatechange = specialPasswordGetResponse ;
            xmlhttp.open("GET", url, true) ;
            xmlhttp.send() ;
        }
    }
}


/**
 * Scrolls page container to the top.
 *
 * @param to - scroll position
 * @return duration - animation time
 *
 **/
function scrollTo(to, duration) {
    if (duration <= 0 ){
        clearTimeout(timer);
        return;
    }
    var form = document.getElementById('content').getElementsByTagName("form")[0];
    var scrollTop = form.scrollTop //+ document.documentElement.scrollTop;
    var difference = to - scrollTop;
    var perTick = difference / duration * 10;

    timer = setTimeout(function() {
      scrollTop = scrollTop + perTick;
      form.scrollTop = scrollTop;
      //document.documentElement.scrollTop = scrollTop;
      if (scrollTop === to){
          clearTimeout(timer);
          return;
      }
      scrollTo(to, duration - 10);
    }, 10);
  }

/**
 * Show or Hide the scroll to top button.
 *
 * @param void
 * @return void
 *
 **/
function backToTopVisibility(){
        
    //console.log("resize")

    var container = document.getElementById("content") ;
    if(container){
        var form = container.getElementsByTagName("form")[0];
        if(form){
            if(form.scrollTop==0)
                document.getElementById("back-to-top").style.opacity = "0" ;
            else    
                document.getElementById("back-to-top").style.opacity = "1"
        }else{
            document.getElementById("back-to-top").style.opacity = "0" ;   
        }
    }
}


/**
 * Displays Reset Message.
 *
 * @param void
 * @return void
 *
 **/
 function showResetMessage() {
    var banner = document.getElementById("main-header-banner") ;
    if(banner){
        banner.innerHTML = "System Reset, Please wait..." ;        
    }
 }

/**
 * Resets the Remote Setup .
 *
 * @param void
 * @return void
 *
 **/
 function doReset() {
    if (window.confirm("Reset remote system?") == true) {
        //httpGetData("") ;
    }
 }

/**
 * Shows the activity indicator.
 *
 * @param id    - String
 * @param busy  - boolean 
 * 
 * @return void
 *
 **/
function showActivity(id, busy) {
    
    var target = document.getElementById(id) ;
    
    if(target && target.activityPtr){
      var loader = target.activityPtr   ;
      if(busy)       
        loader.className = "loader" ;
      else
          loader.className = "loader inactive" ;
    }
 }
 
/**
 * Checks if control is password
 *
 * @param e    - HTML elememt
 * 
 * @return boolean
 *
 **/
function isPassword(e){
     if(!e)return false ;
     if (e.tagName == "INPUT" && e.type == "password")
         return true ;
     else
         return false ;
}

/**
 * Init Dependency Map
 *
 * @param void
 * @param void
 * 
 * @return void
 *
 **/
function InitDependencyMap() { 
 
                              
    if((Config != null) && (Config.Attributes != null)){
            
        var keys = Object.keys(Config.Attributes); 
    
        keys.forEach(function(key){
            evaluateDependency(key) ;
        });    
    }
    
}

/**
 * Set configuration data (Delta)
 *
 * @param id - control/html element id
 * @param value - control/html element value
 * 
 * @return void
 *
 **/
function setConfigData(id, value) { 
 
    var ext = "" ;
    
	
    if(!id || (typeof value == undefined))return ;
    
    var tempId = id ;
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
                            
    if((Delta != null) && (Config != null) && (Config.Attributes != null)){
        
        var arg = {"MappingId":id,"Value":value} ;
        var changed = false ;
        var cfg = Config.Members;
        var delta = Delta.Members;
        
        var CKEYS = Object.keys(Config.Attributes);    
    
        for(var i=0; i < CKEYS.length; i++){
            var ckey = CKEYS[i];
            //console.log("id :"+ckey+" Value :"+Config.Attributes[ckey])
            
            if(ckey == id){
                
                var DKEYS = Object.keys(Delta.Attributes);
                //check for current Delta entry...
                for(var j=0; j< DKEYS.length; j++){
                    var dkey = DKEYS[j];
                    //check for current Delta entry...
                    if(dkey == id){
                        Delta.Attributes[dkey] = value ;        
                        //delta[j].Value = value ;        
                        changed = true ;
                        break ;
                    }
                }
                
                if(changed == false && Delta.Attributes[ckey] == null){
                    //Create new Delta entry...
                    Delta.Attributes[ckey] = value ; 
                    changed = true ;
                }
                
                break ;
            }
            
        }
            
        //console.log(Delta.Attributes)
        
        if(changed){
            CHANGED = true ;
            updateSetupData(tempId, value) ; 
        }
        
    }
    
}

/**
 * Set the dependency MapTo Attribute Data 
 *
 * @param arg[MapToAttribute]
 * 
 * @return void
 *
 **/
function setDependencyMapToAttribute(MapToAttribute) { 
    
    if(!MapToAttribute)
        return ;
    
    if(typeof DependencyMapTo != 'undefined'){
        if(!DependencyMapTo[MapToAttribute]){
            DependencyMapTo[MapToAttribute] = false ;    
        }
    }
}

/**
 * Creates the dependency map data 
 *
 * @param void
 * 
 * @return void
 *
 **/
function CreateDependencyMap() { 
    
    if(typeof DependencyMapTo == 'undefined'){
       DependencyMapTo = {}  ;
    }
    
    if(typeof DependencyForMap == 'undefined'){
       DependencyForMap = {}  ;
    }
    
    if(AttributeRegistry != null){
                
        if(typeof AttributeRegistry.RegistryEntries == 'object'){
                    
            var dependencies = AttributeRegistry.RegistryEntries.Dependencies ;
            if(dependencies){
                dependencies.forEach(function(item){
                    if(!DependencyForMap[item['DependencyFor']]){
                        DependencyForMap[item['DependencyFor']] = {"Dependencies" :[]} ;
                    }
                    DependencyForMap[item['DependencyFor']]['Dependencies'].push(item['Dependency']) ;
                                    
                    var MapToAttribute = item['Dependency']['MapToAttribute'] ;
                    if(!DependencyMapTo[MapToAttribute]){
                        DependencyMapTo[MapToAttribute] = false ;    
                    }    
                    
                });    
            }
        }
    }
}

function evaluateDependency(id){
    
   if((typeof DependencyForMap == 'undefined') || (typeof DependencyForMap[id] == 'undefined'))return 
         
    var evaluationEntry = DependencyForMap[id] ;
        
    if(evaluationEntry){
        
        var Dependencies = evaluationEntry['Dependencies'] ;
        
        Dependencies.forEach(function(Dependency){
            
            var dependency = Dependency ;
            var expressionEval = [] ; // expression evaluation tracking...
            var MapFrom = dependency['MapFrom'] ;
            
            MapFrom.forEach(function(MapFromItem, index){
                
                var MapFromValue = "" ;
                
                if(MapFromItem["MapFromAttribute"] == "SYSACS"){
                    var control = document.getElementById("SYSACS") ;
                    if(control){
                        MapFromValue = parseInt(control.value) ;         
                    }
                    
                }else{
                    MapFromValue = getConfigData(MapFromItem["MapFromAttribute"]) ;    
                }
                            
                expressionEval[index] = false ; // initialize the current expression evaluation...
                                
                switch(MapFromItem["MapFromCondition"]){
                        
                    case "EQU" :
                        if(MapFromValue == MapFromItem["MapFromValue"]){
                            expressionEval[index] = true ;
                            DependencyMapTo[dependency['MapToAttribute']] = dependency['MapToProperty'] ; //GrayOut, Hidden, etc...    
                        }else{
                            expressionEval[index] = false ;
                            DependencyMapTo[dependency['MapToAttribute']] = "" ; //dependency['false'] ; //GrayOut, Hidden, etc...    
						}        
                        break;    
                        
                    case "GTR" :  
                        if(MapFromValue > MapFromItem["MapFromValue"]){
                            expressionEval[index] = true ;
                            DependencyMapTo[dependency['MapToAttribute']] = dependency['MapToProperty'] ; //GrayOut, Hidden, etc...
                        }else{
                            expressionEval[index] = false ;
                            DependencyMapTo[dependency['MapToAttribute']] = "" ; //dependency['false'] ; //GrayOut, Hidden, etc...    
						}
                        break;    
                }//end-switch
                                
                if(MapFromItem["MapTerms"]){
                                
                    switch(MapFromItem["MapTerms"]){
                        case "AND" :
                            if(expressionEval[index] && expressionEval[index - 1]){
                                DependencyMapTo[dependency['MapToAttribute']] = dependency['MapToProperty'] ; //GrayOut, Hidden, etc...                
                            }else{
                                expressionEval[index] = false ;
                                DependencyMapTo[dependency['MapToAttribute']] = "" ; //dependency['false'] ; //GrayOut, Hidden, etc...    
						    }  
                            break ;

                        case "OR" :
                            if(expressionEval[index] || expressionEval[index - 1]){
                                DependencyMapTo[dependency['MapToAttribute']] = dependency['MapToProperty'] ; //GrayOut, Hidden, etc...                
                            }else{
                                expressionEval[index] = false ;
                                DependencyMapTo[dependency['MapToAttribute']] = "" ; //dependency['false'] ; //GrayOut, Hidden, etc...    
						    }
                            break ;    
                    }//end-switch
                    
                }//end-if (MapFromItem["MapTerms"])
                
            })//end - MapFrom

        });//end Dependencies    
        
    }// end evaluationEntry
}

/**
 * Get default configuration data 
 *
 * @param id - control/html element id
 * 
 * @return void
 *
 **/
function getConfigDefault(id) { 
 
    var ext = "" ;
    var value = null ;
    	
    if(!id)return value ;
    
    var tempId = id ;
    
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
                            
    if((Config != null) && (Config.Members.length > 0) ){
        
        var found = false ;
        var cfg = Config.Members;
                
        for(var i=0; i< cfg.length; i++){
            
            if(cfg[i].MappingId == id){
                value = cfg[i].Value ;
                break ;
            }
        }
    }
    
    return value ;
}


/**
 * Get default/initial configuration data 
 *
 * @param id - control/html element id
 * 
 * @return value - data value
 *
 **/
function getConfigData(id) { 
 
    var ext = "" ;
    var value = null ;
    	
    if(!id)return value ;
    
    var tempId = id ;
    
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
                            
    if((Config != null) && (Config.Attributes != null) && (Delta != null)){
        
        var found = false ;
        //var cfg = Config.Members;
        //var delta = Delta.Members;
        
        var CKEYS = Object.keys(Config.Attributes);    
    
        for(var i=0; i < CKEYS.length; i++){
            var ckey = CKEYS[i];
            //console.log("id :"+ckey+" Value :"+Config.Attributes[ckey])
            
            if(ckey == id){
                
                var DKEYS = Object.keys(Delta.Attributes);
                //check for current Delta entry...
                for(var j=0; j< DKEYS.length; j++){
                    var dkey = DKEYS[j];
                    //check for current Delta entry...
                    if(dkey == id){
                        value = Delta.Attributes[dkey] ;        
                        //value = delta[j].Value ;        
                        found = true ;
                        break ;
                    }
                }
                
                //found in Delta?
                if(found == true)
                    break ;
                
                //value = cfg[i].Value ;
                value = Config.Attributes[ckey] ;
                found = true ;
                break ;
            }
            
        }
        
        //return
                
        
        if(!found)return null ;
    }
    
    updateSetupData(tempId, value) ;  
    
    return value ;
}

/**
 * Special handler Sets Data for Boot Priorities, BBS Priorities etc.
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function specialBootSetData(id){
    
    if(!id)return ;
    
    //Get the HTML control making the request...
    var control = document.getElementById(id) ;
    
    if(control){
        
        var value = "" ;
                
        var selectOptions = control.getElementsByTagName('option');
        for (var i = 0; i < selectOptions.length; i++) {
            var opt = selectOptions[i];
            value += opt.text + "," ; 
            //value +=  opt.value + ","  ;
			value +=  opt.value + ","  ;
            value += (opt.enabled==true)? "true" : "false" ;
            value += ";"
        }
        
        if(1){
            setConfigData(id, value) ;
            showActivity(id, false) ;
            return ;   
        }
    }
}

/**
 * Special handler Gets Data for Boot Priorities, BBS Priorities etc.
 *
 * @param void 
 * 
 * @return void
 *
 **/
function specialBootGetData(id, value){
    
    var ext = "" ;
    
	if(!id || (value === undefined))return
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
    
    //Get the HTML control...
    var control = document.getElementById(id+ext) ;

    if(control){

        // turn off the loader image...
        //if(control.activityPtr)
        //    control.activityPtr.className = "loader inactive" ;

        // Create the XML Options DOMS...
        var BootOptions = value ;

        if(control.options.length == 0){
            // add node options...
			
			// get node options...
			var priorities = BootOptions ;

			priorities = priorities.split(";")

			if(priorities.length){
				control.size = 0 ;
				// create and add options...
				for (var i = 0; i < priorities.length; i++){
					
					if(priorities[i] == "")
						continue
					
					var options = priorities[i].split(",") ;
					if(options.length){
						var option = document.createElement("option") ;
						option.text = options[0] ;
						//option.value = parseInt(options[1]) ;
                        option.value = options[1] ;
						option.enabled = (options[2]=="true")? true : false ;
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
                		if(option.enabled == false){
                            option.className = "InactiveBoot" ;
                            
                            if(priorities.length == 2){
                                control.classList.add('InactiveBoot') ;
                            }
                        }
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
						control.add(option) ;
						control.size += 1 ;
                 	}
				}
				
				assertOptionOrder(id+ext);
				
			}
				
			/*
            for (var i = 0; i < BootOptions.length; i++){
                var option = document.createElement("option") ;
                option.text = BootOptions[i]["BootName"] ;
                option.value = BootOptions[i]["Index"] ;
                option.enabled = (BootOptions[i]["Enabled"]=="TRUE")? true : false ;
                if(option.enabled == false)
                    option.className = "InactiveBoot" ;

                control.add(option) ;
            }

            if(control.className.indexOf("oneOf BootOption")>-1)
                control.size = BootOptions.length ;
			*/	

        }//end-if BootPriority
    }//end-if control
    
}//end specialBootCallbackOnGet

/**
 * Updates SetupData.xml (i.e., XML DOM)
 *
 * @param id - control/html element id
 * @param value - control/html element value
 * 
 * @return void
 *
 **/
function updateSetupData(id, value) { //return
 
    var ext = "" ;
    
	if(!id || (typeof value == undefined))return
    
    var tempId = id ;
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
        
    var target = document.getElementById(id+ext) ;
                                  
    if(target != null){
        //target.value = value ; // set the initial value

        //var pageId = target.ControlPageID ;
        //var guid = target.PlatformGuid ;
        //var control = SetupDataGetControlById(id, guid) ;

        // Update control cache...
        if(1){ 
            switch (target.tagName) {
                case "SELECT" :
					
					if(id == "SETUP006"){
						specialBootGetData(id, value) ;
						return
					}
                    
                    //Get the HTML options...
                    var options = target.getElementsByTagName('option');

                    if(options.length){
                        for (var i = 0; i < options.length; i++){
                            if(options[i].value == value){
                                target.selectedIndex = i ;
                                break ;
                            }//end-if
                        }//end-for    
                    }
                    break ;
                    
                case "INPUT": 
                    if(target.type == "checkbox"){
                        //target.checked = (parseInt(value)==1)?true:false ;
                        target.checked = (value==true)?true:false ;
                        //control.setAttribute("value", parseInt(value)) ;
                    }else if(target.type == "radio"){
                        var index = parseInt(value) ;
                        target.checked = (index==target.index)?true:false ;  
                        //control.setAttribute("value", parseInt(value)) ;
                    }else if(target.type == "number"){
                        //control.setAttribute("value", parseInt(value)) ;
                        target.value = parseInt(value) ; 
                    }else if(target.type == "password"){

                        var hasPassword = parseInt(value) ;

                        if(hasPassword){
                            target.parentNode.style.display = "block" ;
                            //control.setAttribute("hasPassword", hasPassword) ;
                        }else{
                            target.parentNode.style.display = "none" ;
                            //control.setAttribute("hasPassword", 0) ;
                        }


                    }else{
                       // control.setAttribute("value", value) ;
                        target.value = value ; 
                    }
                    break;

                default: 
                    //control.setAttribute("value", value) ;
                    target.value = value ; 
            }//end-switch
        }//end-control

        // Remove activity indicator
        //showActivity(id+ext, false) ;

    }//end-target
}

/**
 * Set Dependency condition
 *
 * @param id - control/html element id
 * @param value - control/html element value
 * 
 * @return void
 *
 **/
function setDepencencyCondition(id) { //return
 
    var ext = "" ;
    
	if(!id || (typeof value == undefined))return
    
    var tempId = id ;
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
        
    var target = document.getElementById(id+ext) ;
                                  
    if(target != null){
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
       if(Browser == 'IE' && target == '[object HTMLUnknownElement]'){
            return ;
        }
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
        if(typeof target.styleNormal === 'undefined'){
            target.styleNormal = target.style ;
        }
        
        var MapToProperty = DependencyMapTo[id] ;
        
        //if(MapToProperty){
            
            switch(MapToProperty){

                case "GrayOut" :
                    target.style.pointerEvents = "none" ;
                    if(target.type == "checkbox"){
                        target.style.opacity = "0" ;
                    }else{
                        target.style.opacity = ".4" ;
                    }
                    target.disabled = true ;
                    break ;

                case "Hidden" :
                    var component = target.closest(".component");
                    if(component){
                        component.style.display = "none" ;
                    }
                    break ;

                case "Disabled" :
                    target.disabled = true ;
                    target.style.pointerEvents = "none" ;
                    var cl = target.classList ;
                    if (cl.contains('Grayout')) {
                        cl.remove('Grayout') ;
                    }
                    break ;

                default:
                    //Enabled...
                    var component = target.closest(".component");
                    if(component){
                        component.style.display = "" ;
                    }
                    //target.style = "" ;
                    
                    target.disabled = false ;
                    if(typeof target.styleNormal !== 'undefined'){
                        target.style = target.styleNormal  ;
                    }else{
                       target.style.opacity = "1" ;
                        target.style.pointerEvents = 'all' ; 
                    }
                    
                    
            }
        //}
    }//end-target
    
    
}


/**
 * Sets the Control Id array
 *
 * @param void 
 * 
 * @return void
 *
 **/
function setControlId(id){
 
 if(typeof ControlStateIDs == 'undefined'){
  ControlStateIDs = [] ;
 }
 
 if(ControlStateIDs.indexOf(id) == -1 ){
  ControlStateIDs.push(id) ;
 }
}

/**
 * Sets All Control States
 *
 * @param void 
 * 
 * @return void
 *
 **/
function setAllControlState(){
 if(typeof ControlStateIDs != 'undefined'){
  if(ControlStateIDs.length){
   for (var i = 0; i < ControlStateIDs.length; ++i) {
    var entry = getRegistryEntry(ControlStateIDs[i]) ;
    if(entry && typeof entry.ReadOnly !== 'undefined' && entry.ReadOnly == true){
        setControlState(ControlStateIDs[i], 'GrayOut') ;
    }
    
   }    
  }
 }        
}



/**
 * Sets the control state 
 *
 * @param id - control or html element id
 * @param state - GrayOut/Hidden/Disabled/Default
 * 
 * @return void
 *
 **/
function setControlState(id, state) { //return
 
    var ext = "" ;
	var target = id ; 
    
	if(!id || (typeof state == undefined))return
    
   	if(typeof id == 'string'){
		
		if(id.indexOf("&") >-1){
			ext = "&" + id.split("&")[1] ;
			id = id.split("&")[0] ;
		}
		target = document.getElementById(id+ext) ;
	}
                                  
    if(target != null){
            
		switch(state){
				
			case "GrayOut" :
				target.style.pointerEvents = "none" ;
				if(target.type == "checkbox"){
                    target.style.opacity = "0" ;
                }else{
                    target.style.opacity = ".4" ;
                }
                /*
                target.onkeypress = function(event){
                    cancelBubble(event) ;
                    return false;
                }
                */
                target.disabled = true ;
                /*
				target.disabled = 1 ;
                
				var cl = target.classList ;
				if (!cl.contains('Grayout')) {
					cl.add('Grayout') ;
				} 
                */
				break ;

			case "Hidden" :
				var component = target.closest(".component");
				if(component){
					component.style.display = "none" ;
				}
				break ;

			case "Disabled" :
				target.disabled = true ;
				target.style.pointerEvents = "none" ;
				var cl = target.classList ;
				if (cl.contains('Grayout')) {
					cl.remove('Grayout') ;
				}
				break ;

			default:
                //Enabled...
                var component = target.closest(".component");
                if(component){
                    component.style.display = "" ;
                }
                target.disabled = false ;
                if(typeof target.styleNormal !== 'undefined'){
                    target.style = target.styleNormal  ;
                }else{
                   target.style.opacity = "1" ;
                    target.style.pointerEvents = 'all' ; 
                }
                /*
                if(typeof target.onkeypress == 'function'){
                    target.onkeypress = null ;
                }*/
        }
    }//end-target
}

function evaluateSysAccess(event){
    
    var target = event.target ;
    
    evaluateDependency(target.id) ;
    setAllDepencencyValues() ;
}

/**
 * Set All Dependency conditions
 *
 * @param id - control/html element id
 * @param value - control/html element value
 * 
 * @return void
 *
 **/
function setAllDepencencyValues(){
    if(typeof DependencyMapTo == 'object'){
        var keys = Object.keys(DependencyMapTo);
        keys.forEach(function(key){
            setDepencencyCondition(key) ;    
        })
        
        var nodes = document.querySelectorAll("fieldset.component input[type=password]");
        
        if(nodes.length){
            for (var i = 0; i < nodes.length; ++i) {
                setDepencencyCondition(nodes[i].id) ;
            }    
        }
	
	setAllControlState() ;
    }
}

/**
 * Sends HttpRequest Get Request
 *
 * @param id - control/html element id
 * 
 * @return void
 *
 **/
function httpGetData(id) { //return
 
    var ext = "" ;
	
    if(!id)return
    
    var saveId = id ;
        
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }   
    
    if(1){// RemoteBios or ManagemenServer Mode, call getConfigData
        getConfigData(saveId) ;
        if(DependencyForMap[id]){
            evaluateDependency(id) ;
            setAllDepencencyValues() ;
        }
        

        showActivity(id+ext, false) ;
        
        var control = document.getElementById(id) ;    
            
        if(control && (typeof control.type != undefined)){
            if(control.type !="password")
                return ;
        }else{
            return 
        }
    }
 
    var url="/SETUP/"+id ;
	var xmlhttp ;
                           
    var target = document.getElementById(id+ext) ;
                                  
    if (window.XMLHttpRequest){// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest() ;
	}else  {// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP") ;
	}
                           
    // Display activity indicator
    showActivity(id+ext, true) ;                       
	
    xmlhttp.onreadystatechange=function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200){
			    			
			//GET response format example -> "id=SETUP000&value=0" ;
			var res = xmlhttp.responseText ;
			//print the response to console
			console.log(res) ;  
			// res=[2], where res[0]="id=SETUP000", res[1]="value=0" ;				
			res = res.split("&") ; 
			// res[0]="id=SETUP000", 				
			var id = res[0].split("=")[1] ;
			// res[1]="value=0" ;				
			var value = res[1].split("=")[1] ;
			
			var target = document.getElementById(id+ext) ;
			if(target!=null){
                //target.value = value ; // set the initial value
                
                var pageId = target.ControlPageID ;
                var guid = target.PlatformGuid ;
                var control = SetupDataGetControlById(id, guid) ;
                
                // Update control cache...
                if(control){ 
                    switch (target.tagName) {
                        case "SELECT" :     
                            target.selectedIndex = parseInt(value) ;
                            control.setAttribute("value", target.selectedIndex) ;
                            break ;
                        case "INPUT": 
                            if(target.type == "checkbox"){
                                target.checked = (parseInt(value)==1)?true:false ;
                                control.setAttribute("value", parseInt(value)) ;
                            }else if(target.type == "radio"){
                                var index = parseInt(value) ;
                                target.checked = (index==target.index)?true:false ;  
                                control.setAttribute("value", parseInt(value)) ;
                            }else if(target.type == "number"){
                                control.setAttribute("value", parseInt(value)) ;
                                target.value = parseInt(value) ; 
                            }else if(target.type == "password"){
                                    
                                var hasPassword = parseInt(value) ;
                                    
                                if(hasPassword){
                                    target.parentNode.style.display = "block" ;
                                    control.setAttribute("hasPassword", hasPassword) ;
                                }else{
                                    target.parentNode.style.display = "none" ;
                                    control.setAttribute("hasPassword", 0) ;
                                }
                                
                                    
                            }else{
                                control.setAttribute("value", value) ;
                                target.value = value ; 
                            }
                            break;

                        default: 
                            control.setAttribute("value", value) ;
                            target.value = value ; 
                    }//end-switch
                }//end-control
                
                // Remove activity indicator
                showActivity(id+ext, false) ;
                
            }//end-target
		}//end-readyState
	}//end-xmlhttp.onreadystatechange
	
	// open and send the request
	xmlhttp.open("GET", url, true) ;
	xmlhttp.send() ;
}

/**
 * Sends HttpRequest Post Request
 *
 * @param id    - control id
 * @param value - control value
 * 
 * @return void
 *
 **/
function httpPostData(id, value){ //return
	
    var ext = "" ;
	
    if(!id || (value==null))return
         
    // RemoteBios or ManagemenServer Mode (i.e., Delta Update), call setConfigData
    if(1){
        setConfigData(id, value) ;
        if(DependencyForMap[id]){
            evaluateDependency(id) ;
            setAllDepencencyValues() ;
        }
        return ;   
    }
    
 }
 

/**
 * Loads the text contents for a given file.
 *
 * @param filename - xml filename
 * @retval string 
 *
 **/
function getContent(filename){
    if (window.XMLHttpRequest)  {
        xhttp=new XMLHttpRequest() ;
    }else{ // code for IE5 and IE6
        xhttp=new ActiveXObject("Microsoft.XMLHTTP") ;
    }
    xhttp.open("GET", filename, false) ;
    xhttp.send() ;
    return xhttp.responseText ;
}


/**
 * Returns the xmldoc for a given xmlstring..
 *
 * @param txt - string of xml
 * @retval object - xml doc 
 *
 **/
function loadXMLString(txt){
    if (window.DOMParser)
      {
      parser=new DOMParser();
      xmlDoc=parser.parseFromString(txt,"text/xml");
      }
    else // code for IE
      {
      xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
      xmlDoc.async=false;
      xmlDoc.loadXML(txt);
      }
    return xmlDoc;
} 


/**
 * Returns the xmldoc for a given xml file.
 *
 * @param filename - name of xml file
 * @retval object - xml doc 
 *
 **/
function loadXMLDoc(filename){
    if (window.XMLHttpRequest)  {
        xhttp=new XMLHttpRequest() ;
    }else{ // code for IE5 and IE6
        xhttp=new ActiveXObject("Microsoft.XMLHTTP") ;
    }
    xhttp.open("GET", filename, false) ;
    xhttp.send() ;
    return xhttp.responseXML ;
}


/**
 * Returns the platform string
 *
 * @param id - String Token
 * @param platformid - GUID
 * @param HiiHandle - Handle
 * @param lang - Language 
 *
 * @retval string 
 *
 **/
function getStringById(id, Guid, HiiHandle, lang){
    var string = null ;
    var language = null ;
    var PlatformStrings = null ;
    var found = false ;
    
    if(!StringPack)
        return null ;
    
    var tempLang = lang?lang : CurrentLanguage ;   
    
    var StringPacks = StringPack.getElementsByTagName("StringPack") ;
    if(StringPacks.length){
        // Search for the matching guid or handle
        for(var i=0; i<StringPacks.length; i++){
            
            var PlatformGuid = StringPacks[i].getAttribute("Guid") ;
            var Handle = StringPacks[i].getAttribute("Handle") ;
            
            if(PlatformGuid == Guid && Handle == HiiHandle ){
                var Strings = StringPacks[i].getElementsByTagName("Strings") ;   
                
                if(Strings.length){
                    for(var j=0; j<Strings.length; j++){
                        language = Strings[j].getAttribute("Language") ;
                        
                        if(tempLang == language){
                            var strings = Strings[j].getElementsByTagName("String") ;
                            
                            if(strings.length){
                                for(var k=0; k<strings.length; k++){
                                    if(id == strings[k].getAttribute("id")){
                                        string = strings[k].getAttribute("value") ;
                                        found = true ;
                                        break ;
                                    }//end-if
                                }//end-for
                                
                                if(!found && !lang){
                                    string = getStringById(id, Guid, HiiHandle, "en-US") ; 
                                    if(string != null){
                                        found = true ; 
                                        break;
                                    }
                                }
                            }//end-strings.length
                        }//end-if tempLang
                        
                        if(found) break ;
                    }//end-for    
                }//end-Strings.length
            }//end-if PlatformGuid
            if(found) break ;
        }//end-for StringPacks.length
    }//end-StringPacks.length
       
    return string ;
}

/**
 * Update the current language
 *
 * @param void 
 * 
 * @return void
 *
 **/
function updateLanguage(id, value){
    
    switch(value) {
        case "en-US"  :
        case "fr-FR"  :
        case "es-ES"  :
        case "de-DE"  :
        case "ru-RU"  :
        case "ko-KR"  :
        case "it-IT"  :
        case "da-DK"  :
        case "fi-FI"  :
        case "nl-NL"  :
        case "no-NO"  :
        case "nb-NO"  :
        case "nn-NO"  :
        case "pt-PT"  :
        case "pt-BR"  :
        case "sv-SE"  :
        case "ja-JP"  :
        case "zh-chs" :
        case "zh-cht" :
            CurrentLanguage = value ; 
            break ;  

        default:
            CurrentLanguage = "en-US" ; 
            break;
    };
    
    var ext = "" ;
    
    if(!id || (value === undefined))return
    
    if(id.indexOf("&") >-1){
        ext = "&" + id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
    
    //Get the HTML control...
    var control = document.getElementById(id+ext) ;
        
    if(control){
        var selectOptions = control.getElementsByTagName('option');
        
        for (var i = 0; i < control.options.length; i++){
            if(control.options[i].Language == value){
                control.selectedIndex = i ;
                break ;
            }//end-if
        }//end-for    
    }
    
    
}

/**
 * GET the current language from Config.JSON
 *
 * @param void 
 * 
 * @return void
 *
 **/
function getLanguage(){
    
    var id = "SETUP000" ;
    
    var value = getConfigData(id) ;
    
    if(value != null){
        updateLanguage(id, value) ;
    }
}

/**
 * Sends http POST request to set the current language
 *
 * @param value - index
 * 
 * @return void
 *
 **/
function setLanguage(value){
    
    var oldLanguage = CurrentLanguage ; 
    
    var url = "ISSPECIAL?id="+SYSTEM_LANGUAGE+"&value=" + value ;
    var xmlhttp = createHttpRequest();
    
    xmlhttp.onreadystatechange=function(){
        if (xmlhttp.readyState==4 && xmlhttp.status==200){
            var res = xmlhttp.responseText ;
            res = res.split("&") ; 
            var id = res[0].split("=")[1] ;
            var ACK = res[1].split("=")[1] ;
            
            if(ACK!="ACK"){
                CurrentLanguage = oldLanguage ;
                reload() ;
                            
                if(CurrentPage != null){ //Show the same page
                    var id = CurrentPage.split("&")[0] ;
                    var guid = CurrentPage.split("&")[1] ;
                    showForm(id, guid );    
                }
            }
        }
    } 
    xmlhttp.open("POST", url, true) ;
    xmlhttp.send() ;
}



/**
 * Updates the page content. 
 * This function is an event handler, 
 * called when the browser's back/forward button is pressed.
 *
 * @param event - object
 * @retval void 
 *
 **/
function updateContent (event) {
    // Check to make sure the state object is not null.
    if (event.state) {
        var params = event.state.split("&") ;
        if(params.length){
            var PageID = params[0] ;
            var Guid = params[1] ;
            var scrollTop = params[2] ;
            
            if((PageID == StartPageId) && (StartPageGuid == Guid)){
                showRootMenu();
                showForm(StartPageId, StartPageGuid);
            }else{
                showForm(PageID, Guid);
            }
            
            /*
            var container = document.getElementById("content") ;
            if(container){
                var form = container.getElementsByTagName("form")[0];
                if(form){
                    form.scrollTop = parseInt(scrollTop) ;
                }
            }
            */
        }
    }// end-if
};


/**
 * Create a new history item to aid browser history management by 
 * dynamically bookmarking or setting the browser hash. 
 * Required to fake the browser history when Back/Forward button is pressed.
 *
 * @param id - PageID
 * @param guid - Platform Guid
 * @retval void 
 *
 **/
function createHistoryItem (id, guid) {
    
    var scrollTop=0 ;
    
    /*
    var container = document.getElementById("content") ;
    if(container){
        var form = container.getElementsByTagName("form")[0];
        if(form){
            scrollTop = form.scrollTop ;
        }
    }
    */
    
    history.pushState(id+"&"+guid+"&"+scrollTop, "", "");
};


/**
 * Gets the control node
 *
 * @param ControlID - Control id
 * @param PlatformGuid - Guid
 * @retval object - xml Control node 
 *
 **/
function SetupDataGetControlById(ControlID, PlatformGuid){
    
    if(!ControlID || !PlatformGuid)return null ;
    
    var control = null ;
        
    var platforms = SetupData.getElementsByTagName("Platform") ;
    
    for(var i=0; i<platforms.length; i++){
        if(PlatformGuid == platforms[i].getAttribute("Guid")){
            platform = platforms[i] ;   
            break ;
        }
    }//end-for
     
    if(platform){
        control = null ;
        var controls = platform.getElementsByTagName("Control") ;
        if(controls.length){
            for(var i=0; i<controls.length; i++){
                if(ControlID == controls[i].getAttribute("ControlID")){
                    control = controls[i] ;
                    break ;
                }
            }//end-for
        }//end-if
        
    }//end-if
    
    return control ;
}


/**
 * Gets the page node
 *
 * @param id - PageID
 * @param guid - Platform Guid
 * @retval object - xml Page node 
 *
 **/
function SetupDataGetPageById(PageID, PlatformGuid){
    
    if(!PageID || !PlatformGuid)return null ;
    
    var page = null ;
    var platform = null ;
    
    var platforms = SetupData.getElementsByTagName("Platform") ;
    
    for(var i=0; i<platforms.length; i++){
        if(PlatformGuid == platforms[i].getAttribute("Guid")){
            platform = platforms[i] ;   
            break ;
        }
    }//end-for
     
    if(platform){
        page = null ;
        var pages = platform.getElementsByTagName("Page") ;
        if(pages.length){
            for(var i=0; i<pages.length; i++){
                if(PageID == pages[i].getAttribute("PageID")){
                    page = pages[i] ;
                    break ;
                }
            }//end-for
        }//end-if
        
    }//end-if
    
    return page ;
}

/**
 * Sets the color of the active/selected menu item (sidebar and dropdown).
 *
 * @param id - PageID
 * @param guid - Platform Guid
 * @param subPage - boolean
 * @retval void 
 *
 **/
function highlightMenuLink(id, guid, subPage){
                            
    var links = document.getElementsByTagName("a") ;
    
    var isRootMenu = false ;
    //var link = null ;
    
        
    // Highlight the currently active/selected menu item
    for(var i=0; i< links.length; i++){
        if(links[i].isRootMenuLink){
            var PageId = links[i].href.substr(links[i].href.lastIndexOf("#")+1) ;
            var Guid = links[i].getAttribute("PlatformGuid") ;
			var refguid = links[i].getAttribute("RefGuid") ; 
			if(refguid != "" && refguid != null)
				Guid = refguid ; 
            var cl = links[i].classList ;
            if(id == PageId && guid == Guid){
                isRootMenu = true ;
                cl.remove('back') ;
                //sublink = links[i] ;
                if (cl.contains('selected')==false) {
                    cl.add('selected') ;
                    if(subPage)
                        cl.add('back') ;
                }//end-if
            }else{
                if (cl.contains('selected')) {
                    cl.remove('selected') ;
                    cl.remove('back') ;
                   
                }
            }
        }//end-if
    }// end-for
    
    // The current page/link is NOT a root page,
    // get the root page, highlight the menu link... 
    if(isRootMenu == false) {
		var page = SetupDataGetPageById(id, guid);
        page = getRootPage(page) ;
				        
        if(page){
			var refguid = page.getAttribute("RefGuid") ;
			var tempGuid = (refguid != "" && refguid != null)? refguid : guid ;
            highlightMenuLink(page.getAttribute("PageID"), tempGuid, true);
        }
    }
    
}



/**
 * Displays the HTML content for the xml page node.
 *
 * @param id - PageID
 * @param guid - Platform Guid
 * @retval void 
 *
 **/
function showForm(id, guid){
    
    if(!id)return
    if(id.indexOf("#") >-1){
        id = id.substr(id.lastIndexOf("#")+1) ;
    }
    
    var platform = null ;
    var platforms = SetupData.getElementsByTagName("Platform") ;
    
    for(var i=0; i<platforms.length; i++){
        var PlatformGuid = platforms[i].getAttribute("Guid") ;

        if(PlatformGuid == guid){
            platform = platforms[i] ;   
            break ;
        }
    }//end-for
     
    if(platform){
        var page = null ;
        var pages = platform.getElementsByTagName("Page") ;
        if(pages.length){
            for(var i=0; i<pages.length; i++){
                if(id == pages[i].getAttribute("PageID")){
                    page = pages[i] ;
                    break ;
                }
            }//end-for
        }//end-if
    
        if(page){
            // Get the HTML element which will contain the form data (i.e, controls)
            var container = document.getElementById("content") ;
            if(container){
                
                if(typeof ControlStateIDs != 'undefined'){
                    ControlStateIDs = [] ;
                }
                container.innerHTML = "" ;
                HiiParser.parseChildren(page, container) ;
                CurrentPage = id+"&"+guid ;
                highlightMenuLink(id, guid) ;
                
                /*
                var SYSACS = document.getElementById('SYSACS') ;
                if(SYSACS){
                    SYSACS.dispatchEvent(new Event('change'));    
                }else{
                       
                }
                */
                
                setAllDepencencyValues() ; 

                /*
                var form = container.getElementsByTagName("form")[0];
                form.addEventListener("scroll", backToTopVisibility) ;
                backToTopVisibility();
                */
                if(id == "AMIDIAG"){
                    HiiParser.createDiagTemplate() ;
                }
                
            }else{
                throw("HTML Element with id='content' not found.") ;
            }
        }//end-if
    }//end-if
}



/**
 * Changes the color scheme for the HTML Page.
 *
 * @param void
 * @retval void 
 *
 **/
toggleColorScheme = function(){
    theme += 1 ;
    
    if(theme >= colorSchemes.length)
        theme = 0 ;
    
    document.body.className = colorSchemes[theme].option ;   
}


/**
 * Changes the style for HTML checkbox controls.
 *
 * @param void
 * @retval void 
 *
 **/
toggleCheckBoxStyle = function(){
    checkBoxStyle += 1 ;

    if(checkBoxStyle >= checkBoxStyleOptions.length)
        checkBoxStyle = 0 ;

    if(CurrentPage != null){ //Show the same page
        var id = CurrentPage.split("&")[0] ;
        var guid = CurrentPage.split("&")[1] ;
        var page = SetupDataGetPageById(id, guid) ;

        // reload to show updated checkbox style...
        if(page && page.getAttribute("hasCheckBox")=="true"){
            showForm(id, guid );    
        }
    }
}

/**
 * Handles key events.
 *
 * @param evt - event object
 * @retval void 
 *
 **/
KeyDownHandler = function(evt){
    
    //Ctrl + ?  keys
    if(/* evt.ctrlKey && */ evt.keyCode == 115){ 
        
        var SYSACS = document.getElementById('SYSACS') ;
        if(SYSACS){
            
            var component = SYSACS.closest(".combo");
            if(component){
                
                if(component.style.display == "none"){
                    component.style.display = "" ;    
                }else{
                    component.style.display = "none" ;
                }
            }
        } 
    }  
    
    if(CHANGED && evt.keyCode == 119){ 
        //updateResource() ;
        //CHANGED = false ;
    }    

    if(evt.ctrlKey && evt.keyCode == 119){ 
        toggleColorScheme() ;
    }
    
    if(evt.ctrlKey && evt.keyCode == 120){ 
        toggleCheckBoxStyle() ;
    }

    switch(evt.keyCode)   {
        case 37: break ;  // left
        case 38: 		  // up
            //Limit input through up arrow
            if(evt.target.type == "number") {
                if(evt.target.value >= parseInt(evt.target.max))
                    evt.target.value = parseInt(evt.target.max)-1;
            }
            break ;  
        case 39: break ;  // right
        case 40: 		  // down 
            //Limit input through down arrow
            if(evt.target.type == "number") {
                if(evt.target.value <= parseInt(evt.target.min))
                    evt.target.value = parseInt(evt.target.min)+1;
            }
            break ; 
        case 27: break ;  // esc
        default:
            //console.log(evt.target.value)
            if(evt.target.type == "number") {
                //Dont allow alphabets except backspace and "-" if minimum is less than 0.
                if( !(
                        (evt.keyCode >= 48 && evt.keyCode <= 57)                ||  //normal digits
                        (evt.keyCode >= 96 && evt.keyCode <= 105)               ||  //numpad digits
                        (evt.keyCode == 8)                                      ||  //backspace
                        (evt.keyCode == 46)                                     ||  //delete 
                        ((parseInt(evt.target.min)<0) && (evt.keyCode == 109))  ||  //numpad "-"
                        ((parseInt(evt.target.min)<0) && (evt.keyCode == 173))      //normal "-"
		              )) 						
                {
                    evt.preventDefault();
                }
                
            }
            
            break;
    };
};


/**
 * Handles Mousewheel events.
 *
 * @param e - event object
 * @retval void 
 *
 **/
function mouseWheelEvent (e) {
    var e = window.event || e; // old IE support
    e.preventDefault();
    var delta = e.wheelDelta ? e.wheelDelta : -e.detail;
    this.scrollLeft -= (delta * 5);
}


/**
 * Create and display the root menus in the sidebar, header, and dropdown area..
 *
 * @param void
 * @retval void 
 *
 **/
function showRootMenu(id){
    
    var pages = SetupData.getElementsByTagName("Page") ;
    var container = document.getElementById("content") ;
    var RootMenu = document.getElementById("RootMenu") ;
    var PageMenu = document.getElementById("PageMenu") ;
    var SideNavMenu = document.getElementById("SideNavMenu") ;
    var right_arrow = document.getElementById("right-arrow") ;
    var left_arrow = document.getElementById("left-arrow") ;
    
    if(pages.length && container && RootMenu && SideNavMenu){
        
        if (document.removeEventListener) {
            // IE9, Chrome, Safari, Opera
            RootMenu.removeEventListener("mousewheel", mouseWheelEvent, false);
            // Firefox
            RootMenu.removeEventListener("DOMMouseScroll", mouseWheelEvent, false);
        }
        
        //container.innerHTML = "" ;
        RootMenu.innerHTML = "" ; 
        PageMenu.innerHTML = "" ;
        SideNavMenu.innerHTML = "" ;
        
        StartPageId = "" ;
        StartPageGuid = "" ;
        
        for(var i=0; i<pages.length; i++){
            var page = pages[i] ;
            if(page){
                
                var PageID = page.getAttribute("PageID") ;
                var PlatformGuid = page.getAttribute("PlatformGuid") ;
                var PageParentID = parseInt(page.getAttribute("PageParentID")) ;
                var PageFlags = parseInt(page.getAttribute("PageFlags")) ;
                
                if((PageParentID == 0) && ((PageFlags & 0x1) == 0)){
                    HiiParser.createRootMenu(page, RootMenu) ;
                    HiiParser.createRootMenu(page, SideNavMenu) ;
                    HiiParser.createRootMenu(page, PageMenu) ;
                    
                    if(StartPageId == ""){
                        StartPageId = PageID ;
                        StartPageGuid = PlatformGuid ;
                    }
                        
                }
            }
        }
        
        
        var banner = document.getElementById("main-header-banner") ;
        banner.innerHTML = /* document.title = */ "Setup" ;
                
        container.innerHTML = "" ;
                            
        if (document.addEventListener) {
            // IE9, Chrome, Safari, Opera
            RootMenu.addEventListener("mousewheel", mouseWheelEvent, false);
            // Firefox
            RootMenu.addEventListener("DOMMouseScroll", mouseWheelEvent, false);
            
            RootMenu.scrollLeft = 0 ;
            
            /*
            PageMenu.addEventListener("DOMMouseScroll", function(e){
                var hist = 0 ;
                var e = window.event || e; // old IE support
                e.preventDefault();
                var delta = e.wheelDelta ? e.wheelDelta : -e.detail;
                hist -= (delta);
                
                console.log(history)
                
                if(history.length>0 && hist>0)
                    window.history.back();
                else if(history.length>0 && hist<0)
                    window.history.forward();
            
            
            }, false);
            
            */
        }
        
        if (right_arrow) {
            right_arrow.addEventListener("click", function(){
                RootMenu.scrollLeft += (10 * 5);
            }, false);
        }
        
        if (left_arrow) {
            left_arrow.addEventListener("click", function(){
                RootMenu.scrollLeft -= (10 * 5);
            }, false);
        }
    }//end-if
    
}


/**
 * Returns the root page for a given page node.
 *
 * @param node - xml node
 * @retval object - xml node 
 *
 **/
function getRootPage(node){
    
    if(!node) return null ;
    
    var page = node ;
        
    if(page.getAttribute("isRootPage") == "false"){
        var PageParentID = page.getAttribute("PageParentID") ;
        var PlatformGuid = page.getAttribute("PlatformGuid") ;
		var refguid = page.getAttribute("RefGuid") ;
		var tempGuid = (refguid != "" && refguid != null)? refguid : PlatformGuid ; 
        page = getRootPage(SetupDataGetPageById(PageParentID, tempGuid)) ;    
    }
    
    return page ;
}


/**
 * Sets the PlatformGuid for all pages and controls.
 *
 * @param void
 * @retval void
 *
 **/
function initPageGuids(){
    var platforms = SetupData.getElementsByTagName("Platform") ;
    for(var i=0; i<platforms.length; i++){
        var PlatformGuid = platforms[i].getAttribute("Guid") ;
        var Handle = platforms[i].getAttribute("Handle") ;

        if(PlatformGuid == null){
            alert("ERROR!!!! invalid or missing PlatformGuid...") ;
            return ;
        }
        
        if(Handle == null){
            alert("ERROR!!!! invalid or missing Handle...") ;
            return ;
        }
        
        var pages = platforms[i].getElementsByTagName("Page") ;
        if(pages.length){
            for(var j=0; j<pages.length; j++){
                    var PageParentID ;
                    pages[j].setAttribute("PlatformGuid", PlatformGuid) ;
                    pages[j].setAttribute("isRootPage", false) ;
                    pages[j].setAttribute("Handle", Handle) ;
                
                    PageParentID = pages[j].getAttribute("PageParentID") ;
                    
                    if(PageParentID){
                        if(parseInt(PageParentID)===0)
                            pages[j].setAttribute("isRootPage", true) ;
                    }
                
                    var controls = pages[j].getElementsByTagName("Control") ;
                    if(controls.length){
                        for(var k=0; k<controls.length; k++){
                            if(!controls[k].hasAttribute("PlatformGuid")){
                                controls[k].setAttribute("PlatformGuid", PlatformGuid) ;
                            }
                            
                            if(!controls[k].hasAttribute("ControlPageID")){
                                controls[k].setAttribute("ControlPageID", pages[j].getAttribute("PageID")) ;
                            }
                            
                            if(!controls[k].hasAttribute("Handle")){
                                controls[k].setAttribute("Handle", Handle) ;
                            }
                            
                        }//end-for
                    }//end-if
            }//end-for        
        }
    }//end-for
}  


ReadSessionStorage = function (key){
    var value = null ;
    if(key !=null){
        value = sessionStorage.getItem(key) ;
        if(value != null)
            value = JSON.parse(value) ;
    }
    
    return value ;
};
 
WriteSessionStorage = function(key, obj) {
    if(key && obj)
        sessionStorage.setItem(key, JSON.stringify(obj)) ;
};


/**
 * Reloads and re-process the xml Setupdata for the page.
 *
 * @param init - boolean
 * @retval void
 *
 **/
reload = function(init){ 
             
    if(SetupData /*&& StringPack*/){
               
        var pages = SetupData.getElementsByTagName("Page") ;  
        
        if(!pages.length){
            alert("no available pages") ;
            return
        }else{
            showRootMenu() ;
            if(init){
                //history.pushState(null, null, "index.html") ;
                createHistoryItem(StartPageId, StartPageGuid, "") ;
                showForm(StartPageId, StartPageGuid );
            }
        }  
    }else{
        alert("no available data") ;
        return
    }
    
}


// Spinning activity indicator options 
var opts = {
      lines: 13             // The number of lines to draw
    , length: 30            // The length of each line
    , width: 8             // The line thickness
    , radius: 20            // The radius of the inner circle
    , scale: 1.25           // Scales overall size of the spinner
    , corners: 1            // Corner roundness (0..1)
    , color: '#000'         // #rgb or #rrggbb or array of colors
    , opacity: 0.25         // Opacity of the lines
    , rotate: 0             // The rotation offset
    , direction: -1         // 1: clockwise, -1: counterclockwise
    , speed: 1              // Rounds per second
    , trail: 100            // Afterglow percentage
    , fps: 20               // Frames per second when using setTimeout() as a fallback for CSS
    , zIndex: 2e9           // The z-index (defaults to 2000000000)
    , className: 'spinner'  // The CSS class to assign to the spinner
    , top: '50%'            // Top position relative to parent
    , left: '50%'           // Left position relative to parent
    , shadow: false         // Whether to render a shadow
    , hwaccel: false        // Whether to use hardware acceleration
    , position: 'absolute'  // Element positioning
}


/**
 * Creates and displays the page loading animation.
 *
 * @param void
 * @retval void
 *
 **/
function createLoader() {
    var container = document.getElementById("content") ;
    var spinner = new Spinner(opts).spin()
    container.appendChild(spinner.el)
}

/**
 * decodeEntities
 *
 * Converts HTML Character Entities back to regular text
 *
 * @param s - string (containing HTML Character Entities) to replace
 * @retval str - string
 *
 **/
function decodeEntities(s){
    var str ;
    var temp = document.createElement('p') ;
    temp.innerHTML = s;
    str = temp.textContent || temp.innerText ;
    temp = null;
    return str;
}

/**
 * Escapes XML entities.
 * Characters like "<" and "&" inside an XML element will generate an error in the parser.
 * Replace the character with an XML entity reference.
 *
 * @param unsafe - string (containing illegal characters) to replace
 * @retval string
 *
 **/
function escapeXml(unsafe) {
    return unsafe.replace(/[<>&'"]/g, function (c) {
        switch (c) {
            case '<': return '&lt;';
            case '>': return '&gt;';
            case '&': return '&amp;';
            case '\'': return '&apos;';
            default:
                return "";
            //case '"': return '&quot;';
        }
    });
}
   
Browser = (function(){
  var ua = window.navigator.userAgent
  var khtml = ua.match(/KHTML/)
  var gecko = ua.match(/Gecko/)
  var webkit = ua.match(/WebKit\/\d+/)
  var ie = ua.match(/Explorer/) || ua.match(/Trident/) // To Fix IE masquerading as someone else (i.e, Gecko)
  if (ie) return 'IE'
  if (khtml) return 'KHTML'
  if (gecko) return 'Gecko'
  if (webkit) return 'Webkit'
  //if (ie) return 'IE'
  return 'UNKNOWN'
})()



/**
 * For getting RemoteBios Resources 
 *
 * @param void 
 * 
 * @return void
 *
 **/
getRemoteBiosResources = function(){
    
    
    
    if(typeof Resources === "object" ){
        
        if(typeof Resources === "object" && typeof Resources.Members != undefined && Resources.Members.length > 0){
            var Members = Resources.Members ;

            for(var i=0; i< Members.length; i++){

                if(Members[i].type == "Application")
                    continue ;

                var url = "" ;
                                
                if(Members[i].type == "Config"){
                    var url =  window.location.href  ;
                    url = url.substring(0, url.lastIndexOf("/"));

                    if(url.toLowerCase().indexOf("bios")> -1){
                        url = url.substring(0, url.lastIndexOf("/")) ;
                    }
                    
                    url += "/redfish/v1/Systems/Self/Bios/" ;
                }
                
                url += Members[i].name ;
                
                var xmlhttpReq = createHttpRequest();
                xmlhttpReq.open("GET", url, true) ;
                xmlhttpReq.setRequestHeader("Content-Type","application/json") ;
                xmlhttpReq.resourceType = Members[i].type ;
                xmlhttpReq.onreadystatechange = processResource ;
                xmlhttpReq.send() ;
            }//end-for
        }//end-if
    }//end-if
};


/**
 * Special Callback handler for getting Resources.json [HTTP_GET Responce]
 *
 * @param void 
 * 
 * @return void
 *
 **/
getResources = function(){
    
   if(typeof Resources === "object" && isValidIp){
        //res = JSON.parse(res.content);

        if(typeof Resources === "object" && typeof Resources.Members != undefined && Resources.Members.length > 0){
            var Members = Resources.Members ;

            for(var i=0; i< Members.length; i++){

                if(Members[i].type == "Application")
                    continue ;
                                        
                var url = Members[i].name ;
                
                var xmlhttpReq = createHttpRequest();
                xmlhttpReq.open("GET", url, true) ;
                xmlhttpReq.setRequestHeader("Content-Type","application/json") ;
                //xmlhttpReq.setRequestHeader("Content-Length", params.length) ;
                
                xmlhttpReq.resourceType = Members[i].type ;
                xmlhttpReq.onreadystatechange = processResource ;
                                
                xmlhttpReq.send() ;
            }//end-for
        }//end-if
    }//end-if
};


/**
 * Special Callback handler for getting files contained in Resources.json [HTTP_GET Responce]
 *
 * @param void 
 * 
 * @return void
 *
 **/
processResource = function(){
    
    var xmlhttp = this ;
    
    console.log("xmlhttp.readyState: "+xmlhttp.readyState +"   xmlhttp.status: "+xmlhttp.status)
    
    if (xmlhttp.readyState==4 && xmlhttp.status==200){
        var res = xmlhttp.responseText ;
        
        switch(xmlhttp.resourceType) {
            case "AttributeRegistry" :
                AttributeRegistry = JSON.parse(res);
                CreateDependencyMap() ;
                InitDependencyMap() ;
                break ;
            case "Layout"   : 
                //SetupData = JSON.parse(res);
                SetupData = res   ; 
                break ;
            case "Config"   : 
                Config = JSON.parse(res);
                
                var url = BMCADDRESS + Config.AttributeRegistry+".json" ;
                
                var xmlhttpReq = createHttpRequest();
                xmlhttpReq.resourceType = "AttributeRegistry"  ;
                xmlhttpReq.onreadystatechange = processResource ;
                xmlhttpReq.open("GET", url, true) ;
                xmlhttpReq.send() ;
                break ;    
                
            default:
        };
        
        // Check that all resources are loaded...
        if((typeof SetupData == 'string') && (Config !== null) && (AttributeRegistry !== null)){
            Delta.AttributeRegistry = {} ;
            proceedToLoad() ;
        }
    }//end-if xmlhttp.readyState
};

/**
 * resetChangeStatus 
 *
 * @param void
 * 
 * @return void
 *
 **/
resetChangeStatus = function(){
    var saved = document.getElementById("Save Changes") ;
    if(saved){
        var cl = saved.classList ;
        if (!cl.contains('off')) {
            cl.add('off') ;
        }  
    }

    var discard = document.getElementById("Discard Changes") ;
    if(discard){
        var cl = discard.classList ;
        if (!cl.contains('off')) {
            cl.add('off') ;
        }  
    }
}

/**
 * discardPasswordData 
 *
 * @param void
 * 
 * @return void
 *
 **/
function discardPasswordData() { 
                                 
    if((Authentication != null) && (typeof Authentication.Attributes != undefined)){
        Authentication.Attributes = [] ;
        resetChangeStatus() ;
    }//end-if
}

/**
 * getPasswordData 
 *
 * @param id - html element id
 * 
 * @return void
 *
 **/
function getPasswordData(id) { 
     
    if(!id)return ;
    
    var tempId = id ;
    var ext = "" ;
    
    if(id.indexOf("&") >-1){
        ext = id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
                            
    if((Authentication != null) && (Authentication.Attributes != null)){
        var found = false ;
        var value = "" ;
        
        var AUTHKEYS = Authentication.Attributes ;
        //check for current Authentication entry...
        for(var i=0; i< AUTHKEYS.length; i++){
            var authkey = AUTHKEYS[i];

            if(authkey["PasswordName"] == id){
                if(ext){
                    if(Authentication.Attributes[i][ext+"Password"] != undefined){
                        value = Authentication.Attributes[i][ext+"Password"] ;        
                        found = true ;    
                    }
                }    
                break ;
            }//end-if
        }//end-for

        if(found === true){
            //updatePasswordData(tempId, value) ; 
            var target = document.getElementById(tempId) ;
                                  
            if(target != null){
                target.value = value ;
            }
        }//end-if
    }//end-if
    
}

/**
 * setPasswordData 
 *
 * @param id - control/html element id
 * @param value - control/html element value
 * 
 * @return void
 *
 **/
function setPasswordData(id, value) { 
 
    if(!id || (typeof value == undefined))return ;
    
    var tempId = id ;
    var ext = "" ;
    
    if(id.indexOf("&") >-1){
        ext = id.split("&")[1] ;
        id = id.split("&")[0] ;
    }
                            
    if((Authentication != null) && (Authentication.Attributes != null)){
        
        var arg = { "PasswordName" : id} ;
        var changed = false ;
        
        var AUTHKEYS = Authentication.Attributes ;
        //check for current Authentication entry...
        for(var i=0; i< AUTHKEYS.length; i++){
            var authkey = AUTHKEYS[i];

            if(authkey["PasswordName"] == id){
                if(ext){
                    Authentication.Attributes[i][ext+"Password"] = value ;        
                    changed = true ;
                }    
                break ;
            }//end-if
        }//end-for

        if(changed == false){
            //Create new Authentication entry...

            if(ext){
                arg[ext+"Password"] = value ;
            }

            Authentication.Attributes[Authentication.Attributes.length] = arg ; 
            changed = true ;
        }//end-if
        
        //console.log(Authentication.Attributes)
        
        if(changed){
            CHANGED = true ;
        }//end-if
        
    }
}

discardChanges = function(){
            
    discardPasswordData() ;
        
    if((Delta != null) && (Delta.Attributes != null)){
        
        // Make a JSON copy...
        var dCopy = JSON.parse(JSON.stringify(Delta));
        
        // Clear Delta attributes...
        Delta.Attributes = {} ;
        
        var DKEYS = Object.keys(dCopy.Attributes);
        //get copied Delta entries...
        for(var i=0; i< DKEYS.length; i++){
            var id = DKEYS[i];
            //Set default value for the Delta entry...
            getConfigData(id) ;
        }
        
        dCopy = null ;
        resetChangeStatus() ;
    }
        
    CHANGED = false ;
}

updatePasswordResource = function(){
    
    if((Authentication != null) && (Authentication.Attributes != null)){
        
        var AUTHKEYS = Authentication.Attributes ;
        //check for current Authentication entries...
        for(var i=0; i< AUTHKEYS.length; i++){
            var authkey = AUTHKEYS[i];
            
            if( (authkey != null) && (typeof authkey == "object") ){
                
                if(authkey.OldPassword == undefined)
                    authkey['OldPassword'] = '' ;

                if(authkey.NewPassword == undefined)
                    authkey['NewPassword'] = '' ;
                
                var params = JSON.stringify(authkey, null, 2);
                
                var url = "/redfish/v1/Systems/Self/Bios/Actions/Bios.ChangePassword" ;
                var xmlhttp = createHttpRequest();

                xmlhttp.open("POST", url, true) ;
                //xmlhttp.setRequestHeader("Content-Length", params.length) ;
                xmlhttp.setRequestHeader("Content-Type","application/json") ;
                xmlhttp.onreadystatechange = function(){/* Do Nothing */
                    if ((xmlhttp.readyState==4) && (xmlhttp.status==200 || xmlhttp.status==204)){//<kuaibai002-20180813 Fix OOB web cannot save changes>
                        CHANGED = false ;
                        resetChangeStatus() ;
                    }
                }
                xmlhttp.send(params) ;
            }//end-if
        }//end-for
    }//end-if
};

updateResource = function(){
    
    updatePasswordResource() ;
                
    if( (Delta != null) && (typeof Delta == "object") ){
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
        var tempCopy = JSON.parse(JSON.stringify(Delta, null, 2)) ; // make a copy...
        
        if(typeof tempCopy.AttributeRegistry != 'undefined'){
            delete tempCopy.AttributeRegistry ;  // remove attributeRegistry
        }
        
        if(Object.keys(tempCopy.Attributes).length == 0){ // return if there are no attributes
            return
        }
        var content = JSON.stringify(tempCopy, null, 2);
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
    }
    
    var params = content;
    var url = "/redfish/v1/Systems/Self/Bios/SD" ;
    var xmlhttp = createHttpRequest();
    
    xmlhttp.open("POST", url, true) ;
    //xmlhttp.setRequestHeader("Content-Length", params.length) ;
    xmlhttp.setRequestHeader("Content-Type","application/json") ;
    xmlhttp.onreadystatechange = function(){/* Do Nothing */
        
        if ((xmlhttp.readyState==4) && (xmlhttp.status==200 || xmlhttp.status==204)){
            CHANGED = false ;
            resetChangeStatus() ;
        }
    }
    xmlhttp.send(params) ;
};




/**
 * Callback Reloads and re-process the xml Setupdata for the page.
 *
 * @param init - boolean
 * @retval void
 *
 **/

var jsondata = "" ;

function createJsonTree(){
    
    //jsondata = getContent('BiosAttributeRegistry.json') ;
    
    if(SetupData != ""){
        
        //jsondata = JSON.parse(jsondata) ;
        
        if(SetupData && typeof SetupData == 'object'){
            
            //alert("Oh Yeah")
            
            //alert(jsondata.RegistryEntries.Attributes.length)
            
            var attributes = SetupData.RegistryEntries.Attributes ;
            
            var container = document.getElementById("content") ;
            if(container){
                container.innerHTML = "" ;
                
                // create HTML Form Element
                var e = document.createElement("FORM") ;
            
                if(e){
                    container.appendChild(e);
                    
                    //CurrentPage = id+"&"+guid ;
                    //highlightMenuLink(id, guid) ;
                
                    for(var i=0; i<attributes.length; i++){
                        HiiParser.parseChildren(attributes[i], e) ;
                    }//end-for

                    var form = container.getElementsByTagName("form")[0];
                    form.addEventListener("scroll", backToTopVisibility) ;
                    backToTopVisibility();
                    
                }
                
                
            }
            
            
        }
        
    }
    
}

function proceedToLoad() {
    
    
    //getLanguage() ;
    
    //SetupData = "SetupData.xml" ;
    //StringPack = "StringPack.xml" ;
    
    // Send Request to get data from the Web Server...
    
    /*
    StringPack = getContent(StringPack) ;
    SetupData = getContent(SetupData) ;
    */
    
    // "S C R U B B I N G . . .for illegal characters"
    SetupData = SetupData.replace(/value=["|']([^"|']+)["|']/g, function (scrub){
        var clean = scrub.replace(/([^"|']+)([^"|'])/g, escapeXml) ;
        //console.log(clean); 
        return clean ;
    });
            
    // "S C R U B B I N G . . .for illegal characters"
    /*
    StringPack = StringPack.replace(/value=["|']([^"|']+)["|']/g, function (scrub){
        var clean = scrub.replace(/([^"|']+)([^"|'])/g, escapeXml) ;
        return clean ;
    });
    */
    
    // Create the XML DOMS...
    StringPack = false ; //loadXMLString(StringPack) ;
    SetupData = loadXMLString(SetupData) ;
    
    if(!SetupData){
        alert("error loading files") ;
        return
    }
    
    
    //alert(SetupData)
    
    //Set PlatformGuids for each page... 
    initPageGuids() ;
        
    //sessionStorage.clear() ;	  
    reload(true) ;
}



function proceedToLoad_Org() {
    //alert("Got it")
    createJsonTree() ;
}


/**
 * E N T R Y  P O I N T . . . L O A D I N G...
 *
 * This is the Page load entry point
 *
 * @param void
 * @retval void
 *
 **/
window.onload = function(){
    
    StartPageId = "" ;
    StartPageGuid = ""
    CurrentPage = null ;
    urlParams = null ;
    urlBase = "" ;
    
    SetupData = null ;
    StringPack = null ;
    Config = null ;
    isRemoteBios = true ;
    
    //var v = document.getElementById('nav_address').value = "" ;
        
    urlParams = location.search.split("?");
    
    if(urlParams.length > 1){
        
        urlParams   = urlParams[1].split("&");
        
        for(var i=0; i< urlParams.length; i++){
            
            var name = urlParams[i].split("=")[0] ;
            var value = urlParams[i].split("=")[1] ;
            
            switch (name) {
                case "id" : 
                    urlBase = value ;
                    isRemoteBios = false ;
                    break ;
                    
                case "doRemoteBios" : 
                    Delta.RemoteBiosModeOnReStart = (value=="true")?"TRUE":"FALSE" ;
                    break ;
                
                default: 
                    
            }
        }
        
        /*
        if(urlParams[0].indexOf("id") > -1 ){
            //throw("UrlParam ?id not found.") ;
            //return ;
            urlBase = urlParams[0].split("=")[1] ;    
            isRemoteBios = false ;
        }
        */
    }
      
      
    theme = 0 ;
    checkBoxStyle = 0 ;
    checkBoxStyleOptions = [
        {option:"switch"},
        {option:"checkbox"}
    ] ;
    
    colorSchemes = [
        {option:"dark"},
        {option:"light"}
    ] ;
    
    
    // Listen for Back/Forward button press, handle page update... 
    window.addEventListener("popstate", updateContent) ;
            
    var slideMenuButton = document.getElementById('hamburger-menu-button');
	slideMenuButton.onclick = function(e) {
		var site = document.getElementById('site') ;
        var SideNavMenu = document.getElementById('sideNav').getElementsByTagName("nav")[0] ;
        
        var cl = SideNavMenu.classList ;
        if (cl.contains('open')) {
			cl.remove('open') ;
		} else {
			cl.add('open') ;
		}
        
		cl = site.classList ;
		if (cl.contains('open')) {
			cl.remove('open') ;
		} else {
			cl.add('open') ;
		}
        
        cl = this.classList ;
        if (cl.contains('active')) {
            cl.remove('active') ;
            cl.remove('hover') ;
        }else{    
            cl.add('active') ;
        }
	};
    
    document.addEventListener("keydown", KeyDownHandler) ;
    window.addEventListener("resize", backToTopVisibility, false);
    
    var BackToTop = document.getElementById('back-to-top');
    
    BackToTop.addEventListener("click", function(){
        scrollTo(0, 300);
    }) ;
    
    backToTopVisibility();
        
    BMCADDRESS = "/redfish/v1/Registries/" ;
        
    getRemoteBiosResources();
        
}

function moveUp(selectId) {
    var changed = false ;
	var selectList = document.getElementById(selectId);
	var selectOptions = selectList.getElementsByTagName('option');
	for (var i = 1; i < selectOptions.length; i++) {
		var opt = selectOptions[i];
        var enabled = opt.enabled ;
		if (opt.selected) {
            if(selectOptions[i - 1].enabled != enabled){
                return changed ;
            }
                        
            var prev = selectOptions[i-1] ;
            selectList.remove(i-1);
            selectList.add(prev, i) ;
			selectList.selectedIndex = i  ;
            
            if(Browser == "IE")
                selectList.selectedIndex -= 1 ;
            
            changed = true ;
            break;
		}
    }
    
    return changed ;
}

function moveDown(selectId) {
    var changed = false ;
	var selectList = document.getElementById(selectId);
	var selectOptions = selectList.getElementsByTagName('option');
    
	for (var i = selectOptions.length - 2; i >= 0; i--) {
        var opt = selectOptions[i];
        var enabled = opt.enabled ;
		if (opt.selected) {
            if(selectOptions[i + 1].enabled != enabled){
                return changed ;
            }
            var nextOpt = selectOptions[i + 1];
            selectList.remove(i + 1);
            selectList.add(nextOpt, i) ;
            selectList.selectedIndex = i  ;
            
            if(Browser == "IE")
                selectList.selectedIndex += 1 ;
            
            changed = true ;    
            break;
        }
    }
    
    return changed ;
}



function assertOptionOrder(selectId) {
    var selectList = document.getElementById(selectId);
    var n = selectList.options[selectList.selectedIndex] ;
    var selectOptions = selectList.getElementsByTagName('option');
    var enabled = [] ;
    var disabled = [] ;
    
    //remove all options
    while (selectList.options.length) {
        var opt = selectList.options[selectList.options.length-1];
        
         opt = selectList.removeChild(opt);
        
        if (!opt.classList.contains('InactiveBoot')) {
            enabled.push(opt) ;
        }else if (opt.classList.contains('InactiveBoot')) {
            disabled.push(opt) ;
        }
    }
    
    //add enabled options first...
    while(enabled.length){
        var opt = enabled.pop();
        selectList.add(opt);
        if(opt === n){
            selectList.selectedIndex = selectList.options.length - 1 ;
        }
    }
    
    //add disabled options last...
    while(disabled.length){
        var opt = disabled.pop();
        selectList.add(opt);
        if(opt === n){
            selectList.selectedIndex = selectList.options.length - 1 ;
        }
    }
    
    disabled = enabled = null ;
}

/**
 * Parser for converting the SetupData Xml DOM to HTML.
 *  Emphasis:
 * Translating and displaying the xml representation of setupdata (i.e, forms and controls).
    * Creating the root menu for the HTML page (displayed in the header and side nav)
    * Parsing of control types
        * subtitle
        * text
        * select (oneof)
        * date
        * time
        * submenu (goto)
        * numeric
        * password
        * string
        
    * Dynamic bookmarks for submenu/links (to aid browser history management for single page application)
 *
 **/
HiiParser = {

    // Creates root menus...
    createRootMenu : function(node, container) {
        
        if(!container || !node)return ;
        
        var RootMenu = container ; 
        var page = node ;
        var handle = page.getAttribute("Handle") ; 
        var guid = page.getAttribute("PlatformGuid") ; 
        var PageID = page.getAttribute("PageID") ;
                            
        var pageTitle = "" 
                                
        var PageTitle = node.getAttribute("PageTitle") ;
        
        
        if(PageTitle == "Security"){
            if(isRemoteBios == false)
                return ;    
        }
        
        // create root menu item
        var item = document.createElement("li") ;
            
        if(item){

            item.innerHTML = "<a href='#"+PageID+"'>"+PageTitle+"</a>" ;
            item.setAttribute("PlatformGuid", guid) ;
            item.setAttribute("title", decodeEntities(PageTitle)) ;
            RootMenu.appendChild(item) ;

            var a = item.getElementsByTagName("a")[0] ;
            
            if(a){
                a.setAttribute("PlatformGuid", guid) ;
                a.isRootMenuLink = true ; 
                a.onclick = function(evt) {
                    evt.preventDefault() ;
                    var PageId = this.href.substr(this.href.lastIndexOf("#")+1) ;
                    var Guid = a.getAttribute("PlatformGuid") ;

                    //Check if already loaded/displayed
                    if(CurrentPage == PageId+"&"+Guid)
                        return

                    createHistoryItem(PageId, Guid) ;
                    showForm(PageId , Guid) ;
                    
                    
                };//end-onclick
            }//end-if
        }//end-if

    },
    
    createDiagTemplate : function(node) {
        
        var xmlhttp = createHttpRequest();
        xmlhttp.onreadystatechange = function(){
            if (xmlhttp.readyState==4 && xmlhttp.status==200){
                var res = xmlhttp.responseText ;

               var container = document.getElementById("content") ;
                if(container){
                    container.innerHTML = res ;
                    
                    setTimeout(function() {
                        
                        var runBtn = document.getElementById("RunScript");
                        
                        runBtn.onclick = function(){
                            
                            var runScript = document.getElementById("DiagScript");
                            var diagActivity = document.getElementById("diagActivity");
                            var runBtn = document.getElementById("RunScript");
                            
                            if (diagActivity.classList.contains('inactive')) {
                                diagActivity.classList.remove('inactive') ;
                            }
                            
                            var value = runScript.options[runScript.selectedIndex].value ;
                                    
                            var url = "/DIAG/"+value ;
                            
                            var xmlhttp = createHttpRequest();
                            xmlhttp.onreadystatechange=function(){
                                
                                if (xmlhttp.readyState==4 && xmlhttp.status==200){
                                    var res = xmlhttp.responseText ;
                                    
                                    var DiagRes = JSON.parse(res) ;
                                    
                                    if(typeof DiagRes === "object" && typeof DiagRes.DiagStatus != undefined && DiagRes.DiagStatus == "Started"){
                                        startDiagPolling() ;
                                    }
                                }
                            };

                            runBtn.disabled = true ; // disable the run button
                            xmlhttp.open("POST", url, true) ;
                            xmlhttp.send() ;
                        }
                        
                      
                    }, 1000);
                }

            }//end-if xmlhttp.readyState
        }
    
    var url = "Diag.html" ;    
    if(isRemoteBios == false)
        url = "/mac/?operation=get_resource&id="+urlBase+"/HttpClientApp/"+url ;
    
    xmlhttp.open("GET", url, true) ;
    xmlhttp.send() ;
        
    },

    createComponentTemplate : function(node) {
        // create HTML FieldSet Element
        var component = null ;
        var component = document.createElement("FIELDSET");
        component.className = "component" ;
        return component ;    
    },
    
    createComponentLabel : function(node) {
        // create HTML Label Element
        var label = null ;
        label = document.createElement("LABEL") ;
        return label ;    
    },
    
    createComponentHelp : function(node) {
        // create HTML P Element
        var help = null ;
        help = document.createElement("P");
        return help ;    
    },
    
    startTime : function () {
        function checkTime(i) {
            if (i<10) {i = "0" + i} ;  // add zero in front of numbers < 10
            return i ;
        }
        
        if(timeDiff){
            var realtime = Date.now() + timeDiff;
            var today= new Date(realtime) ;
        }else{
            var today= new Date() ;
        }
        var h=today.getHours() ;
        var m=today.getMinutes() ;
        var s=today.getSeconds() ;
        
        //var ampm = h >= 12 ? 'pm' : 'am' ;
        //h = h % 12 ;
  	    //h = h ? h : 12 ; // the hour '0' should be '12'
        h = h<10 ? ("0"+h):h
        m = checkTime(m) ;
        s = checkTime(s) ;
        var time = document.getElementById(SYSTEM_TIME) ;
        if(time && (time.inUpdate == false)){
            time.value = h+":"+m+":"+s ; //+ " " + ampm ;
            var th = this ;
            Timex = setTimeout(function(){th.startTime()},1000) ;
        }
    },
  
    ControlTypeMapping : {
        
        radio : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
                        
            var ControlID = node.getAttribute("ControlID") ;
            var ControlPageID = node.getAttribute("ControlPageID") ;
            var guid = node.getAttribute("PlatformGuid") ; 
            var handle = node.getAttribute("Handle") ; 
            var QuestionID = node.getAttribute("QuestionID") ;
            
                                    
            if(text1){
                text1.innerHTML = getStringById(node.getAttribute("Prompt"), guid, handle) ;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            //component.appendChild(label);
            container.appendChild(component);
            
            // get node options...
            var options = node.getElementsByTagName("Option") ;
            
            // create and add options...
            for (var i = 0; i < options.length; i++){
                                
                var label = document.createElement("DIV");
                label.className = "topcoat-radio-button" ;
                
                var innerlabel = document.createElement("SPAN");
                //innerlabel.innerHTML = getStringById(options[i].getAttribute("Option"), guid, handle) ; ;
                innerlabel.innerHTML = (i==1)?"Enabled":"Disabled" ;
                innerlabel.style.padding = "0 5px 0 0" ;
            
                var input = document.createElement("INPUT");
                input.type="radio"  ;
                //input.checked = (i==options.length-1)?true:false  ;
                input.name = text1.innerHTML ;
                input.index = i ; //parseInt(options[i].getAttribute("Value"))  ;
                
                if(ControlID){
                    input.id = ControlID+"&"+input.index ;
                    input.QuestionID = QuestionID ;
                    input.ControlPageID = ControlPageID ;
                    input.PlatformGuid = guid ;    
                    // in case where 2 controls have the same id
                    if(QuestionID){ 
                        input.id += "_"+QuestionID ;    
                    }
                    
                    // in case where 2 controls have the same id
                    if(QuestionID){ 
                        input.id += "_"+QuestionID ;    
                    }

                    // Handle the onchange Event...
                    input.onchange = function(e){
                        if(e.target.id !="") {

                            var id = e.target.id.split("&")[0] ;
                            var ctrl = SetupDataGetControlById(id, e.target.PlatformGuid) ;
                            /*    
                            if(ctrl && ctrl.hasAttribute("value") ){
                                var value = parseInt(ctrl.getAttribute("value")) ;
                                if(value == e.target.index)
                                    return ;
                            }
                            */
                            //alert("id="+e.target.id+", value="+e.target.value) ;
                            httpPostData(id, e.target.index) ;


                        }//end-if 
                    };//end-onchange
                }//end-if
                
                var div = document.createElement("DIV");
                div.className = "topcoat-radio-button__checkmark" ;
            
                label.appendChild(innerlabel) ;
                label.appendChild(input) ;
                label.appendChild(div) ;
                component.appendChild(label);
                
                if(node.hasAttribute("value")){
                    //var value = parseInt(node.getAttribute("value")) ;
                    //input.checked = (value==input.index)?true:false ; 
                }
                
                //Ignore the cache!!!!!
                // Get the initial value...
                
                httpGetData(input.id);
                                
            }//end-for
            
            //component.appendChild(label);
            container.appendChild(component);
            
            node.parentNode.setAttribute("hasCheckBox", "true") ;
            
        },
        
        checkbox : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            var Prompt = this.createComponentLabel() ;
            
            var ControlID = node.getAttribute("ControlID") ;
            var ControlPageID = node.getAttribute("ControlPageID") ;
            var guid = node.getAttribute("PlatformGuid") ;
            var handle = node.getAttribute("Handle") ; 
            var QuestionID = node.getAttribute("QuestionID") ;
                        
            if(Prompt){
                Prompt.innerHTML = getStringById(node.getAttribute("Prompt"), guid, handle) ;
                Prompt.className = "left" ;
                component.appendChild(Prompt) ;
            }
            
            // create activity indicator
            var loader = document.createElement("DIV");
            loader.className = "loader" ;
            loader.innerHTML += '<img src="Loader.png" />' ; 
            
            var label = document.createElement("LABEL");
            label.className = "topcoat-checkbox" ;
            
            var checkbox = document.createElement("INPUT");
            checkbox.type="checkbox"  ;
            checkbox.checked = true  ;
            checkbox.activityPtr = loader  ;
                        
            var div = document.createElement("DIV");
            div.className = "topcoat-checkbox__checkmark" ;
            
            var innerlabel = document.createElement("SPAN");
            innerlabel.innerHTML = "" ;
            innerlabel.style.padding = "0 0 0 10px" ;
            innerlabel.className = "checkbox__label" ;
            
            label.appendChild(checkbox) ;
            //label.appendChild(innerlabel) ;
            label.appendChild(div) ;
            label.appendChild(innerlabel) ;
            
            
            checkbox.label = innerlabel ;
            
            checkbox.label.innerHTML = (checkbox.checked)?"Enabled":"Disabled" ;
            
            if(node.getAttribute("help")=="")
                label.title = "" ;//getStringById(node.getAttribute("Prompt"), guid, handle) ;
            else    
                label.title = getStringById(node.getAttribute("Help"), guid, handle) ;
                
        
            component.appendChild(label);
            component.appendChild(loader);
            container.appendChild(component);
            
            node.parentNode.setAttribute("hasCheckBox", "true") ;
            
            
            if(ControlID){
                checkbox.id = ControlID ;
                checkbox.QuestionID = QuestionID ;
                checkbox.ControlPageID = ControlPageID ;
                checkbox.PlatformGuid = guid ;    
                                
                // Handle the onchange Event...
				checkbox.onchange = function(e){
					if(e.target.id !="") {
                        
                        var ctrl = SetupDataGetControlById(e.target.id, e.target.PlatformGuid) ;
                        
                        if(ctrl && ctrl.hasAttribute("value") ){
                            var value = parseInt(ctrl.getAttribute("value")) ;
                            var checked = (value==1)?true:false ;
                            if(checked == e.target.checked)
                                return ;
                        }
                                        
                        httpPostData(e.target.id, (e.target.checked)?true:false) ;
			
                        e.target.label.innerHTML = (e.target.checked)?"Enabled":"Disabled" ;
                        
                    }//end-if 
				};//end-onchange
                
                if(node.hasAttribute("value")){
                    var value = parseInt(node.getAttribute("value")) ;
                    checkbox.checked = (value==1)?true:false ; 
                    checkbox.label.innerHTML = (value==1)?"Enabled":"Disabled" ;
                    loader.className = "loader inactive" ;
                }
                    
                //Ignore the cache!!!!!
                // Get the initial value...
                httpGetData(checkbox.id);
                checkbox.label.innerHTML = (checkbox.checked)?"Enabled":"Disabled" ;
                
            }
            
        },
        
        switch : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
            var prompt = "" ;
            
            
            
            var ControlID = node.getAttribute("AttributeName") ;
            if(ControlID){
                var entry = getRegistryEntry(ControlID) ;
                if(entry){
                    prompt = entry.DisplayName ;
                    
                }
                
            }
            //var ControlPageID = node.getAttribute("ControlPageID") ;
            //var guid = node.getAttribute("PlatformGuid") ;
            //var handle = node.getAttribute("Handle") ; 
            //var QuestionID = node.getAttribute("QuestionID") ;
            
            if(text1){
                text1.innerHTML = prompt;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            // create activity indicator
            //var loader = document.createElement("DIV");
            //loader.className = "loader" ;
            //loader.innerHTML += '<img src="Loader.png" />' ; 
            
            var label = document.createElement("LABEL");
            label.className = "topcoat-switch" ;
            
            var input = document.createElement("INPUT");
            input.type="checkbox"  ;
            input.checked = true  ;
            input.className = "topcoat-switch__input" ;
                        
            var div = document.createElement("DIV");
            div.className = "topcoat-switch__toggle" ;
            
            label.appendChild(input) ;
            label.appendChild(div) ;
            
            component.appendChild(label);
            //component.appendChild(loader);
            container.appendChild(component);
            
            
            var help = this.createComponentHelp() ;
            
            if(help && entry.HelpText){
                help.innerHTML = entry.HelpText ;
                help.className = "form-help" ;
				help.style.clear = "both"
                component.appendChild(help) ;
            }
                        
            //node.parentNode.setAttribute("hasCheckBox", "true") ;
            node.hasCheckBox = "true" ;
            
                                  
            if(ControlID){
                input.id = ControlID ;
                //input.QuestionID = QuestionID ;
                //input.ControlPageID = ControlPageID ;
                //input.PlatformGuid = guid ;    
                //input.activityPtr = loader ;
                                
                // Handle the onchange Event...
				input.onchange = function(e){
					if(e.target.id !="") {
                                                            
                        //alert("id="+e.target.id+", value="+e.target.value) ;
                        //httpPostData(e.target.id, (e.target.checked)?1:0) ;
                        httpPostData(e.target.id, (e.target.checked)?true:false) ;
                        
                        
                    }//end-if 
                    
				};//end-onchange
                
                                
                //if(node.hasAttribute("value")){
                //    var value = parseInt(node.getAttribute("value")) ;
                //    input.checked = (value==1)?true:false ;
                //    loader.className = "loader inactive" ;
                //}
                    
                //Ignore the cache!!!!!
                // Get the initial value...
                setControlId(ControlID) ;
                
                httpGetData(input.id);
                
            }
        },
        
        toggle : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
            
            var ControlID = node.getAttribute("ControlID") ;
            var ControlPageID = node.getAttribute("ControlPageID") ;
            var guid = node.getAttribute("PlatformGuid") ;
            var handle = node.getAttribute("Handle") ; 
            
            if(text1){
                text1.innerHTML = getStringById(node.getAttribute("Prompt"), guid, handle) ;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            var label = document.createElement("SPAN");
            label.className = "toggle" ;
            
            var input = document.createElement("INPUT");
            input.type="checkbox"  ;
            input.checked = true  ;
            //input.className = "topcoat-switch__input" ;
                        
            var div = document.createElement("DIV");
            div.className = "tog" ;
            div.setAttribute("data-on", "1") ;
            div.setAttribute("data-off", "0") ;
            
            label.appendChild(input) ;
            label.appendChild(div) ;
            
            component.appendChild(label);
            container.appendChild(component);
            
            node.parentNode.setAttribute("hasCheckBox", "true") ;
            
                                   
            
        },
    
        menu : function(node, container) {
            this.ControlTypeMapping["page"].call( this, node, container) ;
        },
        
        page : function(node, container) {
            var e = null, root=false, id ;
            
            // create HTML Form Element
            var e = document.createElement("FORM") ;
            
            if(e){
                
                var pageId = node.getAttribute("PageID") ;
                var guid =  false ; //node.getAttribute("PlatformGuid") ;
                var handle = false ; //node.getAttribute("Handle") ; 
                if(guid){
                    e.setAttribute("PlatformGuid", guid) ;
                }
                
                var pageTitle = "" 
                                
                if(pageId == "AMIDIAG")
                    prompt = node.getAttribute("PageTitle") ;
                else
                    prompt = node.getAttribute("PageTitle") ;

                //e.setAttribute("accept-charset", "UTF-16") ;
                e.acceptCharset = "UTF-16" ;
                container.appendChild(e);
                var banner = document.getElementById("main-header-banner") ;
                banner.innerHTML = /* document.title = */ prompt ;
                banner.title = prompt ;
                banner.parentElement.title = prompt ;
            }
            
            return e ;
        },
        
        subtitle : function(node, container) {
          // create HTML sub-component Element
            var component = document.createElement("sub-component") ;
            
            if(component){
                    var text = document.createTextNode(node.getAttribute("SubTitle") || "") ;
                    var span = document.createElement('subtitle-attrib');
                
                    span.style.color = "#29cdfd";
                    span.style.fontSize = "18px";
                    span.style.fontStyle = "Italic";
                
                    span.appendChild(text);
                    span.appendChild(document.createElement('br'));
                    component.appendChild(span);
                
                    var MappingId = node.getAttribute("MappingId") ;
            
                    if(MappingId){
                        setDependencyMapToAttribute(MappingId) ;
                        component.id = MappingId ;
                    }
            }
            container.appendChild(component) ;        
        },

        text : function(node, container) {
          var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
            
            //var guid = container.getAttribute("PlatformGuid") ; 
            //var handle = node.getAttribute("Handle") ; 
            
            var MappingId = node.getAttribute("MappingId") ;
            
            if(MappingId){
                setDependencyMapToAttribute(MappingId) ;
                component.id = MappingId ; 
            }
            
            if(text1){
                text1.innerHTML = node.getAttribute("DisplayName") || "" ;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            var text2 = this.createComponentLabel() ;
            
            if(text2){
                text2.innerHTML = node.getAttribute("Text") || "" ;
                text2.className = "right" ;
                component.appendChild(text2) ;
            }
            
            container.appendChild(component);
            
        },
		
		 _bootOption : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            
            if(!component)return null ;
            
            //var ControlPageID = node.getAttribute("ControlPageID") ;
            //var guid = node.getAttribute("PlatformGuid") ;
            //var handle = node.getAttribute("Handle") ; 
            //var QuestionID = node.getAttribute("QuestionID") ;
            var prompt = "" ;
            var ControlID = node.getAttribute("AttributeName") ;
            if(ControlID){
                var entry = getRegistryEntry(ControlID) ;
                if(entry){
                    prompt = entry.DisplayName ;
                    
                    // create HTML Select Element
                    var control = document.createElement("SELECT");
            
                    if(!control)
                        return null ;

                    control.id = ControlID ;
                    control.changed = false ;
            		control.inUpdate = false ;
            		control.className = "oneOf BootOption" ;
                    
                }//end-if entry
            }//end-if
                     
            var label = this.createComponentLabel() ;
            
            if(label){
                label.innerHTML = prompt ;
                label.className = "left" ;
            }
            
            var help = this.createComponentHelp() ;
            
            if(help && entry.HelpText){
                help.innerHTML = entry.HelpText ;
                help.className = "form-help" ;
            }
			 
			 if(help){
				if(entry.HelpText == ""){
					help.innerHTML = "↑↓: Select, Shift + ↑↓: Move, F8: Enable|Disable" ;//label.innerHTML ;
				}else{    
					help.innerHTML =  "↑↓: Select &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; " ;
					help.innerHTML += "Shift+↑↓: Move &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" ;
					help.innerHTML += "F8: Enable&#47;Disable" ;
				}
				help.className = "form-help" ;

				control.title = "↑↓: Select,   Shift + ↑↓: Move,    F8: Enable/Disable" ;
			}
			 
			var bbsKeyDown = function(evt){
                var e = evt.target ;

                if(evt.shiftKey === true){
                    //pending http response?
                    if(evt.target.inUpdate)return  ;

                    if(evt.keyCode == 38){
                        if(evt.target.options[evt.target.selectedIndex].enabled == false)
                            return ;

                        if(moveUp(evt.target.id)){
                            e.changed = true ;
                        }
                    }

                    if(evt.keyCode == 40){
                        if(evt.target.options[evt.target.selectedIndex].enabled == false)
                            return ;

                        if(moveDown(evt.target.id)){
                            e.changed = true ;
                        }
                    }
                }

                if(!evt.ctrlKey && evt.keyCode == 119){ 

                    var o = evt.target.options[evt.target.selectedIndex] ;
                    
                    var cl = o.classList ;
                    if (cl.contains('InactiveBoot')) {
                        cl.remove('InactiveBoot') ;
                        o.enabled = true ;
                        e.changed = true ;
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
		    	if(evt.target.options.length == 1){
				o.parentNode.classList.remove('InactiveBoot') ;
		    	}
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
                    } else {
                        cl.add('InactiveBoot') ;
                        o.enabled = false ;
                        e.changed = true ;
//<kuaibai002-20180813 Fix OOB web cannot save changes +>
			if(evt.target.options.length == 1){
				o.parentNode.classList.add('InactiveBoot') ;
			}
//<kuaibai002-20180813 Fix OOB web cannot save changes ->
                    }
                    assertOptionOrder(evt.target.id) ;
                }

                console.log("changed="+e.changed) ;
            };

            //control.addEventListener("keydown", bbskey) ;

            //control.onkeydown = bbskey ;

            var bbsKeyUp  = function(evt){
                var e = evt.target ;

                //e.preventDefault();

                if(evt.shiftKey === false /*&& (evt.keyCode == 38 || evt.keyCode == 40)*/){ 
                    if(e.changed == true){
                        e.changed=false ;

                        console.log("POST the update") ;

                        //Set inUpdate flag
                        //e.inUpdate=true ;
                        //Send POST request to update BootOrder
                        //specialBootPostRequest(e.id) ;
                        specialBootSetData(e.id) ;
                    }
                }
            }; 
                        
            component.appendChild(label) ;
            component.appendChild(control) ;
            //component.appendChild(loader) ;
            component.appendChild(help) ;
            container.appendChild(component) ;
			 
			httpGetData(control.id);
		 
			control.addEventListener("keydown", bbsKeyDown) ;
            control.addEventListener("keyup", bbsKeyUp) ; 
        },

        select : function(node, container) {
            
            var component = this.createComponentTemplate(node) ;
            
            if(!component)return null ;
            
            //var ControlPageID = node.getAttribute("ControlPageID") ;
            //var guid = node.getAttribute("PlatformGuid") ;
            //var handle = node.getAttribute("Handle") ; 
            //var QuestionID = node.getAttribute("QuestionID") ;
            var prompt = "" ;
            var ControlID = node.getAttribute("AttributeName") ;
            if(ControlID){
                var entry = getRegistryEntry(ControlID) ;
                if(entry){
                    prompt = entry.DisplayName ;
                    
                    // create HTML Select Element
                    var control = document.createElement("SELECT");
            
                    if(!control)
                        return null ;

                    control.id = ControlID ;
                    //control.QuestionID = QuestionID ;
                    //control.ControlPageID = ControlPageID ;
                    //control.PlatformGuid = guid ;    
                    //control.activityPtr = loader ;  
                    control.className = "oneOf" ;
                    
                    if(Browser == "KHTML")
                        control.className += " combo" ;
                
                    // get node options...
                    var options = entry.Value ;
            
                    // create and add options...
                    for (var i = 0; i < options.length; i++){
                        var option = document.createElement("option") ;
                        option.text = options[i].ValueDisplayName ;
                        option.value = options[i].ValueName ;
                        control.add(option) ;
                    }
                    
                    // Handle the onchange Event...
                    control.onchange = function(e){
                        //e.preventDefault();
                        if(e.target.id !="") {
                            httpPostData(e.target.id, e.target.options[e.target.selectedIndex].value) ;
                            //httpPostData(e.target.id, e.target.selectedIndex) ;
                        }//end-if 
                    };//end-onchange

                }
            }
                     
            var label = this.createComponentLabel() ;
            
            if(label){
                label.innerHTML = prompt ;
                label.className = "left" ;
            }
            
            var help = this.createComponentHelp() ;
            
            if(help && entry.HelpText){
                help.innerHTML = entry.HelpText ;
                help.className = "form-help" ;
            }
                        
            component.appendChild(label) ;
            component.appendChild(control) ;
            //component.appendChild(loader) ;
            component.appendChild(help) ;
            container.appendChild(component) ;
        
            
            setControlId(ControlID) ;
            
            httpGetData(control.id);
            
            //console.log(control.selectedIndex + ", " + control.options.length) ;
            
            if(control.selectedIndex < 0){
                /*
                var val = getConfigDefault(control.id) ;
                if(val != null){
                    val = parseInt(val) ;
                    for (var i = 0; i < control.options.length; i++){
                        if(parseInt(control.options[i].value) == val){
                            control.selectedIndex = i ;
                            break ;
                        }//end-if
                    }//end-for    
                }
                */
            }
                        
            
            
        },
        
        date : function(node, container) {
            var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
            
            var guid = node.getAttribute("PlatformGuid") ;
            var handle = node.getAttribute("Handle") ; 
            
            if(text1){
                text1.innerHTML = getStringById(node.getAttribute("Prompt"), guid, handle) ;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            //var DateText = this.createComponentLabel() ;
            var DateText = document.createElement("INPUT");
            
            if(DateText){
                if(serverDateTime)
                    var d = serverDateTime ;
                else
                    var d = new Date() ;
                
                DateText.type="text"  ;
                DateText.inUpdate = false ;  
                DateText.value = d.toDateString() ;
                DateText.className = "datetime" ;
                DateText.id = SYSTEM_DATE ;
                DateText.title = "Click to enter Date MM/DD/YYYY" ;
                
                component.appendChild(DateText) ;
                
                var month = d.getMonth() ;
                var day = d.getDate() ;
                var year = d.getFullYear() ;
                
                month = month<10 ? ("0"+month):month ;
                day = day<10 ? ("0"+day):day ;
                                
                DateText.value = month+"/"+day+"/"+year ; 
                
                DateText.onkeyup = function(){
                    if(isDate(DateText.value, "/")){
                        DateText.className = "datetime" ;
                    }else{
                        DateText.className = "datetime invalid" ;
                    }
                }
                
                DateText.onchange = function(){
                    if(DateText.className == "datetime"){
                        //setDateTime(DateText.id) ;                     
                    }
                }
                
            }
            
            container.appendChild(component) ;
        },

        time : function(node, container) {
            var component = this.createComponentTemplate(node) ;
            var text1 = this.createComponentLabel() ;
            var guid = node.getAttribute("PlatformGuid") ;
            var handle = node.getAttribute("Handle") ;
            var that = this ;
            
            
            if(text1){
                text1.innerHTML = getStringById(node.getAttribute("Prompt"), guid, handle) ;
                text1.className = "left" ;
                component.appendChild(text1) ;
            }
            
            //var TimeText = this.createComponentLabel() ;
            var TimeText = document.createElement("INPUT");
            
            if(TimeText){
				if(serverDateTime)
                    var d = serverDateTime ;
                else
                    var d = new Date() ;
                TimeText.type="text"  ;
                TimeText.inUpdate = false ;  
                TimeText.value = d.toDateString() ;
                TimeText.className = "datetime" ;
                TimeText.id = SYSTEM_TIME ;
                TimeText.title = "Click to enter Time HH:MM:SS" ;
                
                component.appendChild(TimeText) ;
                
                TimeText.onfocus = function(){
					TimeText.inUpdate = true ;
                    clearTimeout(Timex);
					Timex = null ;
                }
                
                TimeText.onkeyup = function(){
                    
                    function validateTime(s) {
                      var t = s.split(':');

                      return /^\d\d:\d\d:\d\d$/.test(s) &&
                             t[0] >= 0 && t[0] < 25 &&
                             t[1] >= 0 && t[1] < 60 &&
                             t[2] >= 0 && t[2] < 60;
                    }
                                        
                    var isValidTime = validateTime(TimeText.value) ; //patt.test(text2.value);
                   
                    if(isValidTime){
                        TimeText.className = "datetime" ;
                    }else{
                        TimeText.className = "datetime invalid" ;
                    }
                }
                
                TimeText.onblur = function(){
					TimeText.inUpdate = false ;
                    that.startTime();
                }
                
                TimeText.onchange = function(){
                    if(TimeText.className == "datetime"){
                        //setDateTime(TimeText.id) ;                     
                    }
                }
            }
            
            container.appendChild(component) ;
            
            this.startTime() ;
              
        },

        submenu : function(node, container) {
        
            if(node.parentNode.getAttribute("PageID")== StartPageId)
                return ;
            
            //Get id of the Destenation Page...
            var formid = node.getAttribute("ControlDestPageID") || "" ;
            var prompt = node.getAttribute("DisplayName") || "" ;
            var guid = node.getAttribute("PlatformGuid") ;
			var refguid = node.getAttribute("RefGuid") ; 
            var handle = node.getAttribute("Handle") ; 
            var component = this.createComponentTemplate(node) ;
            var label = this.createComponentLabel() ;
            var PageTitle = node.parentNode.getAttribute("PageTitle") || false ;
            var exitPageLink = false ;
						
	var tempGuid = (refguid != "" && refguid != null)? refguid : guid ; 
			
	var page = SetupDataGetPageById(formid, tempGuid) || false ;
			
	if(!page || page == null){
		// PAGE IS NOT PRESENT, DO NOT CREATE THE LINK!!!!
		return 
	}else{
		//...in this case, RefGuid is the guid of the parentPage 
		page.setAttribute("RefGuid", guid) ; 
	}
	    var MappingId = node.getAttribute("MappingId") ;
            
            if(MappingId){
                setDependencyMapToAttribute(MappingId) ;
                label.id = MappingId ;
            }
			            
            if(label){
                if(PageTitle && (PageTitle == "Save & Exit")){
                    switch(prompt){
                        case "Save Changes" :
                        case "Discard Changes" :
                            exitPageLink = true ;
                            break ;

                        default:
                            return ;
                    }
                }
                
                label.innerHTML = "<a id='"+prompt+"' href='#"+formid+"'>"+prompt+"</a>" ;
                label.className = "left" ;
                component.appendChild(label) ;
                                             
                var link = label.getElementsByTagName("a")[0] ;
                
                link.RefGuid = refguid ;
				                               
                link.onclick = function(evt) {
                    evt.preventDefault();
                    
                    if(evt.target.id !="" && evt.target.id=="reset") { 
					    doReset() ;
					    return ;
    			    }//end-if 
                                    
                    var target = link.href.substr(link.href.lastIndexOf("#")+1)
                    
                    if(target.length){
                        
                        if(CurrentPage == target+"&"+guid){
                            
                            switch(evt.target.id){
                                case "Save Changes":
                                    updateResource() ;
                                    break ;
                                    
                                case "Discard Changes" :
                                    discardChanges() ;
                                    break ;
                                    
                                default:    
                            }
                            
                            if(evt.target.exitPageLink) {
                                if(!CHANGED){
                                    var links = document.getElementById("content").getElementsByTagName("a") ;
                                    
                                    if(links.length){

                                        for(var i=0; i<links.length; i++){
                                            var cl = links[i].classList ;
                                            if (!cl.contains('off')) {
                                                cl.add('off') ;
                                            }   
                                        }//end-for
                                    }
                                }
                            }
                            return
                        }
                        
						var refGuid = ((this.RefGuid != "") && (this.RefGuid != null))? this.RefGuid : guid ; 
                        //location.hash = target ; // bookmark this form
                        createHistoryItem(target, refGuid, null) ;
                        showForm(target, refGuid) ;
                    }
		        };
                
                if(exitPageLink) {
                    link.exitPageLink = exitPageLink ;
                    if(!CHANGED){
                        var cl = link.classList ;
                        if (!cl.contains('off')) {
                            cl.add('off') ;
                        }    
                    }
                }
            }
            
            container.appendChild(component);
        },

        numeric : function(node, container) {
            var component = this.createComponentTemplate(node) ;
            
            //var guid = node.getAttribute("PlatformGuid") ;
            //var handle = node.getAttribute("Handle") ; 
            //var QuestionID = node.getAttribute("QuestionID") ;
            
            var prompt = "" ;
            var ControlID = node.getAttribute("AttributeName") ;
            if(ControlID){
                var entry = getRegistryEntry(ControlID) ;
                if(entry){
                    prompt = entry.DisplayName ;
                    
                    var label = this.createComponentLabel() ;
                    if(label){
                        label.innerHTML = entry.DisplayName ;
                        label.className = "left" ;
                    }
                                        
                    // create HTML Input[type=number] Element
                    var control = document.createElement("INPUT");

                    if(control){
                        var MinValue =  entry.LowerBound ;
                        var MaxValue =  entry.UpperBound;
                        var Step =  parseInt(entry.ScalarIncrement) ;
                        
                        //var ControlPageID = node.getAttribute("ControlPageID") ;

                        if(MinValue != undefined)control.setAttribute("min", MinValue) ;
                        if(MaxValue != undefined)control.setAttribute("max", MaxValue) ;
                        if(Step != undefined)control.setAttribute("step", Step) ;

                        control.value = entry.DefaultValue ;
                        control.type = "number" ;
                        //control.className = "control" ;
                        control.className = "numeric" ;
                        
                        control.id = ControlID ;
                        //control.QuestionID = QuestionID ;
                        //control.ControlPageID = ControlPageID ;
                        //control.PlatformGuid = guid ;    

                        // Handle the onchange Event...
                        control.onchange = function(e){
                            //validate max input through keyboard
                            if(parseInt(e.target.value) >= parseInt(e.target.max)){
                                e.target.value = parseInt(e.target.max);  
                            }

                            //validate min input through keyboard
                            if(parseInt(e.target.value) <= parseInt(e.target.min)){
                                e.target.value = parseInt(e.target.min);  
                            }

                            if(e.target.id !="" ) {
                                //console.log("id="+e.target.id+", value="+e.target.value) ;
                                httpPostData(e.target.id, parseInt(e.target.value)) ;
                            }//end-if 
                        };//end-onchange
                    }//end-control
           
                    
                }
            }
            
            var help = this.createComponentHelp() ;
            
            if(help && entry.HelpText){
                help.innerHTML = entry.HelpText ;
                help.className = "form-help" ;
            }
            
            if(component){
                component.appendChild(label) ;
                component.appendChild(control) ;
                component.appendChild(help) ;
                container.appendChild(component) ;
                //Ignore the cache!!!!!
                // Get the initial value...
                
              setControlId(ControlID) ;    
                
		      httpGetData(control.id);
                
            }
            
        },

        string : function(node, container) {
            //var guid = node.getAttribute("PlatformGuid") ;
            //var handle = node.getAttribute("Handle") ; 
            //var ControlID = node.AttributeName ;
            //var ControlPageID = node.getAttribute("ControlPageID") ;
            //var QuestionID = node.getAttribute("QuestionID") ;

            var component = null ;
            var prompt = "" ;
            var ControlID = node.getAttribute("AttributeName") ;
			
			if(ControlID == "SETUP006"){//Process Boot Option Priorities
				this.ControlTypeMapping["_bootOption"].call( this, node, container) ;
				return
			}
				
            
            var entry = getRegistryEntry(ControlID) ;
            if(entry){
                prompt = entry.DisplayName ;
                
                // create HTML Input[type=TEXT] Element
                var control = document.createElement("INPUT");

                if(control){

                    control.type="text"  ;
                    control.className = "form-text" ;
                    control.placeholder = prompt ;

                    var MinValue =  parseInt(entry.MinLength) ;
                    var MaxValue =  parseInt(entry.MaxLength) ;

                    if(MinValue != undefined)control.setAttribute("minlength", MinValue) ;
                    if(MaxValue != undefined)control.setAttribute("maxlength", MaxValue) ; 
                    
                    control.id = ControlID ;
                    //control.ControlPageID = ControlPageID ;
                    //control.PlatformGuid = guid ;
                    //control.QuestionID = QuestionID ;

                    // Handle the onchange Event...
                    control.onchange = function(e){
                        if(e.target.id !="") { 
                            //alert("id="+e.target.id+", value="+e.target.value) ;
                            httpPostData(e.target.id, e.target.value) ;
                        }//end-if 
                    }
                    
                    var label = this.createComponentLabel() ;
                    if(label){
                        label.innerHTML = prompt ;
                        label.className = "left" ;
                    }
                    
                    var help = this.createComponentHelp() ;
            
                    if(help && entry.HelpText){
                        help.innerHTML = entry.HelpText ;
                        help.className = "form-help" ;
                    }
                    
                    component = this.createComponentTemplate(node) ;
                }
                
            }
            
            if(component){
                component.appendChild(label);
                component.appendChild(control);
                component.appendChild(help);
                container.appendChild(component);
                // Get the initial value...
                
                setControlId(ControlID) ;
                
                httpGetData(control.id);
            }
        },

        password : function(node, container, Prompt) {
                        
            var Password = false ;
            
            var ControlID = node.getAttribute("AttributeName") ;
            
            var entry = getRegistryEntry(ControlID) ;
            if(entry){
                //prompt = entry.DisplayName ;
                                    
                var prompt = Prompt?Prompt+"":"Old" ;
            
                // create HTML Input (type=password) Element
                var control = document.createElement("INPUT");
                                
                if(control){
                    control.type="password" ;
                    control.className = "form-text" ;
                    //control.placeholder = prompt + " " + entry.DisplayName ;
                    control.minSize = entry.MinLength || PASSWORD_MIN_SIZE ;
                    control.maxSize = entry.MaxLength|| PASSWORD_MAX_SIZE ;
                    control.id = ControlID ;
                    
                    //if(Prompt){
                        control.id += "&"+prompt ;
                    //}
                    
                    var label = this.createComponentLabel() ;
                    if(label){
                        label.innerHTML = prompt + " " + entry.DisplayName ;
                        label.className = "left" ;
                    }

                    var help = this.createComponentHelp() ;

                    if(help && entry.HelpText){
                        help.innerHTML = entry.HelpText ;
                        help.className = "form-help" ;
                    }
                    
                    Password = this.createComponentTemplate(node) ;
                }
                
            }
                      
            if(Password){
                Password.appendChild(label) ;
                Password.appendChild(control) ;
                Password.appendChild(help) ;
                container.appendChild(Password) ;
                
                Password.control = control ;
                
                // Handle the changeEvent...
                Password.control.onchange = function(e){
                    if(e.target.id !="") {
                        if(e.target.value.length > e.target.maxSize) return true ;
                        //alert("id="+e.target.id+", value="+e.target.value) ;
                        //httpPostData(e.target.id, e.target.value) ;
                        setPasswordData(e.target.id, e.target.value) ;
                    }//end-if 
                }
                            
                getPasswordData(Password.control.id) ;
                
                if(!Prompt){ 
                    //var NewPassword = this.ControlTypeMapping["password"].call( this, node, container, "New") ;
                    var ConfirmPassword = this.ControlTypeMapping["password"].call( this, node, container, "New") ;
                }
                
                return Password ;
            }
        },
        
    },
    

    parseChildren : function(node, c) {
        var e = null;
        try{ 
            if (node.nodeType == 1){
                var nodename = node.tagName.toLowerCase();
                if (nodename=="control" || nodename=="page"|| nodename=="menu") {
                    
                    var type =  node.getAttribute("Type") || nodename ; 
                    
                    if(type == 'control'){
                        var attr = node.getAttribute("AttributeName") || null ;
                        if(attr){
                            type = getRegistryEntryType(attr) ;        
                            
                            if(!type) 
                                type =  nodename ; 
                        }
                    }
                
                    
                    
                    switch(type) {
                            
                        case "Boolean":
                        case "Checkbox":
                        case "CHECKBOX":
                            e = this.ControlTypeMapping["switch"].call( this, node, c) ;
	       			        break;    
                    
    			        // CONTROL_TYPE_SUBMENU
                        case "Submenu":    
                        case "SUBMENU":
    			        case 0x0001: 
    			            e = this.ControlTypeMapping["submenu"].call( this, node, c) ;
	       			        break;

        	       		// CONTROL_TYPE_TEXT
                        case "Text":    
                        case "TEXT":
        	       		case 0x0003: 
    			            e = this.ControlTypeMapping["text"].call( this, node, c) ;
	       			        break;
	       			    
	       			    // CONTROL_TYPE_DATE
                        case "Date":
                        case "DATE":
	       			    case 0x0004: 
    			            //e = this.ControlTypeMapping["date"].call( this, node, c) ;
	       			        break;    
	       			        
	       			    // CONTROL_TYPE_TIME
                        case "Time":
                        case "TIME":
	       			    case 0x0005: 
    			            //e = this.ControlTypeMapping["time"].call( this, node, c) ;
	       			        break;
                        
                        // CONTROL_TYPE_MEMO
                        case "Memo": 
                        case "MEMO": 
	       			    case 0x0008: 
    			            e = this.ControlTypeMapping["subtitle"].call( this, node, c) ;
	       			        break;    
	       			        
                        case "Enumeration":
                        case "PopupSel": 
                        case "POPUPSEL": 
                        case 0x0006: // CONTROL_TYPE_POPUPSEL
	       			        e = this.ControlTypeMapping["select"].call( this, node, c) ;
	       			        break;     
	       		        
                        case "Integer":
                        case "Numeric":
                        case "NUMERIC": 
	       		        case 0x000b: 
    			            e = this.ControlTypeMapping["numeric"].call( this, node, c) ;
	       			        break;
	       			        
	       			    // CONTROL_TYPE_PASSWORD
                        case "Password":    
                        case "PASSWORD":
	       			    case 0x000d: 
    			            e = this.ControlTypeMapping["password"].call( this, node, c) ;
	       			        break;    
	       			        
	       			    // CONTROL_TYPE_POPUP_STRING
                        case "String" :   
                        case "PopupString":    
                        case "STRING":
	       			    case 0x0013: 
    			            e = this.ControlTypeMapping["string"].call( this, node, c) ;
	       			        break;         
        			    
			            // Try the nodename...
			            default: 
			                if (this.ControlTypeMapping[type]) {
                                e = this.ControlTypeMapping[type].call( this, node, c) ;
                            }//end-if
			         }
               }//end-if
                
                if (! e) e = c;
                
                
                for (var i = 0; i < node.childNodes.length; i++){
        	        this.parseChildren(node.childNodes[i], e);
                }//end-for
                
                
           }//end-if
        }finally{
            e = null;
            c = null;
        }//end-try
        
      }

};

// http://spin.js.org/#v2.3.1
!function(a,b){"object"==typeof exports?module.exports=b():"function"==typeof define&&define.amd?define(b):a.Spinner=b()}(this,function(){"use strict";function a(a,b){var c,d=document.createElement(a||"div");for(c in b)d[c]=b[c];return d}function b(a){for(var b=1,c=arguments.length;c>b;b++)a.appendChild(arguments[b]);return a}function c(a,b,c,d){var e=["opacity",b,~~(100*a),c,d].join("-"),f=.01+c/d*100,g=Math.max(1-(1-a)/b*(100-f),a),h=j.substring(0,j.indexOf("Animation")).toLowerCase(),i=h&&"-"+h+"-"||"";return m[e]||(k.insertRule("@"+i+"keyframes "+e+"{0%{opacity:"+g+"}"+f+"%{opacity:"+a+"}"+(f+.01)+"%{opacity:1}"+(f+b)%100+"%{opacity:"+a+"}100%{opacity:"+g+"}}",k.cssRules.length),m[e]=1),e}function d(a,b){var c,d,e=a.style;if(b=b.charAt(0).toUpperCase()+b.slice(1),void 0!==e[b])return b;for(d=0;d<l.length;d++)if(c=l[d]+b,void 0!==e[c])return c}function e(a,b){for(var c in b)a.style[d(a,c)||c]=b[c];return a}function f(a){for(var b=1;b<arguments.length;b++){var c=arguments[b];for(var d in c)void 0===a[d]&&(a[d]=c[d])}return a}function g(a,b){return"string"==typeof a?a:a[b%a.length]}function h(a){this.opts=f(a||{},h.defaults,n)}function i(){function c(b,c){return a("<"+b+' xmlns="urn:schemas-microsoft.com:vml" class="spin-vml">',c)}k.addRule(".spin-vml","behavior:url(#default#VML)"),h.prototype.lines=function(a,d){function f(){return e(c("group",{coordsize:k+" "+k,coordorigin:-j+" "+-j}),{width:k,height:k})}function h(a,h,i){b(m,b(e(f(),{rotation:360/d.lines*a+"deg",left:~~h}),b(e(c("roundrect",{arcsize:d.corners}),{width:j,height:d.scale*d.width,left:d.scale*d.radius,top:-d.scale*d.width>>1,filter:i}),c("fill",{color:g(d.color,a),opacity:d.opacity}),c("stroke",{opacity:0}))))}var i,j=d.scale*(d.length+d.width),k=2*d.scale*j,l=-(d.width+d.length)*d.scale*2+"px",m=e(f(),{position:"absolute",top:l,left:l});if(d.shadow)for(i=1;i<=d.lines;i++)h(i,-2,"progid:DXImageTransform.Microsoft.Blur(pixelradius=2,makeshadow=1,shadowopacity=.3)");for(i=1;i<=d.lines;i++)h(i);return b(a,m)},h.prototype.opacity=function(a,b,c,d){var e=a.firstChild;d=d.shadow&&d.lines||0,e&&b+d<e.childNodes.length&&(e=e.childNodes[b+d],e=e&&e.firstChild,e=e&&e.firstChild,e&&(e.opacity=c))}}var j,k,l=["webkit","Moz","ms","O"],m={},n={lines:12,length:7,width:5,radius:10,scale:1,corners:1,color:"#000",opacity:.25,rotate:0,direction:1,speed:1,trail:100,fps:20,zIndex:2e9,className:"spinner",top:"50%",left:"50%",shadow:!1,hwaccel:!1,position:"absolute"};if(h.defaults={},f(h.prototype,{spin:function(b){this.stop();var c=this,d=c.opts,f=c.el=a(null,{className:d.className});if(e(f,{position:d.position,width:0,zIndex:d.zIndex,left:d.left,top:d.top}),b&&b.insertBefore(f,b.firstChild||null),f.setAttribute("role","progressbar"),c.lines(f,c.opts),!j){var g,h=0,i=(d.lines-1)*(1-d.direction)/2,k=d.fps,l=k/d.speed,m=(1-d.opacity)/(l*d.trail/100),n=l/d.lines;!function o(){h++;for(var a=0;a<d.lines;a++)g=Math.max(1-(h+(d.lines-a)*n)%l*m,d.opacity),c.opacity(f,a*d.direction+i,g,d);c.timeout=c.el&&setTimeout(o,~~(1e3/k))}()}return c},stop:function(){var a=this.el;return a&&(clearTimeout(this.timeout),a.parentNode&&a.parentNode.removeChild(a),this.el=void 0),this},lines:function(d,f){function h(b,c){return e(a(),{position:"absolute",width:f.scale*(f.length+f.width)+"px",height:f.scale*f.width+"px",background:b,boxShadow:c,transformOrigin:"left",transform:"rotate("+~~(360/f.lines*k+f.rotate)+"deg) translate("+f.scale*f.radius+"px,0)",borderRadius:(f.corners*f.scale*f.width>>1)+"px"})}for(var i,k=0,l=(f.lines-1)*(1-f.direction)/2;k<f.lines;k++)i=e(a(),{position:"absolute",top:1+~(f.scale*f.width/2)+"px",transform:f.hwaccel?"translate3d(0,0,0)":"",opacity:f.opacity,animation:j&&c(f.opacity,f.trail,l+k*f.direction,f.lines)+" "+1/f.speed+"s linear infinite"}),f.shadow&&b(i,e(h("#000","0 0 4px #000"),{top:"2px"})),b(d,b(i,h(g(f.color,k),"0 0 1px rgba(0,0,0,.1)")));return d},opacity:function(a,b,c){b<a.childNodes.length&&(a.childNodes[b].style.opacity=c)}}),"undefined"!=typeof document){k=function(){var c=a("style",{type:"text/css"});return b(document.getElementsByTagName("head")[0],c)
,c.sheet||c.styleSheet}();var o=e(a("group"),{behavior:"url(#default#VML)"});!d(o,"transform")&&o.adj?i():j=d(o,"animation")}return h});