<div class='breadcrumb'>
	<?php
	print theme('breadcrumb', array('breadcrumb'=>drupal_get_breadcrumb()));
	?>
</div>
<div id="bookmobileAllStops">

	<h1>HPL Bookmobile Stops</h1>

	<div id="bookmobileStopsList" class="view-id-bm_stop_list">
		<?php
		//print_r($stops);
		//$stops[$key] = array('street'=>$value['street'],'stopStart'=>$startDate,'stopEnd'=>$stopDate);
		$row = 0;
		foreach($stops as $key => $value) {
			$evenOdd = ($row++%2)?'views-row-even':'views-row-odd';
			print '<div class="bookmobileStop '.$evenOdd.'">';
				print '<div class="bookmobile30">'.$key.'</div>';
				print '<div class="bookmobile30">'.$value['street'].'</div>';
				print '<div class="bookmobile30">'.date('l, M d g:ia',strtotime($value['stopStart'])).'-'.date('g:ia',strtotime($value['stopEnd'])).'</div>';
			print '</div>';
		}
		?>
	</div>
</div>