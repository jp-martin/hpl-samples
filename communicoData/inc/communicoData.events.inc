<?php
function communcioData_ajax_callback($page_callback_result) {
	if($page_callback_result['jsonData'] == false) {
		print '<div>There are no Events happening at this time.</di>';
	} else {
		$decodedJson = drupal_json_decode($page_callback_result['jsonData']);
		ob_start();
		foreach($decodedJson['data']['entries'] as $event) {  ?>
			<div class="node-event vevent<?php if($event['modified'] == 'canceled' || $event['modified'] == 'rescheduled') { print ' cancelled-event'; } ?>">
				<div class="container event-row">
					<div class="node-event-5w width4 column push0">
						<div class="event-flag">
							<div class="event-flag-icon">
								<i class="fa fa-calendar-o"></i>
							</div>
							<div class="event-flag-triangle"></div>
						</div>
						<div class="event-details">
							<div class="node-event-date">
								<time class="dtstart" datetime="<?php print $event['eventStart']; ?>">
									<?php if($event['modified'] == 'rescheduled') { 
										print "<div class='cancelled-title'><a href='https://events.hpl.ca/event/".$event['newEventId']."' title='click to view the rescheduled date and time'>RESCHEDULED</a></div>"; 
									} else {
										print date('D M d, Y', strtotime($event['eventStart'])); 
									} ?>
								</time>
							</div>
							<div class="node-event-location">
								<ul class="vcard">
									<?php if($event['locationName'] == 'Bookmobile') { ?>
										<li class="fn org"><a href="/bookmobile-service"><?php print $event['locationName']; ?></a></li>
									<?php } else { ?>
										<li class="fn org"><a href="<?php print '/branches/'.str_replace(' ','-',strtolower($event['locationName'])); ?>"><?php print $event['locationName']; ?></a></li>
										<li class="addr">
											<span class="street-address"><?php print $page_callback_result['locations'][$event['locationName']]['street']; ?></span>
											<span class="locality sr-only"><?php print $page_callback_result['locations'][$event['locationName']]['city']; ?></span>
											<span class="region sr-only">ON</span>
										</li>
										<li class="geo sr-only">
											<span class="latitude">
												<span class="value-title" title="<?php print $page_callback_result['locations'][$event['locationName']]['lat']; ?>"></span>
											</span>
											<span class="longitude">
												<span class="value-title" title="<?php print $page_callback_result['locations'][$event['locationName']]['lon']; ?>"></span>
											</span>
										</li>
									<?php } ?>
									<li>
										<time datetime="<?php print date('Y-m-d H:i:s', strtotime($event['eventStart'])); ?>">
										<?php print strtoupper(date('g:i A', strtotime($event['eventStart']))); ?>
										</time>
									</li>
								</ul>
							</div>
						</div>
					</div>
					<div class="node-event-details width8 column push4">
						<div class="node-event-header">
							<?php if($event['modified'] == 'canceled') { 
								print "<div class='cancelled-title'>CANCELED</div>";
							} ?>
							<div class="node-event-title">
								<h2><a href="https://events.hpl.ca/event/<?php if($event['modified'] == 'rescheduled') { print $event['newEventId']; } else { print $event['eventId']; } ?>" title="<?php print $event['title']; ?> at <?php print $event['locationName']; ?> on <?php print $event['eventStart']; ?>"><?php print $event['title']; ?></a></h2>
							</div>
							<div class="node-event-subtitle">
								<?php if(strlen($event['subTitle']) > 0) { ?>
									<h3><?php print $event['subTitle']; ?></h3>
								<?php } ?>
							</div>
						</div>

						<div class="node-event-description">
							<?php print stripcslashes(trim($event['shortDescription'],"'")); ?>
						</div>
					</div>
				</div>
			</div>
			<div class="clearfix"></div>
		<?php } ?>
		<div class="clearfix"></div>

		<div id="more-events-<?php print $page_callback_result['page']; ?>">
			<div class="item-list">
				<ul class="pager pager-load-more">
					<li class="pager-next first last">
						<?php $loadMoreURL = '/events/ajax/'.$page_callback_result['cat1'].'/'.$page_callback_result['cat2'].'/'.$page_callback_result['page']; ?>
						<button id="communicoAjaxTrigger" onclick="communicoData_ajax_load('more-events-<?php print $page_callback_result['page']; ?>','<?php print $loadMoreURL; ?>')">Load more</button>
					</li>
				</ul>
			</div>
		</div>

		<?php $htmlOutput = ob_get_clean();
		ob_end_clean();
		print $htmlOutput;
	}
}

