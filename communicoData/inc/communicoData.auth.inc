<?php
function communicoData_curl_get_request($curlObj,$queryURL,$jsonSaveLoc) {
	//Validate the API requests, save the json data to a file or write an error to the log
	curl_setopt($curlObj, CURLOPT_URL, $queryURL);
	$apiAnswer = curl_exec($curlObj);
	$httpcode = curl_getinfo($curlObj, CURLINFO_HTTP_CODE);

	if ($httpcode >= 200 && $httpcode < 300) {
		//There was no error, so get the data and save it to the provided location
		//watchdog('communico api call answer','<pre>'.print_r($apiAnswer, true).'</pre>');
		//$response = json_decode($apiAnswer);
		//watchdog('communico json data','<pre>'.print_r($response, true).'</pre>');
		file_put_contents($jsonSaveLoc, $apiAnswer);
	} else {
		watchdog('communico http response','<pre>'.print_r($httpcode, true).'</pre>');
		watchdog('communico request answer','<pre>'.print_r($apiAnswer, true).'</pre>');
		return false;
	}
}

function communicoData_authenticate() {
	/* This function will connect to the Communico API and 
	request an access_token through OAuth 2.0 

	RETURNS:
	false - if not able to connect and successfully request auth credentials
	or
	array - associative array of the access_token and token_type */

	$encodedAuthKey = base64_encode($clientKey.':'.$clientSecret);

	$authorizationURL = 'https://api.communico.co/v3/token';

	$headers = array(
		'Authorization:Basic '.$encodedAuthKey,
		'Content-Type:application/x-www-form-urlencoded;charset=UTF-8',
	);

	$data = array('grant_type' => 'client_credentials');

	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $authorizationURL);
	curl_setopt($ch, CURLOPT_POST, 1);
	curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($data));
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

	$apiAnswer = curl_exec($ch);
	$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

	if (curl_errno($ch)) {
		//There was an error connecting/authenticating
		//Return false so we don't overwrite the json datafile
		watchdog('communico api get auth error','<pre>'.print_r($apiAnswer, true).'</pre>');
		watchdog('communico http code','<pre>'.print_r($httpcode, true).'</pre>');
		watchdog('communico error','<pre>'.print_r(curl_errno($ch), true).'</pre>');
		curl_close($ch);
		return false;
	} else if ($httpcode >= 200 && $httpcode < 300) {
		//There was no error, so get the access_token & token_type to use for our API calls
		$response = json_decode($apiAnswer);

		$communicoAPICreds = array('access_token' => $response->access_token, 'token_type' => $response->token_type);

		curl_close($ch);
		return $communicoAPICreds;
	} else {
		watchdog('communico api auth failure','<pre>'.print_r($apiAnswer, true).'</pre>');
		curl_close($ch);
		return false;
	}


	/* Auth. request ex.
	POST /token HTTP/1.1
	Host: api.communico.co
	Authorization: Basic 
	Content-Type: application/x-www-form-urlencoded;charset=UTF-8

	Body
	grant_type=client_credentials

	//Valid respone ex.
	{
	"access_token":"",
	"token_type":"Bearer",
	"expires_in":3600,
	"scope":"EVENTS"
	}
	*/
}