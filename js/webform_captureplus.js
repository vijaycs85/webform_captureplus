/**
 * Initiates the capture plus object.
 */
(function ($) {
    Drupal.behaviors.webform_captureplus = Drupal.behaviors.webform_captureplus || {};
    Drupal.behaviors.webform_captureplus.attach = function() {
        // create quasi static var to save perfomance
        $.each(Drupal.settings, function(settings_id, settings) {
            if(settings_id.substring(0, 20) == 'webform_captureplus_') {
                $.each(settings, function(key, info){
                    info.control = new pca.Address(info.fields, info);
                });
            }
        });
        return;
    };

}(jQuery));