function communicoData_menu() {
	$items['events/%/%'] = array(
		'title callback' => 'HPL Events',
		'page callback' => 'communicoData_events_handler',
		'page arguments' => array(1,2),
		'access callback' => TRUE,
		'type' => MENU_CALLBACK,
	);

	//LOAD MORE BUTTON ON EVENTS PAGES
	$items['events/ajax/%/%/%'] = array(
		'page callback' => 'communicoData_events_ajax_handler',
		'page arguments' => array(2,3,4),
		'access callback' => TRUE,
		'delivery callback' => 'communcioData_ajax_callback',
		'type' => MENU_CALLBACK,
	);

	//EVENT SEARCH PAGE
	$items['eventsearch/%'] = array(
		'page callback' => 'communicoData_search_handler',
		'page arguments' => array(1),
		'access callback' => TRUE,
		'type' => MENU_CALLBACK,
	);

	return $items;
}

function communicoData_theme() {
	return array(
		'communicoData_template' => array(
			'variables' => array(
				'jsonData' => null,
				'title' => null,
				'locations' => null,
				'page' => null,
			),
			'template' => 'communicoData--events',
		),
		'communicoData_search_template' => array(
			'variables' => array(
				'jsonData' => null,
				'locations' => null,
			),
			'template' => 'communicoData--search',
		),
	);
}

