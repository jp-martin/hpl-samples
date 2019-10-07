<div id="communicoSearchResults">

<h1>Event Search Results</h1>
<?php 
$decodedJson = drupal_json_decode($jsonData);
if($decodedJson['data']['total'] == 0) {
	?><div>There are no Events matching that search. <a href="https://events.hpl.ca/events" title="events page">Please try the advanced search options on our events list page.</a></div></div><?php
} else {
	foreach($decodedJson['data']['entries'] as $event) { ?>
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
										<span class="street-address"><?php print $locations[$event['locationName']]['street']; ?></span>
										<span class="locality sr-only"><?php print $locations[$event['locationName']]['city']; ?></span>
										<span class="region sr-only">ON</span>
									</li>
									<li class="geo sr-only">
										<span class="latitude">
											<span class="value-title" title="<?php print $locations[$event['locationName']]['lat']; ?>"></span>
										</span>
										<span class="longitude">
											<span class="value-title" title="<?php print $locations[$event['locationName']]['lon']; ?>"></span>
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
		<div id="more-events">
			<div class="item-list">
				<ul class="pager pager-load-more">
					<li class="pager-next first last">
						<a href="https://events.hpl.ca/events">Didn't find what you are looking for? Try the advanced search</a>
					</li>
				</ul>
			</div>
		</div>
	</div>
<?php } ?>