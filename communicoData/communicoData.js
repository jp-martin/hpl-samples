function communicoData_ajax_load(target,ajaxURL) {
	jQuery('#'+target).load(ajaxURL);
	//jQuery('a#communicoAjaxTrigger').preventDefault();
}