function communicoData_events_handler($cat1,$cat2) {
	drupal_add_js(drupal_get_path('module','communicoData').'/communicoData.js');
	//return theme('communicoData_template');
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

		//get the date to search from
		//$startDate = '2019-12-01';
		$startTime = date('Y-m-d H:i:s');
		$filterDateTime = urlencode($startTime);
		
		//page start count
		$page = 0;
		if(arg(3) !== null) { $page = arg(3); }  //the last arguemnt of the url if it is set, other wise set as 0
		$start = $page*20; //multiply the starting record by 20, since there are 20 records per page
	
		$tagSearch = '';
		//get the search tags
		if(strtolower($cat2) != 'all' || strtolower($cat1) == 'all') {
			$tagSearch = '&searchTags='.urlencode(str_replace('_','-',str_replace('-',' ',$cat2)));
		}

		$typeSearch = ''; //event types AKA parent events

		//custom march break age filter and query
		if($cat1 == 'march-break') {
			$ageFilter = '&ages=teens%20(12-18%20years),0-12%20months,0-24%20months,0-4%20years,0-5%20years,10-12%20years,13-24%20months,2-3%20years,3-5%20years,4-12%20years,4-8%20years,6-12%20years,8-12%20years';

			$startDateFilter = '2019-03-09';
			$endDateFilter = '2019-03-17';
			$filterDateTime = $startDateFilter;
		} else { //all other event type searches
			//Age filter based on URL
			$urlComps = explode('.',$_SERVER['HTTP_HOST']);
			$ageFilter = '';
			if($urlComps[0] == 'kids') {
				$ageFilter = '&ages=0-12%20months,0-24%20months,0-4%20years,0-5%20years,10-12%20years,13-24%20months,2-3%20years,3-5%20years,4-12%20years,4-8%20years,6-12%20years,8-12%20years';
			} else if ($urlComps[0] == 'teens') {
				$ageFilter = '&ages=teens%20(12-18%20years)';
			} 
			if(strtolower($cat1) != 'all') {
				$typeSearch = '&types='.urlencode(str_replace('-',' ',$cat1));
			}
		}

		//Get events for this URL
    	$queryURL = 'https://api.communico.co/v3/attend/events?start='.$start.'&registrationOpens='.$filterDateTime.'&privateEvents=false&limit=20&fields=modified&status=published&startDate='.$filterDateTime.$typeSearch.$tagSearch.$ageFilter;


    	curl_setopt($ch, CURLOPT_URL, $queryURL);
		$apiAnswer = curl_exec($ch);
		$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

		if ($httpcode >= 200 && $httpcode < 300) {
			//format the title from the args
			if($cat1 == 'all') {
				$title = ucwords(str_replace('_','-',str_replace('-',' ',$cat2)));
			} else {
				$title = ucwords(str_replace('-',' ',$cat1));
			}
			//Let's get the location info now so we aren't doing it if there was a problem
			$result = db_query("GET LOCATION INFORMATION");
		    
		    $locations = array();
		    while($item = $result->fetchAssoc()) {
		    	$locations[$item['title']] = array('street'=>$item['street'],'city'=>$item['city'],'lat'=>$item['latitude'],'lon'=>$item['longitude']);
		    	//watchdog('$item','<pre>'.print_r($item, true).'</pre>');	
		    }

			//There was no error, so get the data and return it to the page
			return theme('communicoData_template',array('jsonData'=>$apiAnswer,'title'=>$title,'locations'=>$locations));
		} else {
			watchdog('communico http response','<pre>'.print_r($httpcode, true).'</pre>');
			watchdog('communico request answer','<pre>'.print_r($apiAnswer, true).'</pre>');
			//return 'error no data available.';
			return theme('communicoData_template',array('jsonData' => false,'title'=>false,'locations'=>false));
		}

    	curl_close($ch);
    }  else {
    	watchdog('communicoData_page','Communico API failed Authorization');
    	//return 'error no data available.';
    	return theme('communicoData_template',array('jsonData' => false,'title'=>false,'locations'=>false));
    }
}

