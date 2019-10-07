<?php 

/* -------------------------------------------------
		CRON JOB FUNCTIONS AND TASKS
	---------------------------------------------- */

function communicoData_cron() {    
    //Get Authentication
    $credentials = communicoData_authenticate();

    if($credentials) {
    	/*Credintials were able to be generated
    	lets use them to get event data and save that data to json files
    	for later use on our site, so we aren't doing expensive API calls
    	each time the site and event content is loaded/requested */

    	//Define the headers that will be used for the cURL connection
    	$headers = array(
			'Authorization:'.$credentials['token_type'].' '.$credentials['access_token'],
		);

		//Create our cURL connection to be used for all the API calls
    	//we'll just swap out the URL being called
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

		//$startDate = '2019-12-01';
		$startTime = date('Y-m-d H:i:s',strtotime('-1 day'));
		$filterDateTime = $startTime; //urlencode($startTime);
    	//$startDate = date('Y-m-d');/

		//-------------------------------------------
    	//Get next upcoming 7 events for primary homepage
    	$queryURL = 'https://api.communico.co/v3/attend/events?start=0&limit=5&status=published&privateEvents=false&startDate='.$filterDateTime.'&registrationOpens='.$filterDateTime;
    	$jsonLoc = 'sites/default/files/jsondata/communico/events_www-next_5.json';
    	communicoData_curl_get_request($ch,$queryURL,$jsonLoc);

    	//-------------------------------------------
    	//Get next upcoming 7 events for kids homepage
    	$queryURL = 'https://api.communico.co/v3/attend/events?start=0&limit=5&status=published&privateEvents=false&startDate='.$filterDateTime.'&registrationOpens='.$filterDateTime.'&ages=0-12%20months,0-4%20years,0-24%20months,0-5%20years,10-12%20years,13-24%20months,2-3%20years,3-5%20years,4-12%20years,4-8%20years,6-12%20years,8-12%20years';
    	$jsonLoc = 'sites/default/files/jsondata/communico/events_kids-next_5.json';
    	communicoData_curl_get_request($ch,$queryURL,$jsonLoc);

    	//-------------------------------------------
    	//Get next upcoming 7 events for teens homepage
    	$queryURL = 'https://api.communico.co/v3/attend/events?start=0&limit=5&status=published&privateEvents=false&startDate='.$filterDateTime.'&registrationOpens='.$filterDateTime.'&ages=teens%20(12-18%20years)';
    	$jsonLoc = 'sites/default/files/jsondata/communico/events_teens-next_5.json';
    	communicoData_curl_get_request($ch,$queryURL,$jsonLoc);

    	//All API calls done? close the connection
    	curl_close($ch);

    	watchdog('communicoData_cron','Communico API cron job ran');
    } else {
        //write log message that json files were not updated
    	watchdog('communicoData_cron','Communico API failed Authorization');
    }
}