function communicoData_events_ajax_handler($cat1,$cat2,$page) {
	//return theme('communicoData_template');
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

		//get the date to search from
		//$startDate = '2019-12-01';
		$startTime = date('Y-m-d H:i:s');
		$filterDateTime = urlencode($startTime);
		
		//page start count
		$start = $page*20; //multiply the starting record by 20, since there are 20 records per page
	
		$tagSearch = '';
		//get the search tags
		if(strtolower($cat2) != 'all' || strtolower($cat1) == 'all') {
			$tagSearch = '&searchTags='.urlencode(str_replace('_','-',str_replace('-',' ',$cat2)));
		}

		$typeSearch = ''; //event types AKA parent events

		//custom march break age filter and query
		if($cat1 == 'march-break') {
			$ageFilter = '&ages=teens%20(12-18%20years),0-12%20months,0-24%20months,0-4%20years,0-5%20years,10-12%20years,13-24%20months,2-3%20years,3-5%20years,4-12%20years,4-8%20years,6-12%20years,8-12%20years';

			$startDateFilter = '2019-03-09';
			$endDateFilter = '2019-03-17';
			$filterDateTime = $startDateFilter;
		} else { //all other event type searches
			//Age filter based on URL
			$urlComps = explode('.',$_SERVER['HTTP_HOST']);
			$ageFilter = '';
			if($urlComps[0] == 'kids') {
				$ageFilter = '&ages=0-12%20months,0-24%20months,0-4%20years,0-5%20years,10-12%20years,13-24%20months,2-3%20years,3-5%20years,4-12%20years,4-8%20years,6-12%20years,8-12%20years';
			} else if ($urlComps[0] == 'teens') {
				$ageFilter = '&ages=teens%20(12-18%20years)';
			} 
			if(strtolower($cat1) != 'all') {
				$typeSearch = '&types='.urlencode(str_replace('-',' ',$cat1));
			}
		}

		//Get events for this URL
    	$queryURL = 'https://api.communico.co/v3/attend/events?start='.$start.'&registrationOpens='.$filterDateTime.'&privateEvents=false&limit=20&fields=modified&status=published&startDate='.$filterDateTime.$typeSearch.$tagSearch.$ageFilter;

		
    	curl_setopt($ch, CURLOPT_URL, $queryURL);
		$apiAnswer = curl_exec($ch);
		$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

		if ($httpcode >= 200 && $httpcode < 300) {
			//format the title from the args
			if($cat1 == 'all') {
				$title = ucwords(str_replace('_','-',str_replace('-',' ',$cat2)));
			} else {
				$title = ucwords(str_replace('-',' ',$cat1));
			}
			//Let's get the location info now so we aren't doing it if there was a problem
			$result = db_query("GET LOCATION INFORMATION");
		    
		    $locations = array();
		    while($item = $result->fetchAssoc()) {
		    	$locations[$item['title']] = array('street'=>$item['street'],'city'=>$item['city'],'lat'=>$item['latitude'],'lon'=>$item['longitude']);
		    	//watchdog('$item','<pre>'.print_r($item, true).'</pre>');	
		    }

			//There was no error, so get the data and return it to the page
			return array('jsonData'=>$apiAnswer,'title'=>$title,'locations'=>$locations,'cat1'=>$cat1,'cat2'=>$cat2,'page'=>$page+1);
		} else {
			watchdog('communico http response','<pre>'.print_r($httpcode, true).'</pre>');
			watchdog('communico request answer','<pre>'.print_r($apiAnswer, true).'</pre>');
			//return 'error no data available.';
			return false;
		}

    	curl_close($ch);
    }  else {
    	watchdog('communicoData_page','Communico API failed Authorization');
    	//return 'error no data available.';
    	return false;
    }
}

function communicoData_search_handler($searchTerm) {
	//drupal_add_js(drupal_get_path('module','communicoData').'/communicoData.js');
	//return theme('communicoData_template');
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

		//get the date to search from
		//$startDate = '2019-12-01';
		$startTime = date('Y-m-d H:i:s');
		$filterDateTime = urlencode($startTime);
		

		//Get events for this URL
    	$queryURL = 'https://api.communico.co/v3/attend/events?privateEvents=false&limit=20&fields=modified&status=published&startDate='.$filterDateTime.'&term='.$searchTerm;


    	curl_setopt($ch, CURLOPT_URL, $queryURL);
		$apiAnswer = curl_exec($ch);
		$httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

		if ($httpcode >= 200 && $httpcode < 300) {

			//Let's get the location info now so we aren't doing it if there was a problem
			$locations = communicoData_getLocations();

			//There was no error, so get the data and return it to the page
			return theme('communicoData_search_template',array('jsonData'=>$apiAnswer,'locations'=>$locations));
		} else {
			watchdog('communico http response','<pre>'.print_r($httpcode, true).'</pre>');
			watchdog('communico request answer','<pre>'.print_r($apiAnswer, true).'</pre>');
			//return 'error no data available.';
			return theme('communicoData_search_template',array('jsonData' => false,'locations'=>false));
		}

    	curl_close($ch);
    }  else {
    	watchdog('communicoData_page','Communico API failed Authorization');
    	//return 'error no data available.';
    	return theme('communicoData_search_template',array('jsonData' => false,'locations'=>false));
    }
}

function communicoData_getLocations() {
	$result = db_query("GET LOCATION INFORMATION");
    
    $locations = array();
    while($item = $result->fetchAssoc()) {
    	$locations[$item['title']] = array('street'=>$item['street'],'city'=>$item['city'],'lat'=>$item['latitude'],'lon'=>$item['longitude']);
    	//watchdog('$item','<pre>'.print_r($item, true).'</pre>');	
    }

    return $locations;
}